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
   * The final states of postblank sections in the dictionaries
   */
  map<Node *, double> postblank;

  /**
   * The final states of preblank sections in the dictionaries
   */
  map<Node *, double> preblank;

  /**
   * Merge of 'inconditional', 'standard', 'postblank' and 'preblank' sets
   */
  map<Node *, double> all_finals;

  /**
   * Queue of blanks, used in reading methods
   */
  queue<wstring> blankqueue;

  /**
   * Set of characters being considered alphabetics
   */
  set<wchar_t> alphabetic_chars;

  /**
   * Set of characters to escape with a backslash
   */
  set<wchar_t> escaped_chars;

  /**
   * Set of characters to ignore
   */
  set<wchar_t> ignored_chars;

  /**
   * Mapping of characters for simplistic diacritic restoration specified in RCX files
   */
  map<int, set<int> > rcx_map;

  /**
   * Original char being restored
   */
  int rcx_current_char;

  /**
   * Alphabet
   */
  Alphabet alphabet;

  /**
   * Input buffer
   */
  Buffer<int> input_buffer;

  /**
   * Begin of the transducer
   */
  Node root;

  /**
   * if true, uses the dictionary case, discarding surface case
   * information
   */
  bool dictionaryCase;

  /**
   * if true, displays the final weights (if any)
   */
  bool displayWeightsMode;

  /**
   * try analysing unknown words as compounds
   */
  bool do_decomposition;

  /**
   * Symbol of CompoundOnlyL
   */
  int compoundOnlyLSymbol;

  /**
   * Symbol of CompoundR
   */
  int compoundRSymbol;

  /**
   * Max compound elements
   * Hard coded for now, but there might come a switch one day
   */
  int compound_max_elements;

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
   * Reads a character that is defined in the set of escaped_chars
   * @param input the stream to read from
   * @return code of the character
   */
  wchar_t readEscaped(clb_stream_t input);

  /**
   * Reads a block from the stream input, enclosed by delim1 and delim2
   * @param input the stream being read
   * @param delim1 the delimiter of the beginning of the sequence
   * @param delim1 the delimiter of the end of the sequence
   */
  wstring readFullBlock(clb_stream_t input, wchar_t const delim1, wchar_t const delim2);

  /**
   * Returns true if the character code is identified as alphabetic
   * @param c the code provided by the user
   * @return true if it's alphabetic
   */
  bool isAlphabetic(wchar_t const c) const;

  /**
   * Tests if a character is in the set of escaped_chars
   * @param c the character code provided by the user
   * @return true if it is in the set
   */
  bool isEscaped(wchar_t const c) const;

  /**
   * Read text from stream (analysis version, also used in postgeneration)
   * @param input the stream to read
   * @return the next symbol in the stream
   */
  int readAnalysis(clb_stream_t input);

  /**
   * Flush all the blanks remaining in the current process
   * @param output stream to write blanks
   */
  void flushBlanks(FILE *output);

  /**
   * Calculate the initial state of parsing
   */
  void calcInitial();

  /**
   * Calculate all the results of the word being parsed
   */
  void classifyFinals();

  /**
   * Write a string to an output stream,
   * @param str the string to write, escaping characters
   * @param output the stream to write in
   */
  void writeEscaped(wstring const &str, FILE *output);

  /**
   * Checks if an string ends with a particular suffix
   * @param str the string to test
   * @param the searched suffix
   * @returns true if 'str' has the suffix 'suffix'
   */
  static bool endsWith(wstring const &str, wstring const &suffix);

  /**
   * Prints a word
   * @param sf surface form of the word
   * @param lf lexical form of the word
   * @param output stream where the word is written
   */
  void printWord(wstring const &sf, wstring const &lf, FILE *output);

  /**
   * Prints an unknown word
   * @param sf surface form of the word
   * @param output stream where the word is written
   */
  void printUnknownWord(wstring const &sf, FILE *output);

  unsigned int lastBlank(wstring const &str);
  void printSpace(wchar_t const val, FILE *output);
  wstring compoundAnalysis(wstring str, bool uppercase, bool firstupper);
  size_t firstNotAlpha(wstring const &sf);

  void analysis_wrapper_null_flush(istream &input, FILE *output);

public:
  FSTProcessor();

  void initAnalysis();

  void analysis(clb_stream_t input, FILE *output = stdout);

  void load(istream &input);

  bool valid() const;

  void setDictionaryCaseMode(bool const value);
};

#endif
