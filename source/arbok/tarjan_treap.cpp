
#include <cassert>

#include <arbok/treap.h>

#include "tarjan_impl.h"

using namespace std;
using namespace arbok;

TreapImpl::TreapImpl(int n) : managedSets(n, nullptr) {
}

TreapImpl::~TreapImpl() {
    // BFS to delete all reachable treap nodes
    vector<Node*> toDelete;
    for(auto node : managedSets)
        if(node) toDelete.push_back(node);
    for(int next=0; next<size(toDelete); ++next) {
        auto cur = toDelete[next];
        if(cur->l) toDelete.push_back(cur->l);
        if(cur->r) toDelete.push_back(cur->r);
        delete cur;
    }
}

void TreapImpl::create_edge(int from, int to, int weight) {
    managedSets[to] = merge(managedSets[to], new Node(weight, from, to));
}

TarjanImpl::Edge TreapImpl::get_min_edge(int v, DSU& dsu) {
    assert(managedSets[v] != nullptr);
    Node* res = nullptr;
    while(res == nullptr) {
        tie(res, managedSets[v]) = split_min(managedSets[v]); // res becomes min and m[v] keeps the rest
        if(dsu.find(res->x.from) == v) {
            delete res;
            res = nullptr;
        }
    }
    auto e = res->x;
    delete res;
    return {e.from, e.to, (int)e.weight};
}

void TreapImpl::update_incoming_edge_weights(int v, int w) {
    apply(managedSets[v], -w);
}

void TreapImpl::move_edges(int from, int to) {
    managedSets[to] = merge(managedSets[from], managedSets[to]);
    managedSets[from] = nullptr;
}
