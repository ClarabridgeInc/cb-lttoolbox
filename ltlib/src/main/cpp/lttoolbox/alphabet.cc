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
#include <lttoolbox/alphabet.h>
#include <lttoolbox/compression.h>
#include <lttoolbox/my_stdio.h>

#include <cctype>
#include <cstdlib>
#include <set>
#include <cwchar>
#include <cwctype>

#ifdef PRINT_ALPHABET_READ
#   include <iostream>
#endif

using namespace std;

Alphabet::Alphabet()
{
  spair[pair<int, int>(0,0)] = 0;
  spairinv.push_back(pair<int, int>(0,0));
}

Alphabet::~Alphabet()
{
  destroy();
}

Alphabet::Alphabet(Alphabet const &a)
{
  copy(a);
}

Alphabet &
Alphabet::operator =(Alphabet const &a)
{
  if(this != &a)
  {
    destroy();
    copy(a);
  }
  return *this;
}

void
Alphabet::destroy()
{
}

void
Alphabet::copy(Alphabet const &a)
{
  slexic = a.slexic;
  slexicinv = a.slexicinv;
  spair = a.spair;
  spairinv = a.spairinv;
}

int
Alphabet::operator()(int const c1, int const c2)
{
  auto tmp = make_pair(c1, c2);
  if(spair.find(tmp) == spair.end())
  {
    int spair_size = spair.size();
    spair[tmp] = spair_size;
    spairinv.push_back(tmp);
  }

  return spair[tmp];
}

int
Alphabet::operator()(wstring const &s)
{
  return slexic[s];
}

int
Alphabet::operator()(wstring const &s) const
{
  auto it = slexic.find(s);
  if (it == slexic.end()) {
    return -1;
  }
  return it->second;
}

void
Alphabet::read(istream &input)
{
  Alphabet a_new;
  a_new.spairinv.clear();
  a_new.spair.clear();

  // Reading of taglist
  int tam = Compression::multibyte_read(input);
  map<int, string> tmp;
  while(tam > 0)
  {
    tam--;
    wstring mytag = L"<" + Compression::wstring_read(input) + L">";
    a_new.slexicinv.push_back(mytag);
    int tag_id = -a_new.slexicinv.size();
    a_new.slexic[mytag]= tag_id;
#ifdef PRINT_ALPHABET_READ
    std::wcout << mytag << L": " << tag_id << std::endl;
#endif
  }

  // Reading of pairlist
  unsigned int bias = a_new.slexicinv.size();
  tam = Compression::multibyte_read(input);
  while(tam > 0)
  {
    tam--;
    int first = Compression::multibyte_read(input);
    int second = Compression::multibyte_read(input);
    pair<int, int> tmp(first - bias, second - bias);
    int spair_size = a_new.spair.size();
    a_new.spair[tmp] = spair_size;
    a_new.spairinv.push_back(tmp);
  }

  *this = a_new;
}

void
Alphabet::getSymbol(wstring &result, int const symbol) const
{
  if(symbol == 0)
  {
    return;
  }

  { // clb: if(!uppercase)
    if(symbol >= 0)
    {
      result += static_cast<wchar_t>(symbol);
    }
    else
    {
      result.append(slexicinv[-symbol-1]);
    }
  }
}

pair<int, int> const &
Alphabet::decode(int const code) const
{
  return spairinv[code];
}
