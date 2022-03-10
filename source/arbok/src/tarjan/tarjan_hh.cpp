
#include <arbok/data_structures/hollow_heap.h>
#include <arbok/tarjan/impl.h>
#include <cassert>

using namespace std;
using namespace arbok;

HollowHeapImpl::HollowHeapImpl(int n) : managedSets(n) {}

void HollowHeapImpl::create_edge(int from, int to, int weight) {
    managedSets[to].push({from, to, weight, weight});
}

Edge HollowHeapImpl::get_min_edge(int v, DSU &dsu) {
    assert(size(managedSets[v]));
    while (dsu.find(managedSets[v].top().from) == v)
        managedSets[v].pop(); // delete selfloops
    auto res = managedSets[v].top();
    managedSets[v].pop(); // extract the edges that is returned
    return res;
}

void HollowHeapImpl::update_incoming_edge_weights(int v, int w) {
    managedSets[v].apply_update(-w);
}

void HollowHeapImpl::move_edges(int from, int to) {
    managedSets[to].merge(managedSets[from]);
}
