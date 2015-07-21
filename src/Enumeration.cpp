/*
 * Enumeration.cpp
 *
 *  Created on: Jul 21, 2015
 *      Author: goldman
 */

#include "Enumeration.h"

Enumeration::Enumeration(const MKLandscape & landscape_, size_t radius) : landscape(landscape_) {
  start = std::chrono::steady_clock::now();
  auto graph = build_graph(landscape);
  moves = k_order_subgraphs(graph, radius);
  bit_to_sub.resize(landscape.get_length());

  const auto& subfunctions = landscape.get_subfunctions();
  for (size_t sub = 0; sub < subfunctions.size(); sub++) {
    for (const auto& bit : subfunctions[sub].variables) {
      bit_to_sub[bit].push_back(sub);
    }
  }

  sub_to_move.resize(subfunctions.size());
  move_to_sub.resize(moves.size());
  single_bit_moves.resize(landscape.get_length(), -1);

  for (size_t m = 0; m < moves.size(); m++) {
    if (moves[m].size() == 1) {
      single_bit_moves[moves[m][0]] = m;
    }
    for (const auto& bit : moves[m]) {
      for (const auto& sub : bit_to_sub[bit]) {
        move_to_sub[m].insert(sub);
        sub_to_move[sub].insert(m);
      }
    }
  }

  // Table for tracking the fitness effects of making each move
  delta.resize(moves.size(), 0);
  fitness = 0;
  reference.resize(landscape.get_length(), false);
}

void Enumeration::initialize_deltas() {
  fitness = 0;
  // fill delta with 0s
  delta.assign(delta.size(), 0);
  const size_t number_of_subs = landscape.get_subfunctions().size();
  for (size_t sub = 0; sub < number_of_subs; sub++) {
    auto score = landscape.evaluate(sub, reference);
    fitness += score;
    // Update the effect each move has on this subfunction
    for (const auto& move : sub_to_move[sub]) {
      delta[move] -= score;
      flip_move(move);
      delta[move] += landscape.evaluate(sub, reference);
      flip_move(move);
    }
  }
}

// Internal function to flip all bits associated with a move
void Enumeration::flip_move(size_t move_index) {
  for (const auto& bit : moves[move_index]) {
    reference[bit] = not reference[bit];
  }
}

// Given the index of a move, apply it to the solution
// and update auxiliary information.
int Enumeration::make_flip(size_t index) {
  // update fitness and record it
  fitness += delta[single_bit_moves[index]];
  // For each subfunction effected by this flip
  for (const auto& sub : bit_to_sub[index]) {
    auto pre_move = landscape.evaluate(sub, reference);
    reference[index] = not reference[index];  // Put in the flip
    auto just_move = landscape.evaluate(sub, reference);
    reference[index] = not reference[index];  // Take out move
    // for each move that overlaps the effected subfunction
    for (const auto& next : sub_to_move[sub]) {
      flip_move(next);  // Put in next
      auto just_next = landscape.evaluate(sub, reference);
      reference[index] = not reference[index];  // Put in move
      auto move_next = landscape.evaluate(sub, reference);
      reference[index] = not reference[index];  // Take out move
      flip_move(next);  // Take out next

      auto & bin = moves_in_bin[move_to_bin[next]];
      // if it was positive, remove it
      bin -= (delta[next] > 0);
      // Take out old information and add in new information
      delta[next] += (pre_move - just_next + move_next - just_move);
      // if it is positive, add it
      bin += (delta[next] > 0);
    }
  }
  reference[index] = not reference[index];  // Put in move
  return fitness;
}

void Enumeration::remap() {
  int length = landscape.get_length();
  vector<int> location(moves.size(), -1);
  vector<unordered_set<int>> move_bin(length + 1);
  vector<vector<int>> bit_to_move(length);
  for (size_t move = 0; move < moves.size(); move++) {
    unordered_set<int> depends;
    for (const auto& sub : move_to_sub[move]) {
      for (int bit : landscape.get_subfunctions()[sub].variables) {
        depends.insert(bit);
      }
    }
    move_bin[depends.size()].insert(move);
    location[move] = depends.size();
    for (const auto& bit : depends) {
      bit_to_move[bit].push_back(move);
    }
  }

  int highest_available = length - 1;
  org_to_new = vector<int>(length, -1);
  new_to_org = vector<int>(length, -1);

  while (highest_available >= 0) {
    int move=-1;
    for (const auto& bin : move_bin) {
      if (bin.size()) {
        move = *bin.begin();
        break;
      }
    }
    if (move == -1) {
      break;
    }

    for (const auto& sub : move_to_sub[move]) {
      for (int bit : landscape.get_subfunctions()[sub].variables) {
        if (org_to_new[bit] == -1) {
          org_to_new[bit] = highest_available;
          new_to_org[highest_available] = bit;
          for (const auto& affected : bit_to_move[bit]) {
            int current = location[affected];
            move_bin[current].erase(affected);
            move_bin[current-1].insert(affected);
            location[affected] = current-1;
          }
          highest_available--;
        }
      }
    }
    move_bin[0].erase(move);
  }
}


void Enumeration::bin_moves() {
  int length = landscape.get_length();
  move_to_bin.resize(moves.size());
  moves_in_bin.resize(length);
  for (size_t move = 0; move < moves.size(); move++) {
    int min_dependency = length;
    for (const auto& sub : move_to_sub[move]) {
      for (int bit : landscape.get_subfunctions()[sub].variables) {
        if (min_dependency > org_to_new[bit]) {
          min_dependency = org_to_new[bit];
        }
      }
    }
    // Assign the move to a bin
    move_to_bin[move] = min_dependency;
    // Increment the bin in this move is fitness improving
    moves_in_bin[min_dependency] += (delta[move] > 0);
  }
}

using std::cout;
using std::endl;
void Enumeration::enumerate(std::ostream& out) {

  int length = landscape.get_length();

  reference.assign(length, false);

  initialize_deltas();
  remap();
  bin_moves();


  size_t count = 0;
  int pass=1;
  int progress = -1;
  cout << "Pass " << pass << ": ";
  int i=length - 1;
  while (true) {
    while (i >= 0 and moves_in_bin[i] == 0) {
      i--;
    }
    if (i == -1) { // nothing needs to be flipped to be a local optimum
      out << fitness << " ";
      for (const auto bit : reference) {
        out << bit;
      }
      out << endl;
      count++;
      i = 0;
    }
    while (i < length and reference[new_to_org[i]]) {
      make_flip(new_to_org[i]); // reference[i] = 0
      i++;
    }
    // End is reached
    if (i >= length) {
      cout << endl;
      auto current = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration<double>(current - start).count();
      out << "Count: " << count << " Elapsed: " << elapsed << endl;
      return;
    }
    make_flip(new_to_org[i]); // reference[i] = 1
    // output stuff
    if (i > progress) {
      progress=i;
      cout << i << ", ";
      cout.flush();
      if (progress==length-pass) {
        progress=-1;
        pass++;
        cout << endl << "Pass " << pass << ": ";
      }
    }
  }
}
