
#include <numeric>
#include <vector>
#include <cassert>

#include "tarjan_impl.h"

using namespace std;
using namespace arbok;

PQImpl::PQImpl(int n) : managedSets(n), offsets(n,0) {
}

void PQImpl::create_edge(int from, int to, int weight) {
    managedSets[to].push({from, to, weight, weight});
}

Edge PQImpl::get_min_edge(int v, DSU& dsu) {
    assert(size(managedSets[v]));
    while(dsu.find(managedSets[v].top().from) == v)
        managedSets[v].pop(); // delete selfloops
    auto res = managedSets[v].top();
    managedSets[v].pop(); // extract the edges that is returned

    res.weight -= offsets[v];
    return res;
}

void PQImpl::update_incoming_edge_weights(int v, int w) {
    offsets[v] += w;
}

void PQImpl::move_edges(int from, int to) {
    // make sure set of from is smaller than to
    auto& small = managedSets[from];
    auto& large = managedSets[to];
    if(size(small)>size(large)) {
        swap(small, large);
        swap(offsets[from], offsets[to]);
    }

    // smaller into larger while applying offset
    while(size(small)) {
        auto e = small.top();
        small.pop();
        e.weight -= offsets[from] - offsets[to];
        large.push(e);
    }
}
