
#include <cassert>
#include <vector>

#include <arbok/data_structures/treap.h>
#include <arbok/tarjan/impl.h>

using namespace std;
using namespace arbok;

TreapImpl::TreapImpl(int n) : managedSets(n, nullptr) {
}

TreapImpl::~TreapImpl() {
    // BFS to delete all reachable treap nodes
    vector<treap::Node*> toDelete;
    for(auto node : managedSets)
        if(node) toDelete.push_back(node);
    for(int next=0; next<toDelete.size(); ++next) {
        auto cur = toDelete[next];
        if(cur->l) toDelete.push_back(cur->l);
        if(cur->r) toDelete.push_back(cur->r);
        delete cur;
    }
}

void TreapImpl::create_edge(int from, int to, int weight) {
    managedSets[to] = treap::merge(managedSets[to], new treap::Node(from, to, weight));
}

Edge TreapImpl::get_min_edge(int v, DSU& dsu) {
    assert(managedSets[v] != nullptr);
    treap::Node* res = nullptr;
    while(res == nullptr) {
        tie(res, managedSets[v]) = treap::split_min(managedSets[v]); // res becomes min and m[v] keeps the rest
        if(dsu.find(res->x.from) == v) {
            delete res;
            res = nullptr;
        }
    }
    auto e = res->x;
    delete res;
    return e;
}

void TreapImpl::update_incoming_edge_weights(int v, int w) {
    treap::apply(managedSets[v], -w);
}

void TreapImpl::move_edges(int from, int to) {
    managedSets[to] = treap::merge(managedSets[from], managedSets[to]);
    managedSets[from] = nullptr;
}
