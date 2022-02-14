
#include "compressed_tree.h"

namespace arbok {

template<typename T>
CompressedTree<T>::CompressedTree(int n) : parent(n,-1), value(n) {
}

template<typename T>
int CompressedTree<T>::find(int i) {
    assert(i < parent.size());
    if(parent[i]<0) return i;
    auto root = find(parent[i]);
    if(parent[i]!=root)
        value[i] += value[parent[i]];
    parent[i] = root;
    return root;
}

template<typename T>
T CompressedTree<T>::find_value(int i) {
    auto root = find(i); // after this we are root or are directly below root
    return value[i] + (i!=root ? value[root] : 0);
}

template<typename T>
void CompressedTree<T>::add_value(int i, T increment) {
    value[find(i)] += increment;
}

template<typename T>
bool CompressedTree<T>::same(int i, int j) {
    return find(i) == find(j);
}

template<typename T>
int CompressedTree<T>::size(int i) {
    return -parent[find(i)];
}

template<typename T>
bool CompressedTree<T>::join(int i, int j) {
    i = find(i), j = find(j);
    if (i == j) return false;
    if (parent[i] > parent[j])
        std::swap(i, j);
    parent[i] += parent[j];
    parent[j] = i;
    value[j] -= value[i];
    return true;
}

template<typename T>
size_t CompressedTree<T>::size() {
    return std::size(parent);
}

}  // namespace arbok
