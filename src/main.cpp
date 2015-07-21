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
  string filename = argv[1];
  string output = argv[2];
  int radius = atoi(argv[3]);
  MKLandscape problem(filename);

  Enumeration find_local(problem, radius);
  ofstream out(output);
  find_local.enumerate(out);
	return 0;
}
