/*
 * MKLandscape.h
 *
 *  Created on: Jul 20, 2015
 *      Author: goldman
 */

#ifndef MKLANDSCAPE_H_
#define MKLANDSCAPE_H_
#include <iostream>
#include <vector>
using std::vector;

using std::string;

struct Subfunction {
  vector<size_t> variables;
  vector<int> values;
};

class MKLandscape {
 public:
  MKLandscape(string filename);
  ~MKLandscape() = default;
  inline const vector<Subfunction>& get_subfunctions() const {
    return subfunctions;
  }
  inline const size_t get_length() const {
    return length;
  }
  int evaluate(size_t subfunction_index, const vector<bool> & solution) const;
 protected:
  size_t length;
  vector<Subfunction> subfunctions;
};

#endif /* MKLANDSCAPE_H_ */
