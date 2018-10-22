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

#ifndef _FSTPROCESSOR_
#define _FSTPROCESSOR_

#include <lttoolbox/alphabet.h>
#include <lttoolbox/buffer.h>
#include <lttoolbox/ltstr.h>
#include <lttoolbox/state.h>
#include <lttoolbox/trans_exe.h>
#include <lttoolbox/my_stdio.h>
#include <clb/clb_stream.h>

#include <cwchar>
#include <map>
#include <queue>
#include <set>
#include <string>

using namespace std;

/**
 * Kind of output of the generator module
 */
enum GenerationMode
{
  gm_clean,      // clear all
  gm_unknown,    // display unknown words, clear transfer and generation tags
  gm_all,        // display all
  gm_tagged,     // tagged generation
  gm_tagged_nm,  // clean tagged generation
  gm_carefulcase // try lowercase iff no uppercase
};

/**
 * Class that implements the FST-based modules of the system
 */
class FSTProcessor
{
private:
  /**
   * Transducers in FSTP
   */
  map<wstring, TransExe, Ltstr> transducers;

  /**
   * Initial state of every token
   */
  State initial_state;

  /**
   * Default value of weight unless specified
   */
  double default_weight;

  /**
   * The final states of inconditional sections in the dictionaries
   */
  map<Node *, double> inconditional;

  /**
   * The final states of standard sections in the dictionaries
   */
  map<Node *, double> standard;

  /**
   * Merge of 'inconditional', 'standard', 'postblank' and 'preblank' sets
   */
  map<Node *, double> all_finals;

  /**
   * Set of characters being considered alphabetics
   */
  set<wchar_t> alphabetic_chars;

  /**
   * Set of characters to ignore
   */
  set<wchar_t> ignored_chars;

  /**
   * Alphabet
   */
  Alphabet alphabet;

  /**
   * Begin of the transducer
   */
  Node root;

  /**
   * Output no more than 'N' number of weighted analyses
   */
  int maxAnalyses;

  /**
   * Output no more than 'N' best weight classes
   */
  int maxWeightClasses;

  /**
   * Prints an error of input stream and exits
   */
  void streamError();

  /**
   * Returns true if the character code is identified as alphabetic
   * @param c the code provided by the user
   * @return true if it's alphabetic
   */
  bool isAlphabetic(wchar_t const c) const;

  /**
   * Read text from stream (analysis version, also used in postgeneration)
   * @param input_buffer the input buffer
   * @param input the stream to read
   * @return the next symbol in the stream
   */
  int readAnalysis(Buffer<int> &input_buffer, clb_stream_t input);

  /**
   * Calculate the initial state of parsing
   */
  void calcInitial();

  /**
   * Calculate all the results of the word being parsed
   */
  void classifyFinals();

  /**
   * Checks if an string ends with a particular suffix
   * @param str the string to test
   * @param the searched suffix
   * @returns true if 'str' has the suffix 'suffix'
   */
  static bool endsWith(wstring const &str, wstring const &suffix);

  unsigned int lastBlank(wstring const &str);
  size_t firstNotAlpha(wstring const &sf);

  void analysisIterattion(clb_stream_t &input, clb_writer_t out, State &current_state,
          wstring &sf, bool &lf, bool &last_incond,
          int &last, Buffer<int> &input_buffer, wchar_t val);

public:
  FSTProcessor();

  void initAnalysis();

  void analysis(clb_stream_t &input, clb_writer_t out);

  void load(istream &input);

  bool valid() const;
};

#endif
