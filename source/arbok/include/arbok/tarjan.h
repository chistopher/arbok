#pragma once

#include <vector>
#include <memory>

#include <arbok/dsu.h>

namespace arbok {

// TODO rename this to be more expressive; what this does is manage the edges
class TarjanImpl; // FWD
enum class TarjanVariant { SET, MATRIX, TREAP };

class Tarjan {
public:
    Tarjan(int num_vertices, TarjanVariant variant);
    virtual ~Tarjan() = default;

    void create_edge(int from, int to, int weight);
    int run(int root);
    std::vector<std::pair<int,int>> reconstruct();

protected:
    const int num_vertices;
    DSU cy; // for weakly connected components
    DSU co; // for actual merges

    std::unique_ptr<TarjanImpl> m_impl; // PImpl pattern (https://en.cppreference.com/w/cpp/language/pimpl)
};

}  // end namespace arbok
