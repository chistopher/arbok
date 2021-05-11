#pragma once

#include <vector>
#include <memory>

#include <arbok/dsu.h>

namespace arbok {

// TODO rename this to be more expressive; what this does is manage the edges
class TarjanImpl; // FWD
enum class TarjanVariant { SET, MATRIX, TREAP };
struct Edge {
    int from, to, weight, orig_weight;
    inline bool operator<(const Edge& rhs) const { return tie(weight,from) < tie(rhs.weight, rhs.from); }
    inline bool operator==(const Edge& rhs) const { return tie(weight,from) == tie(rhs.weight,rhs.from); }
};
static const Edge NO_EDGE = {(int)1e9, (int)1e9, (int)1e9, (int)1e9};

class Tarjan {
public:
    Tarjan(int n, TarjanVariant variant);
    ~Tarjan();

    void create_edge(int from, int to, int weight);
    long long run(int root);
    std::vector<Edge> reconstruct();

protected:
    const int num_vertices;
    DSU cy; // for weakly connected components
    DSU co; // for actual merges

    // last time a representative appeared in the queue
    // this is the pos in inc where the most recent incoming edge is stored for this node
    vector<unsigned> queue_id;
    // edges that the algorithm picked in each iteration of the queue
    vector<Edge> inc;
    // forest over the edges in inc that stores parent indices
    std::vector<unsigned> forest;

    std::unique_ptr<TarjanImpl> m_impl; // PImpl pattern (https://en.cppreference.com/w/cpp/language/pimpl)
};

}  // end namespace arbok
