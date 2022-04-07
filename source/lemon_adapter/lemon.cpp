
#include <arbok/others/lemon.h>

#include <vector>

using namespace std;
using namespace arbok;

#ifdef LEMON

#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/min_cost_arborescence.h>

struct arbok::LemonImpl {
    LemonImpl(int n, int /*m*/) : weights(graph), algo(graph, weights) {
        for(int i=0; i<n; ++i)
            graph.addNode();
    }
    void create_edge(int from, int to, int weight) {
        auto arc = graph.addArc(graph.nodeFromId(from), graph.nodeFromId(to));
        weights[arc] = weight;
    }
    long long run(int root) {
        algo.run(graph.nodeFromId(root));
        return algo.arborescenceCost();
    }
    std::vector<int> reconstruct(int root) {
        return {};
    }

    using LGraph = lemon::SmartDigraph;
    using LMap = LGraph::ArcMap<long long>;
    using LAlg = lemon::MinCostArborescence<LGraph,LMap>;

    LGraph graph;
    LMap weights;
    LAlg algo;
};

// the adapter just passes through to the impl
Lemon::~Lemon() = default;
arbok::Lemon::Lemon(int n, int m) : num_vertices(n), m_impl(make_unique<LemonImpl>(n, m)) { }
void Lemon::create_edge(int from, int to, int weight) { m_impl->create_edge(from,to,weight); }
long long Lemon::run(int root) { return m_impl->run(root); }
std::vector<int> Lemon::reconstruct(int root) { return m_impl->reconstruct(root); }

#else

#include <iostream>
struct arbok::LemonImpl { };
Lemon::~Lemon() = default;
arbok::Lemon::Lemon(int n, int m) : num_vertices(n) {
    std::cout << "ERROR: can't use lemon solver without lemon" << endl;
    exit(1);
}
void Lemon::create_edge(int from, int to, int weight) { }
long long Lemon::run(int root) { return 0; }
std::vector<int> Lemon::reconstruct(int root) { return {}; }
#endif
