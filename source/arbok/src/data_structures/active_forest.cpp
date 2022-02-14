
#include <arbok/data_structures/active_forest.h>

using namespace arbok;
using namespace std;

ActiveForest::ActiveForest(CompressedTree<int> &_co)
    : co(_co)
    , active_edge(co.size())
    , active_sets(co.size())
{

}

void ActiveForest::makeActive(EdgeLink link) {
    int from = co.find(link.from);
    int to = co.find(link.to);
    if(!active_edge[from]) { // from has no active edge yet
        active_edge[from] = new FibHeapNode{link};
        active_sets[to].push_back(active_edge[from]);
        return;
    }
    // from has active edge; we need to steal it
    // TODO removal must be done without knowing pset
    // TODO if to == find(active_edge[from].to) don't steal but just decrease key
    //      maybe move also works when moving from the same set into itself and we just don't care?
    auto& pset = active_sets[co.find(active_edge[from]->data.to)];
    pset.erase(find(begin(pset),end(pset), active_edge[from]));
    active_edge[from]->data = link;
    active_sets[to].push_back(active_edge[from]);
}

EdgeLink ActiveForest::deleteActiveEdge(int i) {
    auto v = active_edge[i];

    // delete from previous active set
    auto& pset = active_sets[co.find(v->data.to)];
    pset.erase(find(begin(pset),end(pset), v));

    // delete the edge itself
    auto res = v->data;
    active_edge[i] = nullptr;
    delete v;
    return res;
}

EdgeLink ActiveForest::extractMin(int i) {
    auto it = min_element(begin(active_sets[i]), end(active_sets[i]), [this](FibHeapNode* a, FibHeapNode* b) {
        return value(a) < value(b);
    });
    return deleteActiveEdge(co.find((*it)->data.from));
}

void ActiveForest::mergeHeaps(int i, int j) {
    active_sets[i].splice(end(active_sets[i]), active_sets[j]);
    active_sets[j].clear();
}
