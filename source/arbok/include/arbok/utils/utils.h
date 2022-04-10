#pragma once

#include <vector>
#include <string>

namespace arbok {

struct Graph {
    struct Edge { int from, to, weight; };
    int n = 0;
    std::vector<Edge> edges;
    bool weighted = false;
    Graph() {}
    Graph(int n_, int m=0, bool weighted_=false) : n(n_), edges(m), weighted(weighted_) { }
};

Graph giantCC(const Graph& graph);

Graph fromFile(const std::string& file);

bool isArborescence(const Graph& graph, const std::vector<int>& arbo, long long claimed_weight, int root);

}