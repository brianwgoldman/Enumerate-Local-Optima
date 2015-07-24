/*
 * Enumeration.cpp
 *
 *  Created on: Jul 21, 2015
 *      Author: goldman
 */

#include "Enumeration.h"
#include <algorithm>
using std::cout;
using std::endl;

Enumeration::Enumeration(const MKLandscape & landscape_, size_t radius_)
    : landscape(landscape_),
      length(landscape_.get_length()),
      radius(radius_){
  start = std::chrono::steady_clock::now();
  auto graph = build_graph(landscape);
  moves = k_order_subgraphs(graph, radius);
  bit_to_sub.resize(length);

  const auto& subfunctions = landscape.get_subfunctions();
  for (size_t sub = 0; sub < subfunctions.size(); sub++) {
    for (const auto& bit : subfunctions[sub].variables) {
      bit_to_sub[bit].push_back(sub);
    }
  }

  sub_to_move.resize(subfunctions.size());
  move_to_sub.resize(moves.size());
  single_bit_moves.resize(length, -1);

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
  improving_moves = 0;
  reference.resize(length, false);
  org_to_new.resize(length);
  new_to_org.resize(length);
  iota(org_to_new.begin(), org_to_new.end(), 0);
  iota(new_to_org.begin(), new_to_org.end(), 0);
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
  // For each subfunction affected by this flip
  for (const auto& sub : bit_to_sub[index]) {
    auto pre_move = landscape.evaluate(sub, reference);
    reference[index] = not reference[index];  // Put in the flip
    auto just_move = landscape.evaluate(sub, reference);
    reference[index] = not reference[index];  // Take out move
    // for each move that overlaps the affected subfunction
    for (const auto& next : sub_to_move[sub]) {
      flip_move(next);  // Put in next
      auto just_next = landscape.evaluate(sub, reference);
      reference[index] = not reference[index];  // Put in move
      auto move_next = landscape.evaluate(sub, reference);
      reference[index] = not reference[index];  // Take out move
      flip_move(next);  // Take out next

      auto & bin = moves_in_bin[move_to_bin[next]];
      // if it was positive, remove it
      int was_improving = (delta[next] > 0);
      bin -= was_improving;
      improving_moves -= was_improving;
      // Take out old information and add in new information
      delta[next] += (pre_move - just_next + move_next - just_move);
      // if it is positive, add it
      int is_improving = (delta[next] > 0);
      bin += is_improving;
      improving_moves += is_improving;
    }
  }
  reference[index] = not reference[index];  // Put in move
  return fitness;
}

void Enumeration::remap() {
  vector<int> location(moves.size(), -1);
  vector<unordered_set<int>> move_bin(length + 1, unordered_set<int>());
  vector<vector<int>> bit_to_move(length, vector<int>());
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
  org_to_new.assign(length, -1);
  new_to_org.assign(length, -1);

  while (highest_available >= 0) {
    int move = -1;
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
            move_bin[current - 1].insert(affected);
            location[affected] = current - 1;
          }
          highest_available--;
        }
      }
    }
    move_bin[0].erase(move);
  }
}

void Enumeration::bin_moves() {
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
    int is_improving = (delta[move] > 0);
    moves_in_bin[min_dependency] += is_improving;
    improving_moves += is_improving;
  }
}

void Enumeration::enumerate(std::ostream& out, bool hyper, bool reorder) {
  reference.assign(length, false);

  initialize_deltas();
  if (reorder) {
    remap();
  }
  bin_moves();

  size_t count = 0;
  int pass = 1;
  int progress = -1;
  cout << "Pass " << pass << ": ";
  out << "# All " << radius << "-bit local optima. Hyper is " << (hyper? "on" : "off");
  out << ". Reorder is " << (reorder? "on": "off") << "." << endl;
  out << "# Fitness Representation" << endl;
  int i = length - 1;
  while (true) {
    if (hyper) {
      while (i > 0 and moves_in_bin[i] == 0) {
        i--;
      }
    } else {
      i = 0;
    }
    if (improving_moves == 0) {  // nothing needs to be flipped to be a local optimum
      out << fitness << " ";
      for (const auto bit : reference) {
        out << (bit==1);
      }
      out << endl;
      count++;
    }
    while (i < length and reference[new_to_org[i]]) {
      make_flip(new_to_org[i]);  // reference[i] = 0
      i++;
    }
    // End is reached
    if (i >= length) {
      cout << endl;
      auto current = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration<double>(current - start).count();
      out << "# Count: " << count << " Seconds: " << elapsed << endl;
      return;
    }
    make_flip(new_to_org[i]);  // reference[i] = 1
    // Everything below here is just for output purposes
    if (i > progress) {
      progress = i;
      cout << i << ", ";
      cout.flush();
      if (progress == length - pass) {
        progress = -1;
        pass++;
        cout << endl << "Pass " << pass << ": ";
      }
    }
  }
}
