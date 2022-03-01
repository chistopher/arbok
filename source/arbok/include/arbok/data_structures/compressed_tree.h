#pragma once

#include <vector>
#include <cassert>

namespace arbok {

template<typename T>
struct CompressedTree {
    explicit CompressedTree(int n);
    int find(int i);
    T find_value(int i);
    void add_value(int i, T increment);

    bool same(int i, int j);
    int size(int i);
    bool join(int i, int j);

    size_t size();

protected:
    std::vector<int> parent;
    std::vector<T> value;
};


}  // namespace arbok


#include <arbok/data_structures/compressed_tree.inl>