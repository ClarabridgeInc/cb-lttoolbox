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


FSTProcessor::FSTProcessor() :
default_weight(0.0000)
{
  // escaped_chars chars
  escaped_chars.insert(L'[');
  escaped_chars.insert(L']');
  escaped_chars.insert(L'{');
  escaped_chars.insert(L'}');
  escaped_chars.insert(L'^');
  escaped_chars.insert(L'$');
  escaped_chars.insert(L'/');
  escaped_chars.insert(L'\\');
  escaped_chars.insert(L'@');
  escaped_chars.insert(L'<');
  escaped_chars.insert(L'>');

  dictionaryCase = false;

  do_decomposition = false;
  displayWeightsMode = false;

  maxAnalyses = INT_MAX;
  maxWeightClasses = INT_MAX;
  compoundOnlyLSymbol = 0;
  compoundRSymbol = 0;
  compound_max_elements = 4;

  ignored_chars.insert(173); // '\u00AD', soft hyphen
}

void
FSTProcessor::streamError()
{
  throw Exception("Error: Malformed input stream.");
}

wchar_t
FSTProcessor::readEscaped(clb_stream_t &input)
{
  if(input.eof())
  {
    streamError();
  }

  wchar_t val = input.getWC();

  if(input.eof() || escaped_chars.find(val) == escaped_chars.end())
  {
    streamError();
  }

  return val;
}

wstring
FSTProcessor::readFullBlock(clb_stream_t &input, wchar_t const delim1, wchar_t const delim2)
{
  wstring result = L"";
  result += delim1;
  wchar_t c = delim1;

  while(!input.eof() && c != delim2)
  {
    c = input.getWC();
    result += c;
    if(c != L'\\')
    {
      continue;
    }
    else
    {
      result += static_cast<wchar_t>(readEscaped(input));
    }
  }

  if(c != delim2)
  {
    streamError();
  }

  return result;
}

int
FSTProcessor::readAnalysis(clb_stream_t &input)
{
  if(!input_buffer.isEmpty())
  {
    return input_buffer.next();
  }

  wchar_t val = input.getWC();
  int altval = 0;
  if(input.eof())
  {
    return 0;
  }

  if(ignored_chars.find(val) != ignored_chars.end())
  {
    input_buffer.add(val);
    val = input.getWC();
  }

  if(escaped_chars.find(val) != escaped_chars.end())
  {
    switch(val)
    {
      case L'<':
        altval = static_cast<int>(alphabet(readFullBlock(input, L'<', L'>')));
        input_buffer.add(altval);
        return altval;

      case L'[':
        blankqueue.push(readFullBlock(input, L'[', L']'));
        input_buffer.add(static_cast<int>(L' '));
        return static_cast<int>(L' ');

      case L'\\':
        val = input.getWC();
        if(escaped_chars.find(val) == escaped_chars.end())
        {
          streamError();
        }
        input_buffer.add(static_cast<int>(val));
        return val;

      default:
        streamError();
    }
  }

  input_buffer.add(val);
  return val;
}

void
FSTProcessor::flushBlanks(FILE *output)
{
  for(unsigned int i = blankqueue.size(); i > 0; i--)
  {
    fputws_unlocked(blankqueue.front().c_str(), output);
    blankqueue.pop();
  }
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
    else if(endsWith(it->first, L"@postblank"))
    {
      postblank.insert(it->second.getFinals().begin(),
                       it->second.getFinals().end());
    }
    else if(endsWith(it->first, L"@preblank"))
    {
      preblank.insert(it->second.getFinals().begin(),
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

void
FSTProcessor::writeEscaped(wstring const &str, FILE *output)
{
  for(unsigned int i = 0, limit = str.size(); i < limit; i++)
  {
    if(escaped_chars.find(str[i]) != escaped_chars.end())
    {
      fputwc_unlocked(L'\\', output);
    }
    fputwc_unlocked(str[i], output);
  }
}

void
FSTProcessor::printWord(wstring const &sf, wstring const &lf, FILE *output)
{
  fputwc_unlocked(L'^', output);
  writeEscaped(sf, output);
  fputws_unlocked(lf.c_str(), output);
  fputwc_unlocked(L'$', output);
}

void
FSTProcessor::printUnknownWord(wstring const &sf, FILE *output)
{
  fputwc_unlocked(L'^', output);
  writeEscaped(sf, output);
  fputwc_unlocked(L'/', output);
  fputwc_unlocked(L'*', output);
  writeEscaped(sf, output);
  fputwc_unlocked(L'$', output);
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

void
FSTProcessor::printSpace(wchar_t const val, FILE *output)
{
  if(blankqueue.size() > 0)
  {
    flushBlanks(output);
  }
  else
  {
    fputwc_unlocked(val, output);
  }
}

bool
FSTProcessor::isEscaped(wchar_t const c) const
{
  return escaped_chars.find(c) != escaped_chars.end();
}

bool
FSTProcessor::isAlphabetic(wchar_t const c) const
{
  return alphabetic_chars.find(c) != alphabetic_chars.end();
}

void
FSTProcessor::load(istream &input)
{
  if (input.tellg() == 0) {
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
  all_finals.insert(postblank.begin(), postblank.end());
  all_finals.insert(preblank.begin(), preblank.end());
}

wstring
FSTProcessor::compoundAnalysis(wstring input_word, bool uppercase, bool firstupper)
{
  const int MAX_COMBINATIONS = 32767;

  State current_state = initial_state;

  for(unsigned int i=0; i<input_word.size(); i++)
  {
    wchar_t val=input_word.at(i);

    current_state.step_case(val, false);

    if(current_state.size() > MAX_COMBINATIONS)
    {
      wcerr << L"Warning: compoundAnalysis's MAX_COMBINATIONS exceeded for '" << input_word << L"'" << endl;
      wcerr << L"         gave up at char " << i << L" '" << val << L"'." << endl;

      wstring nullString = L"";
      return  nullString;
    }

    if(i < input_word.size()-1)
    {
      current_state.restartFinals(all_finals, compoundOnlyLSymbol, &initial_state, '+');
    }

    if(current_state.size()==0)
    {
      wstring nullString = L"";
      return nullString;
    }
  }

  current_state.pruneCompounds(compoundRSymbol, '+', compound_max_elements);
  wstring result = current_state.filterFinals(all_finals, alphabet, escaped_chars, displayWeightsMode, maxAnalyses, maxWeightClasses, uppercase, firstupper);

  return result;
}

void
FSTProcessor::analysis(clb_stream_t &input, FILE *output)
{
  bool last_incond = false;
  bool last_postblank = false;
  bool last_preblank = false;
  State current_state = initial_state;
  wstring lf = L"";   //lexical form
  wstring sf = L"";   //surface form
  int last = 0;
  bool firstupper = false, uppercase = false;
  map<int, set<int> >::iterator rcx_map_ptr;

  while(wchar_t val = readAnalysis(input))
  {
    // test for final states
    if(current_state.isFinal(all_finals))
    {
      if(current_state.isFinal(inconditional))
      {
        if(!dictionaryCase)
        {
          firstupper = iswupper(sf[0]);
          uppercase = firstupper && iswupper(sf[sf.size()-1]);
        }

        if(do_decomposition && compoundOnlyLSymbol != 0)
        {
          current_state.pruneStatesWithForbiddenSymbol(compoundOnlyLSymbol);
        }
        lf = current_state.filterFinals(all_finals, alphabet,
                                        escaped_chars,
                                        displayWeightsMode, maxAnalyses, maxWeightClasses,
                                        uppercase, firstupper);
        last_incond = true;
        last = input_buffer.getPos();
      }
      else if(current_state.isFinal(postblank))
      {
        if(!dictionaryCase)
        {
          firstupper = iswupper(sf[0]);
          uppercase = firstupper && iswupper(sf[sf.size()-1]);
        }

        if(do_decomposition && compoundOnlyLSymbol != 0)
        {
          current_state.pruneStatesWithForbiddenSymbol(compoundOnlyLSymbol);
        }
        lf = current_state.filterFinals(all_finals, alphabet,
                                        escaped_chars,
                                        displayWeightsMode, maxAnalyses, maxWeightClasses,
                                        uppercase, firstupper);
        last_postblank = true;
        last = input_buffer.getPos();
      }
      else if(current_state.isFinal(preblank))
      {
        if(!dictionaryCase)
        {
          firstupper = iswupper(sf[0]);
          uppercase = firstupper && iswupper(sf[sf.size()-1]);
        }

        if(do_decomposition && compoundOnlyLSymbol != 0)
        {
          current_state.pruneStatesWithForbiddenSymbol(compoundOnlyLSymbol);
        }
        lf = current_state.filterFinals(all_finals, alphabet,
                                        escaped_chars,
                                        displayWeightsMode, maxAnalyses, maxWeightClasses,
                                        uppercase, firstupper);
        last_preblank = true;
        last = input_buffer.getPos();
      }
      else if(!isAlphabetic(val))
      {
        if(!dictionaryCase)
        {
          firstupper = iswupper(sf[0]);
          uppercase = firstupper && iswupper(sf[sf.size()-1]);
        }

        if(do_decomposition && compoundOnlyLSymbol != 0)
        {
          current_state.pruneStatesWithForbiddenSymbol(compoundOnlyLSymbol);
        }
        lf = current_state.filterFinals(all_finals, alphabet,
                                        escaped_chars,
                                        displayWeightsMode, maxAnalyses, maxWeightClasses,
                                        uppercase, firstupper);
        last_postblank = false;
        last_preblank = false;
        last_incond = false;
        last = input_buffer.getPos();
      }
    }
    else if(sf == L"" && iswspace(val))
    {
      lf = L"/*";
      lf.append(sf);
      last_postblank = false;
      last_preblank = false;
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
        if(iswspace(val))
        {
          printSpace(val, output);
        }
        else
        {
          if(isEscaped(val))
          {
            fputwc_unlocked(L'\\', output);
          }
          fputwc_unlocked(val, output);
        }
      }
      else if(last_postblank)
      {
        printWord(sf.substr(0, sf.size()-input_buffer.diffPrevPos(last)),
                  lf, output);
        fputwc_unlocked(L' ', output);
        input_buffer.setPos(last);
        input_buffer.back(1);
      }
      else if(last_preblank)
      {
        fputwc_unlocked(L' ', output);
        printWord(sf.substr(0, sf.size()-input_buffer.diffPrevPos(last)),
                  lf, output);
        input_buffer.setPos(last);
        input_buffer.back(1);
      }
      else if(last_incond)
      {
        printWord(sf.substr(0, sf.size()-input_buffer.diffPrevPos(last)),
                  lf, output);
        input_buffer.setPos(last);
        input_buffer.back(1);
      }
      else if(isAlphabetic(val) &&
              ((sf.size()-input_buffer.diffPrevPos(last)) > lastBlank(sf) ||
               lf == L""))
      {
        do
        {
          alphabet.getSymbol(sf, val);
        }
        while((val = readAnalysis(input)) && isAlphabetic(val));

        unsigned int limit = firstNotAlpha(sf);
        unsigned int size = sf.size();
        limit = (limit == static_cast<unsigned int>(wstring::npos)?size:limit);
        if(limit == 0)
        {
          input_buffer.back(sf.size());
          writeEscaped(sf.substr(0,1), output);
        }
        else
        {
          input_buffer.back(1+(size-limit));
          wstring unknown_word = sf.substr(0, limit);
          if(do_decomposition)
          {
            if(!dictionaryCase)
            {
              firstupper = iswupper(sf[0]);
              uppercase = firstupper && iswupper(sf[sf.size()-1]);
            }

            wstring compound = L"";
            compound = compoundAnalysis(unknown_word, uppercase, firstupper);
            if(compound != L"")
            {
              printWord(unknown_word, compound, output);
            }
            else
            {
              printUnknownWord(unknown_word, output);
            }
          }
          else
          {
            printUnknownWord(unknown_word, output);
          }
        }
      }
      else if(lf == L"")
      {
        unsigned int limit = firstNotAlpha(sf);
        unsigned int size = sf.size();
        limit = (limit == static_cast<unsigned int >(wstring::npos)?size:limit);
        if(limit == 0)
        {
          input_buffer.back(sf.size());
          writeEscaped(sf.substr(0,1), output);
        }
        else
        {
          input_buffer.back(1+(size-limit));
          wstring unknown_word = sf.substr(0, limit);
          if(do_decomposition)
          {
            if(!dictionaryCase)
            {
              firstupper = iswupper(sf[0]);
              uppercase = firstupper && iswupper(sf[sf.size()-1]);
            }

            wstring compound = L"";
            compound = compoundAnalysis(unknown_word, uppercase, firstupper);
            if(compound != L"")
            {
              printWord(unknown_word, compound, output);
            }
            else
            {
              printUnknownWord(unknown_word, output);
            }
          }
          else
          {
            printUnknownWord(unknown_word, output);
          }
        }
      }
      else
      {
        printWord(sf.substr(0, sf.size()-input_buffer.diffPrevPos(last)), lf, output);
        input_buffer.setPos(last);
        input_buffer.back(1);
      }

      current_state = initial_state;
      lf = L"";
      sf = L"";
      last_incond = false;
      last_postblank = false;
      last_preblank = false;
    }
  }

  // print remaining blanks
  flushBlanks(output);
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

void
FSTProcessor::setDictionaryCaseMode(bool const value)
{
  dictionaryCase = value;
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
