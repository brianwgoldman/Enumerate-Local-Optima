// Brian Goldman

// An MK Landscape is any problem which can be expressed as
// a sum of k-bounded subfunctions. This can include problems
// such as:
// * MAX-kSAT
// * NKq Landscapes
// * Mesh Ising Spin Glasses
//
// A Landscape is defined in the following file format
// * Any line beginning with a 'c' is a comment
// * Must include 1 line starting with 'p' of the form "p MK N K"
//   where N is the number of variables
// * A series of line pairs describing each subfunction, starting with
//   a line 'm' followed by space-separated, zero indexed, variables.
//   The line immediately following is then the fitness table of the
//   subfunction.

#ifndef MKLANDSCAPE_H_
#define MKLANDSCAPE_H_
#include <iostream>
#include <vector>
using std::vector;
using std::string;

// Internal struct used to represent a subfunction
struct Subfunction {
  vector<size_t> variables;
  vector<int> values;
};

// Defines an MK Landscape
class MKLandscape {
 public:
  // Read the landscape from a file
  MKLandscape(string filename);
  ~MKLandscape() = default;
  inline const vector<Subfunction>& get_subfunctions() const {
    return subfunctions;
  }
  inline const size_t get_length() const {
    return length;
  }
  // Determines the subfunction's value given a binary "solution"
  int evaluate(size_t subfunction_index, const vector<char> & solution) const;
 protected:
  size_t length;
  vector<Subfunction> subfunctions;
};

#endif /* MKLANDSCAPE_H_ */
