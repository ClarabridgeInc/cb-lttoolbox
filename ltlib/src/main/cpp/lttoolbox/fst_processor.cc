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
#include <lttoolbox/fst_processor.h>
#include <lttoolbox/compression.h>
#include <lttoolbox/exception.h>

#include <cerrno>
#include <climits>
#include <cstring>
#include <iostream>

using namespace std;

FSTProcessor::FSTProcessor()
:
    default_weight(0.0000)
{
  maxAnalyses = INT_MAX;
  maxWeightClasses = INT_MAX;

  ignored_chars.insert(173); // '\u00AD', soft hyphen
}

void
FSTProcessor::streamError()
{
    throw LttException("Error: Malformed input stream.");
}

void
FSTProcessor::calcInitial()
{
  for(map<wstring, TransExe, Ltstr>::iterator it = transducers.begin(),
                                             limit = transducers.end();
      it != limit; it++)
  {
    root.addTransition(0, 0, it->second.getInitial(), default_weight);
  }

  initial_state.init(&root);
}

bool
FSTProcessor::endsWith(wstring const &str, wstring const &suffix)
{
  if(str.size() < suffix.size())
  {
    return false;
  }
  else
  {
    return str.substr(str.size()-suffix.size()) == suffix;
  }
}

void
FSTProcessor::classifyFinals()
{
  for(map<wstring, TransExe, Ltstr>::iterator it = transducers.begin(),
                                             limit = transducers.end();
      it != limit; it++)
  {
    if(endsWith(it->first, L"@inconditional"))
    {
      inconditional.insert(it->second.getFinals().begin(),
                           it->second.getFinals().end());
    }
    else if(endsWith(it->first, L"@standard"))
    {
      standard.insert(it->second.getFinals().begin(),
                      it->second.getFinals().end());
    }
    else
    {
      wcerr << L"Error: Unsupported transducer type for '";
      wcerr << it->first << L"'." << endl;
      exit(EXIT_FAILURE);
    }
  }
}

unsigned int
FSTProcessor::lastBlank(wstring const &str)
{
  for(int i = static_cast<int>(str.size())-1; i >= 0; i--)
  {
    if(alphabetic_chars.find(str[i]) == alphabetic_chars.end())
    {
      return static_cast<unsigned int>(i);
    }
  }
  return 0;
}

bool
FSTProcessor::isAlphabetic(wchar_t const c) const
{
  return alphabetic_chars.find(c) != alphabetic_chars.end();
}

void
FSTProcessor::load(istream &input)
{
  std::istream::pos_type cur_pos = input.tellg();
  if (cur_pos == 0) {
      char header[4]{};
      input.read(header, 4);
      if (strncmp(header, HEADER_LTTOOLBOX, 4) == 0) {
          auto features = Compression::multibyte_read(input);
          if (features >= LTF_UNKNOWN) {
              throw std::runtime_error("FST has features that are unknown to this version of lttoolbox - upgrade!");
          }
      }
      else {
          // Old binary format
          input.seekg(0, input.beg);
      }
  }

  // letters
  int len = Compression::multibyte_read(input);
  while(len > 0)
  {
    alphabetic_chars.insert(static_cast<wchar_t>(Compression::multibyte_read(input)));
    len--;
  }

  // symbols
  alphabet.read(input);

  len = Compression::multibyte_read(input);

  while(len > 0)
  {
    int len2 = Compression::multibyte_read(input);
    wstring name = L"";
    while(len2 > 0)
    {
      name += static_cast<wchar_t>(Compression::multibyte_read(input));
      len2--;
    }
    transducers[name].read(input, alphabet);
    len--;
  }
}

void
FSTProcessor::initAnalysis()
{
  calcInitial();
  classifyFinals();
  all_finals = standard;
  all_finals.insert(inconditional.begin(), inconditional.end());
}

// TBD: clb: hypothesis - looks like thre result is always >= 0 ???
int
FSTProcessor::readAnalysis(Buffer<int> &input_buffer, clb_stream_t &input)
{
  if(!input_buffer.isEmpty())
  {
    return input_buffer.next();
  }

  wchar_t val = input.getWC();
  if(input.eof())
  {
    return 0;
  }

  // TBD: clb: maybe use while instead of if here
  if(ignored_chars.find(val) != ignored_chars.end())
  {
    input_buffer.add(val);
    val = input.getWC();
  }

  input_buffer.add(val);
  return val;
}

void
FSTProcessor::analysisIterattion(clb_stream_t &input, clb_writer_t out, State &current_state,
        wstring &sf, bool &lf, bool &last_incond,
        int &last, Buffer<int> &input_buffer, wchar_t val)
{
    // test for final states
    if(current_state.isFinal(all_finals))
    {
      if(current_state.isFinal(inconditional))
      {
        lf = current_state.filterFinals(all_finals, alphabet, maxAnalyses, maxWeightClasses, out);
        last_incond = true;
        last = input_buffer.getPos();
      }
      else if(!isAlphabetic(val))
      {
        lf = current_state.filterFinals(all_finals, alphabet, maxAnalyses, maxWeightClasses, out);
        last_incond = false;
        last = input_buffer.getPos();
      }
    }
    else if(sf == L"" && iswspace(val))
    {
      // lf = L"/*";
      // lf.append(sf); // sf == L""
      lf = true;
      last_incond = false;
      last = input_buffer.getPos();
    }

    {
      if(!iswupper(val))
      {
        current_state.step(val);
      }
      else
      {
        current_state.step(val, towlower(val));
      }
    }

    if(current_state.size() != 0)
    {
      alphabet.getSymbol(sf, val);
    }
    else
    {
      if(!isAlphabetic(val) && sf == L"")
      {
          out.ch(val); // clb: not-escaped
      }
      else if(last_incond)
      {
        // clb: sentence end
        out.word(sf.substr(0, sf.size()-input_buffer.diffPrevPos(last)));
        input_buffer.setPos(last);
        input_buffer.back(1);
      }
      else if(isAlphabetic(val) &&
              ((sf.size()-input_buffer.diffPrevPos(last)) > lastBlank(sf) ||
               !lf))
      {
        do
        {
          alphabet.getSymbol(sf, val);
        }
        while((val = readAnalysis(input_buffer, input)) && isAlphabetic(val));

        unsigned int limit = firstNotAlpha(sf);
        unsigned int size = sf.size();
        limit = (limit == static_cast<unsigned int>(wstring::npos)?size:limit);
        if(limit == 0)
        {
          input_buffer.back(sf.size());

          // TBD: clb: hard-to-find-steps:  first symbol of sf is not-alpha or sf is empty
          // std::wcout << std::endl << L"esc-sp-char1: " << sf.substr(0,1) << std::endl;

          out.ch(sf.at(0)); // sf.substr(0,1) == sf.at[0] - single char
        }
        else
        {
          input_buffer.back(1+(size-limit));
          wstring unknown_word = sf.substr(0, limit);

          {
            out.unknownWord(unknown_word);
          }
        }
      }
      else if(!lf)
      {
        unsigned int limit = firstNotAlpha(sf);
        unsigned int size = sf.size();
        limit = (limit == static_cast<unsigned int >(wstring::npos)?size:limit);
        if(limit == 0)
        {
          input_buffer.back(sf.size());

          // TBD: clb: hard-to-find-steps:  first symbol of sf is not-alpha or sf is empty
          // std::wcout << std::endl << L"esc-sp-char2: " << sf.substr(0,1) << std::endl;

          out.ch(sf.at(0)); // s.substr(0,1) == s.at(0) - single char
        }
        else
        {
          input_buffer.back(1+(size-limit));
          wstring unknown_word = sf.substr(0, limit);

          {
            out.unknownWord(unknown_word);
          }
        }
      }
      else
      {
        out.word(sf.substr(0, sf.size()-input_buffer.diffPrevPos(last)));
        input_buffer.setPos(last);
        input_buffer.back(1);
      }

      current_state = initial_state;
      sf = L"";
      lf = false;
      last_incond = false;
    }
}

void
FSTProcessor::analysis(clb_stream_t &input, clb_writer_t out)
{
  State current_state = initial_state;
  bool last_incond = false;
  wstring sf = L"";   //surface form
  bool lf = false;    //lexical form (lema) found
  int last = 0;
  Buffer<int> input_buffer; // clb: this buffer can recycle !

  while (wchar_t val = readAnalysis(input_buffer, input))
  {
      analysisIterattion(input, out, current_state, sf, lf, last_incond, last, input_buffer, val);
  }

  while (last_incond)
  {
      analysisIterattion(input, out, current_state, sf, lf, last_incond, last, input_buffer, ' ');
  }

  if (out.isLFOpen())
  {
      analysisIterattion(input, out, current_state, sf, lf, last_incond, last, input_buffer, '.');
  }

  out.done(); 
}

bool
FSTProcessor::valid() const
{
  if(initial_state.isFinal(all_finals))
  {
    wcerr << L"Error: Invalid dictionary (hint: the left side of an entry is empty)" << endl;
    return false;
  }
  else
  {
    State s = initial_state;
    s.step(L' ');
    if(s.size() != 0)
    {
      wcerr << L"Error: Invalid dictionary (hint: entry beginning with whitespace)" << endl;
      return false;
    }
  }

  return true;
}

size_t
FSTProcessor::firstNotAlpha(wstring const &sf)
{
  for(size_t i = 0, limit = sf.size(); i < limit; i++)
  {
    if(!isAlphabetic(sf[i]))
    {
      return i;
    }
  }

  return wstring::npos;
}
