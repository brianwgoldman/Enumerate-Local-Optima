// Brian Goldman

// Parse arguments and drive the enumeration tool.
// Will read in an MK Landscape from a specified file and then
// write all of its r-bit local optima to another specified file.
// An r-bit local optima is a solution which cannot be improved
// by flipping r or less bits.

// ---------------- Compilation ----------------------------
// In Release/ and Debug/ there are makefiles to compile the source
// To perform experiments, call "make MKL" in the Release directory.
// For our experiments we used G++ 4.8.2.

// ---------------- Example Usage ----------------------------
// From the top level directory call:
// Release/MKL problem_folder/DeceptiveTrap_20_5_0.txt output.txt 1
// This will find all local optima of a Deceptive Trap problem using
// N=20 and k=5 and random seed 0 (shuffled varible ordering) as the landscape
// and will output all 1 bit local optima to "output.txt"
// There are also optional switches which can disable hyperplane elimination and
// disable reordering, both of which are enabled by default.

#include "MKLandscape.h"
#include "GraphUtilities.h"
#include "Enumeration.h"

#include <iostream>
using namespace std;
#include <cassert>
#include <fstream>

int main(int argc, char * argv[]) {
  if (argc < 4) {
    // Help message
    cout << "Usage: input_filename output_filename radius [use_hyperplanes] [use_reordering]" << endl;
    cout << endl;
    cout << "By default hyperplanes and reordering are used, but can be set to 0 to turn off" << endl;
    cout << "Example: ./MKL input.txt output.txt 2 1 0" << endl;
    cout << "         This will read a problem from input.txt, write local optima to output.txt," << endl;
    cout << "         only find 2-bit local optima, use hyperplanes but turn off reordering." << endl;
    return 0;
  }
  string problem_file = argv[1];
  string output_file = argv[2];
  int radius = atoi(argv[3]);

  bool hyper = true;
  if (argc > 4) {
    // Turn off hyperplane elimination if 3rd argument is 0
    hyper = atoi(argv[4]) != 0;
  }
  bool reorder = true;
  if (argc > 5) {
    // Turn off reordering if 4th argument is 0
    reorder = atoi(argv[5]) != 0;
  }
  // Construct the landscape
  MKLandscape problem(problem_file);
  // Construct the enumeration tool
  Enumeration find_local(problem, radius);
  ofstream out(output_file);
  // Find all local optima
  find_local.enumerate(out, hyper, reorder);
  return 0;
}
