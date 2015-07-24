//============================================================================
// Name        : main.cpp
// Author      : Brian W. Goldman
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "MKLandscape.h"
#include "GraphUtilities.h"
#include "Enumeration.h"

#include <iostream>
using namespace std;
#include <cassert>
#include <fstream>

int main(int argc, char * argv[]) {
  if (argc < 4) {
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
    hyper = atoi(argv[4]) != 0;
  }
  bool reorder = true;
  if (argc > 5) {
    reorder = atoi(argv[5]) != 0;
  }
  MKLandscape problem(problem_file);

  Enumeration find_local(problem, radius);
  ofstream out(output_file);
  find_local.enumerate(out, hyper, reorder);
  return 0;
}
