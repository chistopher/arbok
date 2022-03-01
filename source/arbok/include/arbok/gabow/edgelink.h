#pragma once

#include <list>
#include <optional>

#include <arbok/data_structures/compressed_tree.h>

namespace arbok {

struct EdgeLink {
    EdgeLink(int _from, int _to, int _weight, int _id) : from(_from), to(_to), weight(_weight), id(_id) {};
    int from, to, weight; // the actual edge
    int id; // pos in edges
    std::optional<std::list<int>::iterator> exit_list_iter;
    std::optional<std::list<int>::iterator> passive_set_iter;
};

inline auto currentWeight(const EdgeLink& e, CompressedTree<int>& dsu) { return e.weight + dsu.find_value(e.to); }

}
