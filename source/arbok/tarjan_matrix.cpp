
#include <arbok/tarjan.h>

#include <numeric>
#include <cassert>

#include "tarjan_impl.h"

using namespace std;
using namespace arbok;

const auto NO_EDGE = numeric_limits<int>::max();

MatrixImpl::MatrixImpl(int n) {
    adj.resize(n, vector(n, NO_EDGE));
};

void MatrixImpl::create_edge(int from, int to, int weight) {
    adj[to][from] = min(adj[to][from], weight);  // we save backwards edges in the adjacency matrix
};

TarjanImpl::Edge MatrixImpl::get_min_edge(int v, DSU& dsu) {
        pair<int, int> mw_mo = {NO_EDGE, 0};
        // iterate over the adjacency matrix
        for (int origin = 0; origin < adj.size(); origin++) {
            if (dsu.find(origin) == v)
                continue;  // we don't care about self loops
            mw_mo = min(mw_mo, {adj[v][origin], origin});
        }
        auto& [min_edge_weight, min_edge_origin] = mw_mo;
        assert(min_edge_weight != NO_EDGE);
        return {min_edge_origin, v, min_edge_weight};
}

void MatrixImpl::update_incoming_edge_weights(int v, int w) {
    for (int origin = 0; origin < adj.size(); origin++) {
        if (adj[v][origin] == NO_EDGE)
            continue;
        if (origin == v)
            continue;
        adj[v][origin] -= w;
    }
}

void MatrixImpl::move_edges(int from, int to) {
    assert(from != to);
    for (int origin = 0; origin < adj.size(); origin++) {
        adj[to][origin] = min(adj[to][origin], adj[from][origin]);
    }
    // outgoing edges are not updates because we find them also from non-reps in get-min-edge
}

