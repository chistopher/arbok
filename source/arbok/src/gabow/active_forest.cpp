
#include <arbok/gabow/active_forest.h>

#include <cassert>
#include <tuple>

using namespace arbok;
using namespace std;

struct FibHeapNode {
    FibHeapNode(int _from, int _to, int _weight, int _id) : from(_from), to(_to), weight(_weight), id(_id) {};

    int from,to,weight,id;
    FibHeapNode *parent = nullptr; // The root of any tree in an F-heap is always in its home heap
    list<FibHeapNode*> children;
    list<FibHeapNode*>::iterator list_it; // iterator to myself in either children list of parent or root list of home_heap
    bool is_loser = false;
};

ActiveForest::ActiveForest(CompressedTree<int> &_co)
    : co(_co)
    , active_edge(co.size())
    , active_sets(co.size())
{
}

ActiveForest::~ActiveForest() {
    for(auto* v : active_edge)
        delete v; // clang told me deleting nullptr is ok :)
}

void ActiveForest::makeActive(int from, int to, int weight, int id) {
    int from_rep = co.find(from);
    if(!active_edge[from_rep]) // from has no active edge yet
        return moveHome(active_edge[from_rep] = new FibHeapNode(from,to,weight,id));

    auto v = active_edge[from_rep];
    assert(pair(weight + co.find_value(to), from) < pair(curWeight(v),v->from) || co.find(to) != co.find(v->to));
    removeFromCurrentList(v);
    v->to = to; v->weight = weight; v->id = id, v->from = from;
    moveHome(v); // heap property is only violated when v's children get displaced
}

void ActiveForest::deleteActiveEdge(int i) {
    auto v = active_edge[i];

    for(auto* child : v->children)
        moveHome(child);
    v->children.clear(); // TODO actually we do not need this cleanup

    removeFromCurrentList(v);
    active_edge[i] = nullptr;
    delete v;
}

int ActiveForest::getMin(int i) {
    // merge rem nodes by rank and create new root list
    vector<FibHeapNode*> order_rep;
    for(auto v : active_sets[i]) {
        v->is_loser = false;
        while(size(order_rep)>size(v->children) && order_rep[size(v->children)]) {
            auto other = order_rep[size(v->children)];
            order_rep[size(v->children)] = nullptr;
            if(pair(curWeight(other),other->from) < pair(curWeight(v),v->from))
                swap(v,other);
            assert(!other->parent);
            other->list_it = v->children.insert(end(v->children), other);
            other->parent = v;
            assert(v!=other);
            assert(find(begin(v->children), end(v->children), other) == other->list_it);
        }
        order_rep.resize(max(size(order_rep),size(v->children)+1),nullptr);
        order_rep[size(v->children)] = v;
    }
    active_sets[i].clear();
    for(auto v : order_rep)
        if(v) v->list_it = active_sets[i].insert(end(active_sets[i]),v);

    // find top element in rootlist
    assert(!empty(active_sets[i])); // list not empty
    FibHeapNode* v_min = *min_element(begin(active_sets[i]), end(active_sets[i]), [this](auto a, auto b){
        return pair(curWeight(a),a->from) < pair(curWeight(b),b->from);
    });
    assert(v_min == active_edge[co.find(v_min->from)]); // edge is active edge
    assert(co.find(v_min->to) == i); // edge is in home heap

    return v_min->id;
}

void ActiveForest::mergeHeaps(int i, int j) {
    active_sets[i].splice(end(active_sets[i]), active_sets[j]);
    assert(active_sets[j].empty());
}

list<FibHeapNode*>& ActiveForest::home_heap(FibHeapNode* v) {
    return active_sets[co.find(v->to)];
}

void ActiveForest::removeFromCurrentList(FibHeapNode* v) {
    auto& lst = v->parent ? v->parent->children : home_heap(v);
    assert(count(begin(lst),end(lst),v));
    assert(find(begin(lst),end(lst),v)==v->list_it);
    lst.erase(v->list_it);
    if(v->parent) {
        loseChild(v->parent);
        v->parent = nullptr;
    }
}

void ActiveForest::moveHome(FibHeapNode* v) {
    auto& home = home_heap(v);
    assert(find(begin(home),end(home),v)==end(home));
    v->list_it = home.insert(end(home), v);
    v->parent = nullptr;
}

void ActiveForest::loseChild(FibHeapNode* v) {
    if(!v->parent) return;
    if(v->is_loser) {
        loseChild(v->parent); // cascade
        v->parent->children.erase(v->list_it);
        moveHome(v);
    }
    v->is_loser ^= 1;
}

int ActiveForest::curWeight(FibHeapNode *v) {
    return v->weight + co.find_value(v->to);
}
