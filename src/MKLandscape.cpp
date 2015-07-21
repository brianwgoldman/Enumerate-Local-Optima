/*
 * MKLandscape.cpp
 *
 *  Created on: Jul 20, 2015
 *      Author: goldman
 */

#include "MKLandscape.h"
#include <fstream>
#include <sstream>
#include <cassert>

// Temporary
using std::cout;
using std::endl;

MKLandscape::MKLandscape(string filename) : length(0) {
  std::ifstream input(filename);
  string line;
  char head;
  while (getline(input, line)) {
    // skip blank lines and commented out lines
    if (line.size() == 0 or line[0] == 'c') {
      continue;
    }
    std::istringstream line_stream(line);
    line_stream >> head;
    // If this line is the problem statement
    if (head == 'p') {
      string problem;
      line_stream >> problem;
      // ensure you have a file with the right format
      assert(problem == "MK" or problem == "NK");
      line_stream >> length;
      // ignore everything after the length. You don't need it
    } else if (head == 'm') {
      // creates a new subfunction
      subfunctions.emplace_back();
      size_t index;
      while (line_stream >> index) {
        subfunctions.back().variables.push_back(index);
      }
      // get the values line
      getline(input, line);
      std::istringstream values(line);
      double value;
      while (values >> value) {
        subfunctions.back().values.push_back(value);
      }
      // Verify that the the required number of values were read
      size_t expected_values = 1 << subfunctions.back().variables.size();
      assert(expected_values == subfunctions.back().values.size());
    } else {
      std::cerr << "Unexpected line header: " << head << std::endl;
    }
  }
  // Ensures that at some point the length was set correctly
  assert(length != 0);
  // Ensures that all variable indices are within the specified length
  for (const auto & subfunction : subfunctions) {
    for (const auto & variable_index : subfunction.variables) {
      assert (variable_index < length);
    }
  }
}

int MKLandscape::evaluate(size_t subfunction_index, const vector<bool>& solution) const {
  const auto& subfunction = subfunctions[subfunction_index];
  size_t index = 0;
  for (const auto& neighbor : subfunction.variables) {
    index = (index << 1) | solution[neighbor];
  }
  return subfunction.values[index];
}
