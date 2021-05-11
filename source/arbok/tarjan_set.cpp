
#include <numeric>
#include <set>
#include <vector>
#include <cassert>

#include "tarjan_impl.h"

using namespace std;
using namespace arbok;

SetImpl::SetImpl(int n) : managedSets(n), offsets(n,0) {
}

void SetImpl::create_edge(int from, int to, int weight) {
    managedSets[to].insert({from, to, weight, weight});
}

Edge SetImpl::get_min_edge(int v, DSU& dsu) {
    assert(size(managedSets[v]));
    while(dsu.find(managedSets[v].begin()->from) == v)
        managedSets[v].erase(managedSets[v].begin()); // delete selfloops
    auto res = *managedSets[v].begin();
    managedSets[v].erase(managedSets[v].begin()); // extract the edge that is returned
    res.weight -= offsets[v];
    return res;
}

void SetImpl::update_incoming_edge_weights(int v, int w) {
    offsets[v] += w;
}

void SetImpl::move_edges(int from, int to) {
    // make sure set of from is smaller than to
    auto& small = managedSets[from];
    auto& large = managedSets[to];
    if(size(small)>size(large)) {
        swap(small, large);
        swap(offsets[from], offsets[to]);
    }

    // smaller into larger while applying offset
    while(size(small)) {
        auto e = small.extract(begin(small));
        e.value().weight -= offsets[from] - offsets[to];
        large.insert(move(e));
    }
}
