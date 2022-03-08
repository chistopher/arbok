#pragma once

#include <arbok/data_structures/compressed_tree.h>

namespace arbok {

struct EdgeLink {
    EdgeLink(int _from, int _to, int _weight, int _id) : from(_from), to(_to), weight(_weight), id(_id) {};
    int from, to, weight; // the actual edge
    int id; // pos in edges; // TODO I think we can get rid of this :) we'll need it only in active_forest
    // when extending the path to vertex u, all its outgoing passive edges will never ever be relevant to the algo
    // they are still in some passive sets on the growth path, but we'll ignore them later
    bool ignore = false;
};

inline auto currentWeight(const EdgeLink& e, CompressedTree<int>& dsu) { return e.weight + dsu.find_value(e.to); }

}
