
#include <arbok/tarjan/tarjan.h>
#include <arbok/tarjan/impl.h>

#include <cassert>


using namespace std;
using namespace arbok;

MatrixImpl::MatrixImpl(int n, int /* m */) {
    adj.resize(n, vector(n, NO_EDGE));
};

void MatrixImpl::create_edge(int from, int to, int weight) {
    adj[to][from] = min(adj[to][from], {from,to,weight,weight});  // we save backwards edges in the adjacency matrix
};

Edge MatrixImpl::get_min_edge(int v, DSU& dsu) {
        auto mn = NO_EDGE;
        // iterate over the adjacency matrix
        for (int origin = 0; origin < adj.size(); origin++)
            if (dsu.find(origin) != v) // we don't care about self loops
                mn = min(mn, adj[v][origin]);
        assert(mn < NO_EDGE || NO_EDGE < mn);
        return mn;
}

void MatrixImpl::update_incoming_edge_weights(int v, int w) {
    for (int origin = 0; origin < adj.size(); origin++) {
        if (origin==v || adj[v][origin] == NO_EDGE)
            continue;
        adj[v][origin].weight -= w;
    }
}

void MatrixImpl::move_edges(int from, int to) {
    assert(from != to);
    for (int origin = 0; origin < adj.size(); origin++) {
        adj[to][origin] = min(adj[to][origin], adj[from][origin]);
    }
    // outgoing edges are not updates because we find them also from non-reps in get-min-edge
}

