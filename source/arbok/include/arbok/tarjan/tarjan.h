#pragma once

#include <vector>
#include <memory>

#include <arbok/data_structures/dsu.h>

namespace arbok {

// TODO rename this to be more expressive; what this does is manage the edges
class TarjanImpl; // FWD
enum class TarjanVariant { SET, MATRIX, TREAP, PQ, HH };
struct Edge {
    Edge() = default;
    Edge(int f, int t, int w) : from(f), to(t), weight(w), orig_weight(w) {}
    Edge(int f, int t, int w, int ow) : from(f), to(t), weight(w), orig_weight(ow) {}

    int from, to, weight, orig_weight;
    inline bool operator<(const Edge& rhs) const { return std::tie(weight,from) < std::tie(rhs.weight, rhs.from); }
    inline bool operator==(const Edge& rhs) const { return std::tie(weight,from) == std::tie(rhs.weight,rhs.from); }
    inline bool operator>(const Edge& rhs) const { return rhs < *this; }
};
static const Edge NO_EDGE{(int)1e9, (int)1e9, (int)1e9};

class Tarjan {
public:
    Tarjan(int n, int m, TarjanVariant variant);
    ~Tarjan();

    void create_edge(int from, int to, int weight);
    long long run(int root);
    std::vector<Edge> reconstruct(int root);

    int contractions = 0;
protected:
    const int num_vertices;
    DSU cy; // for weakly connected components
    DSU co; // for actual merges

    // last time a representative appeared in the queue
    // this is the pos in inc where the most recent incoming edge is stored for this node
    std::vector<unsigned> queue_id;
    // edges that the algorithm picked in each iteration of the queue
    std::vector<Edge> inc;
    // forest over the edges in inc that stores parent indices
    std::vector<unsigned> forest;

    std::unique_ptr<TarjanImpl> m_impl; // PImpl pattern (https://en.cppreference.com/w/cpp/language/pimpl)
};

template<TarjanVariant var>
struct SpecificTarjan : Tarjan { SpecificTarjan(int n, int m) : Tarjan(n, m, var) {} };
using SetTarjan = SpecificTarjan<TarjanVariant::SET>;
using MatrixTarjan = SpecificTarjan<TarjanVariant::MATRIX>;
using TreapTarjan = SpecificTarjan<TarjanVariant::TREAP>;
using PQTarjan = SpecificTarjan<TarjanVariant::PQ>;
using HHTarjan = SpecificTarjan<TarjanVariant::HH>;

}  // end namespace arbok
