// Brian Goldman

// A collection of tools for creating variable interaction
// graphs and finding k-oder subgraphs. These are used
// when finding all useful moves in an r-bit hamming ball.

#ifndef GRAPHUTILITIES_H_
#define GRAPHUTILITIES_H_

#include "MKLandscape.h"
#include <vector>
using std::vector;
#include <unordered_set>
using std::unordered_set;

// Constructs a sparse graph from the variable interaction tables of the evaluator
vector<unordered_set<size_t>> build_graph(const MKLandscape& evaluator);

// Finds all connected induced subgraphs with "radius" or less vertices.
vector<vector<size_t>> k_order_subgraphs(
    const vector<unordered_set<size_t>>& graph, size_t radius);

// Recursive function used internally by k_order_subgraphs.
void recurse(const vector<unordered_set<size_t>>& graph, size_t v,
             unordered_set<size_t> & closed, vector<size_t> & prev,
             unordered_set<size_t> & prevopen, size_t radius,
             vector<vector<size_t>> & found);

#endif /* GRAPHUTILITIES_H_ */
