/*
 * Copyright (C) 2005 Universitat d'Alacant / Universidad de Alicante
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#include <lttoolbox/state.h>

#include <cstring>
#include <cwctype>
#include <climits>
#include <algorithm>

State::State()
{
}

State::~State()
{
  destroy();
}

State::State(State const &s)
{
  copy(s);
}

State &
State::operator =(State const &s)
{
  if(this != &s)
  {
    destroy();
    copy(s);
  }

  return *this;
}

void
State::destroy()
{
  for(size_t i = 0, limit = state.size(); i != limit; i++)
  {
    delete state[i].sequence;
  }

  state.clear();
}

void
State::copy(State const &s)
{
  // release references
  for(size_t i = 0, limit = state.size(); i != limit; i++)
  {
    delete state[i].sequence;
  }

  state = s.state;

  for(size_t i = 0, limit = state.size(); i != limit; i++)
  {
    vector<pair<int, double>> *tmp = new vector<pair<int, double>>();
    *tmp = *(state[i].sequence);
    state[i].sequence = tmp;
  }
}

int
State::size() const
{
  return state.size();
}

void
State::init(Node *initial)
{
  state.clear();
  state.push_back(TNodeState(initial, new vector<pair<int, double>>(), false));
  state[0].sequence->clear();
  epsilonClosure();
}

void
State::apply(int const input)
{
  vector<TNodeState> new_state;
  if(input == 0)
  {
    state = new_state;
    return;
  }

  for(size_t i = 0, limit = state.size(); i != limit; i++)
  {
    map<int, Dest>::const_iterator it;
    it = state[i].where->transitions.find(input);
    if(it != state[i].where->transitions.end())
    {
      for(int j = 0; j != it->second.size; j++)
      {
        vector<pair<int, double>> *new_v = new vector<pair<int, double>>();
        *new_v = *(state[i].sequence);
        if(it->first != 0)
        {
          new_v->push_back(make_pair(it->second.out_tag[j], it->second.out_weight[j]));
        }
        new_state.push_back(TNodeState(it->second.dest[j], new_v, state[i].dirty||false));
      }
    }
    delete state[i].sequence;
  }

  state = new_state;
}

void
State::apply(int const input, int const alt)
{
  vector<TNodeState> new_state;
  if(input == 0 || alt == 0)
  {
    state = new_state;
    return;
  }


  for(size_t i = 0, limit = state.size(); i != limit; i++)
  {
    map<int, Dest>::const_iterator it;
    it = state[i].where->transitions.find(input);
    if(it != state[i].where->transitions.end())
    {
      for(int j = 0; j != it->second.size; j++)
      {
        vector<pair<int, double>> *new_v = new vector<pair<int, double>>();
        *new_v = *(state[i].sequence);
        if(it->first != 0)
        {
          new_v->push_back(make_pair(it->second.out_tag[j], it->second.out_weight[j]));
        }
        new_state.push_back(TNodeState(it->second.dest[j], new_v, state[i].dirty||false));
      }
    }
    it = state[i].where->transitions.find(alt);
    if(it != state[i].where->transitions.end())
    {
      for(int j = 0; j != it->second.size; j++)
      {
        vector<pair<int, double>> *new_v = new vector<pair<int, double>>();
        *new_v = *(state[i].sequence);
        if(it->first != 0)
        {
          new_v->push_back(make_pair(it->second.out_tag[j], it->second.out_weight[j]));
        }
        new_state.push_back(TNodeState(it->second.dest[j], new_v, true));
      }
    }
    delete state[i].sequence;
  }

  state = new_state;
}

void
State::epsilonClosure()
{
  for(size_t i = 0; i != state.size(); i++)
  {
    map<int, Dest>::iterator it2;
    it2 = state[i].where->transitions.find(0);
    if(it2 != state[i].where->transitions.end())
    {
      for(int j = 0 ; j != it2->second.size; j++)
      {
        vector<pair<int, double>> *tmp = new vector<pair<int, double>>();
        *tmp = *(state[i].sequence);
        if(it2->second.out_tag[j] != 0)
        {
          tmp->push_back(make_pair(it2->second.out_tag[j], it2->second.out_weight[j]));
        }
        state.push_back(TNodeState(it2->second.dest[j], tmp, state[i].dirty));
      }
    }
  }
}

void
State::step(int const input)
{
  apply(input);
  epsilonClosure();
}

void
State::step(int const input, int const alt)
{
  apply(input, alt);
  epsilonClosure();
}

bool
State::isFinal(map<Node *, double> const &finals) const
{
  for(size_t i = 0, limit = state.size(); i != limit; i++)
  {
    if(finals.find(state[i].where) != finals.end())
    {
      return true;
    }
  }

  return false;
}

/**
* Returns the lexical form in a sorted by weights manner
* and restricts to N analyses or N weight classes if those options are provided.
* @param the original lexical form map
* @param the max number of printable analyses
* @param the max number of printable weight classes
* @return the sorted lexical form
*/

template <typename T1, typename T2>
struct sort_weights {
  typedef pair<T1, T2> type;
  bool operator ()(type const& a, type const& b) const {
      return a.second < b.second;
  }
};

bool
State::filterFinals(map<Node *, double> const &finals,
                    Alphabet const &alphabet,
                    set<wchar_t> const &escaped_chars,
                    int max_analyses, int max_weight_classes,
                    clb_writer_t out) const
{
  bool result = false;
  vector<pair< size_t, double >> resp;

  for (size_t i = 0, limit = state.size(); i != limit; ++i)
  {
    if(finals.find(state[i].where) != finals.end())
    {
      wstring result = L"";
      double cost = 0.0000;
      // if(state[i].dirty)
      {
        // clb: unused
        // unsigned int const first_char = result.size() + firstchar;

        for(size_t j = 0, limit2 = state[i].sequence->size(); j != limit2; j++)
        {
          cost += ((*(state[i].sequence))[j]).second;
        }
      }

      resp.push_back(make_pair(i, cost));
    }
  }

  // response = NFinals(response, max_analyses, max_weight_classes);
  sort(resp.begin(), resp.end(), sort_weights<size_t, double>());
  for (vector<pair<size_t, double> >::iterator it = resp.begin(); it != resp.end(); ++it)
  {
    double last_weight = 0.0000;
    if (max_analyses > 0 && max_weight_classes > 0)
    {
      --max_analyses;
      if (last_weight != it->second)
      {
        --max_weight_classes;
      }

      // clb: looks like always equal to 0.0
      out.beginLF(it->second);
      size_t i = it->first;

      for (size_t j = 0, limit2 = state[i].sequence->size(); j != limit2; j++)
      {
        int sym = ((*(state[i].sequence))[j]).first;
        if (sym < 0)
        {
          wstring tag(L"");
          alphabet.getSymbol(tag, sym); // TBD: clb: move alphabet inside "out"
          out.tagLF(tag, sym);
        }
        else
        {
          wchar_t ch = static_cast<wchar_t>(sym);
          out.chLF(ch);
        }
      }

      out.endLF();
      result = true;
    }
    else break;
  }

  return result;
}
