/*
 * GraphUtilities.cpp
 *
 *  Created on: Jul 21, 2015
 *      Author: goldman
 */

#include "GraphUtilities.h"

vector<unordered_set<size_t>> build_graph(const MKLandscape& landscape) {
  vector<unordered_set<size_t>> graph(landscape.get_length(), unordered_set<size_t>());
  const auto& subfunctions = landscape.get_subfunctions();
  for (const auto & subfunction : subfunctions) {
    for (const auto & x : subfunction.variables) {
      for (const auto & y : subfunction.variables) {
        if (x != y) {
          graph[x].insert(y);
        }
      }
    }
  }
  return graph;
}

// Finds all possible connected induced subgraphs with k or less vertices
vector<vector<size_t>> k_order_subgraphs(
    const vector<unordered_set<size_t>>& graph, size_t radius) {
  // The list of all subgraphs found so far.
  vector<vector<size_t>> found;
  unordered_set<size_t> closed, prevopen;
  // Vertices already in the set, initially empty
  vector<size_t> prev;
  // Call the recursive function starting at each vertex
  for (size_t v=0; v < graph.size(); v++) {
    closed.insert(v);
    recurse(graph, v, closed, prev, prevopen, radius, found);
  }
  return found;
}

// Recursive function used by k_order_subgraphs to expand each subset by "v"
// and then find all possible subgraphs which contain "prev" and "v"
void recurse(const vector<unordered_set<size_t>>& graph,
             size_t v, unordered_set<size_t> & closed, vector<size_t> & prev,
             unordered_set<size_t> & prevopen, size_t radius,
             vector<vector<size_t>> & found) {
  // Create a new subset containing "prev" and "v"
  vector<size_t> inset(prev);
  inset.push_back(v);
  found.push_back(inset);
  // Add that subset to "found"
  if (inset.size() >= radius) {
    return;
  }
  vector<size_t> closed_here;
  // open everything adjacent to v
  unordered_set<size_t> openset(prevopen);
  const auto & adjacent = graph.at(v);
  openset.insert(adjacent.begin(), adjacent.end());
  // recurse using each not-closed option
  for (const auto& working : openset) {
    if (closed.count(working) == 0) {
      closed_here.push_back(working);
      closed.insert(working);
      recurse(graph, working, closed, inset, openset, radius, found);
    }
  }
  // reopen anything closed at this level
  for (const auto& working : closed_here) {
    closed.erase(working);
  }
}
