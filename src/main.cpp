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
  assert(argc > 3);
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
