/*
 * Enumeration.h
 *
 *  Created on: Jul 21, 2015
 *      Author: goldman
 */

#ifndef ENUMERATION_H_
#define ENUMERATION_H_

#include "MKLandscape.h"
#include "GraphUtilities.h"
#include <ostream>
#include <chrono>

class Enumeration {
 public:
  Enumeration(const MKLandscape & landscape_, size_t radius_);
  void enumerate(std::ostream& out, bool hyper = true, bool reorder = true);
  int make_flip(size_t index);
 protected:
  const MKLandscape& landscape;
  int length, radius;
  vector<bool> reference;
  int fitness;
  // List of all moves, which are just collections of indices
  vector<vector<size_t>> moves;
  // Lookup tables to find which moves effect what subfunctions, and vice versa
  vector<unordered_set<size_t>> move_to_sub, sub_to_move;
  vector<vector<size_t>> bit_to_sub;
  vector<size_t> move_to_bin;
  vector<size_t> moves_in_bin;
  vector<size_t> single_bit_moves;
  // Table storing the fitness effect of making a particular move
  vector<int> delta;
  int improving_moves;

  void initialize_deltas();
  void flip_move(size_t move_index);

  vector<int> new_to_org, org_to_new;

  std::chrono::steady_clock::time_point start;
  void remap();
  void bin_moves();
};

#endif /* ENUMERATION_H_ */
