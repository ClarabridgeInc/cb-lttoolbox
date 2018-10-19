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
#ifndef _STATE_
#define _STATE_

#include <lttoolbox/alphabet.h>
#include <lttoolbox/node.h>
#include <clb/clb_stream.h>

#include <map>
#include <set>
#include <string>
#include <vector>
#include <climits>

using namespace std;

/**
 * Class to represent the current state of transducer processing
 */
class State
{
private:
  /**
   * The current state of transducer processing
   */
  struct TNodeState
  {
    Node *where;
    vector<pair<int, double>> *sequence;
    // a state is "dirty" if it was introduced at runtime (case variants, etc.)
    bool dirty;

    TNodeState(Node * const &w, vector<pair<int, double>> * const &s, bool const &d): where(w), sequence(s), dirty(d){}
    TNodeState & operator=(TNodeState const &other)
    {
      where = other.where;
      sequence = other.sequence;
      dirty = other.dirty;
      return *this;
    }
  };

  vector<TNodeState> state;

  /**
   * Destroy function
   */
  void destroy();

  /**
   * Make a transition, version for lowercase letters and symbols
   * @param input the input symbol
   */
  void apply(int const input);

  /**
   * Make a transition, version for lowercase and uppercase letters
   * @param input the input symbol
   * @param alt the alternative input symbol
   */
  void apply(int const input, int const alt);

  /**
   * Calculate the epsilon closure over the current state, replacing
   * its content.
   */
  void epsilonClosure();

public:
  /**
   * Copy function
   * @param s the state to be copied
   */
  void copy(State const &s);

  /**
   * Constructor
   */
  State();

  /**
   * Destructor
   */
  ~State();

  /**
   * Copy constructor
   * @param s the state to be copied
   */
  State(State const &s);

  /**
   * Assignment operator
   * @param s the state to be assigned
   * @return the object that results from the assignation
   */
  State & operator =(State const &s);

  /**
   * Number of alive transductions
   * @return the size
   */
  int size() const;

  /**
   * step = apply + epsilonClosure
   * @param input the input symbol
   */
  void step(int const input);

  /**
   * step = apply + epsilonClosure
   * @param input the input symbol
   * @param alt the alternative input symbol
   */
  void step(int const input, int const alt);

  /**
   * Init the state with the initial node and empty output
   * @param initial the initial node of the transducer
   */
  void init(Node *initial);

  /**
   * Returns true if at least one record of the state references a
   * final node of the set
   * @param finals set of final nodes @return
   * @true if the state is final
   */

  bool isFinal(map<Node *, double> const &finals) const;
  /**
   * All outputs of current parsing from the final nodes of the state
   * @param finals the set of final nodes
   * @param a the alphabet to decode strings
   * @return the result of the transduction
   */
  bool filterFinals(map<Node *, double> const &finals,
                    Alphabet const &a,
                    int max_analyses,
                    int max_weight_classes,
                    clb_writer_t out) const;
};

#endif
