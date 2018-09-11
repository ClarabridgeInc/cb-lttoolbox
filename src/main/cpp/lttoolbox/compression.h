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
#ifndef _COMPRESSION_
#define _COMPRESSION_

#include <cstdio>
#include <cstdint>
#include <string>
#include <iostream>

using namespace std;

// Global lttoolbox features
constexpr char HEADER_LTTOOLBOX[4]{'L', 'T', 'T', 'B'};
enum LT_FEATURES : uint32_t {
  LTF_UNKNOWN = (1u << 0), // Features >= this are unknown, so throw an error; Inc this if more features are added
  LTF_RESERVED = (1u << 31), // If we ever reach this many feature flags, we need a flag to know how to extend beyond 32 bits
};

// Invididual transducer features
constexpr char HEADER_TRANSDUCER[4]{'L', 'T', 'T', 'D'};
enum TD_FEATURES : uint32_t {
  TDF_WEIGHTS = (1u << 0),
  TDF_UNKNOWN = (1u << 1), // Features >= this are unknown, so throw an error; Inc this if more features are added
  TDF_RESERVED = (1u << 31), // If we ever reach this many feature flags, we need a flag to know how to extend beyond 32 bits
};

/**
 * Clase "Compression".
 * Class methods to access compressed data by the byte-aligned method
 */
class Compression
{
private:
  /**
   * Readinging a byte
   * @param input input stream
   * @return the value of the next byte in the input
   */
  static unsigned char readByte(FILE *input);

public:
  /**
   * Read and decode an integer from the input stream.
   * @see multibyte_read()
   * @param input input stream.
   * @return the integer value readed.
   */
  static unsigned int multibyte_read(FILE *input);

  /**
   * Read and decode an integer from the input stream.
   * @see multibyte_read()
   * @param input input stream.
   * @return the integer value readed.
   */
  static unsigned int multibyte_read(istream &is);

  /**
   * This method reads a wide string from the input stream.
   * @see wstring_write()
   * @param input the input stream.
   * @return the wide string read.
   */
  static wstring wstring_read(FILE *input);

  /**
   * This method reads a wide string from the input stream.
   * @see wstring_write()
   * @param input the input stream.
   * @return the wide string read.
   */
  static wstring wstring_read(istream &input);

  /**
   * This method reads a plain string from the input stream.
   * @see string_write()
   * @param input the input stream.
   * @return the string read.
   */
  static string string_read(FILE *input);

  /**
   * Read and decode a double from the input stream.
   * @see long_multibyte_read()
   * @param input input stream.
   * @return the double value read.
   */
  static double long_multibyte_read(FILE *input);

  /**
   * Read and decode a double from the input stream.
   * @see long_multibyte_read()
   * @param input input stream.
   * @return the double value read.
   */
  static double long_multibyte_read(istream &is);
};

#endif
