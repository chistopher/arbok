#pragma once

#include <tuple>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <memory>
#include <optional>
#include <cassert>

#include <arbok/data_structures/dsu.h>
#include <arbok/data_structures/compressed_tree.h>
// prevents cyclic dependency in gabow and active set

namespace arbok {

struct EdgeLink {
    EdgeLink(int _from, int _to, int _weight, int _id, CompressedTree<int>& _dsu)
    : from(_from), to(_to), weight(_weight), id(_id), dsu(_dsu) {};
    EdgeLink(const EdgeLink& rhs) = default; // default copy constructor
    EdgeLink& operator=(const EdgeLink& rhs) {
        from = rhs.from, to = rhs.to, weight = rhs.weight, id = rhs.id;
        exit_list_iter = rhs.exit_list_iter; passive_set_iter = rhs.passive_set_iter;
        return *this;
    } // cannot default this because of dsu reference
    int from, to, weight; // the actual edge
    int id; // pos in edges
    std::optional<std::list<int>::iterator> exit_list_iter;
    std::optional<std::list<int>::iterator> passive_set_iter;
    CompressedTree<int>& dsu;
    auto currentWeight() const { return weight + dsu.find_value(to); }
    bool operator<(const EdgeLink& other) const { return currentWeight() < other.currentWeight(); }
};

}
