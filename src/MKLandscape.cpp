// Brian Goldman

// Implements an MK Landscape, which is any problem that
// can be expressed as the sum of M subfunctions,
// each of which read at most k problem variables.

#include "MKLandscape.h"
#include <fstream>
#include <sstream>
#include <cassert>

MKLandscape::MKLandscape(string filename)
    : length(0) {
  std::ifstream input(filename);
  string line;
  // First character of the line is used to specify its purpose
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
      // read in the problem
      string problem;
      line_stream >> problem;
      // ensure you have a file with the right format
      assert(problem == "MK" or problem == "NK");
      line_stream >> length;
      // ignore everything after the length. You don't need it
    } else if (head == 'm') {
      // creates a new subfunction
      subfunctions.emplace_back();
      // add the variables associated with this subfunction
      size_t index;
      while (line_stream >> index) {
        subfunctions.back().variables.push_back(index);
      }
      // get the values line
      getline(input, line);
      std::istringstream values(line);
      // Add the values associated with this subfunction
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
      assert(variable_index < length);
    }
  }
}

int MKLandscape::evaluate(size_t subfunction_index,
                          const vector<char>& solution) const {
  const auto& subfunction = subfunctions[subfunction_index];
  // Convert the solution's values to an index into the fitness table
  size_t index = 0;
  for (const auto& neighbor : subfunction.variables) {
    index = (index << 1) | solution[neighbor];
  }
  return subfunction.values[index];
}
