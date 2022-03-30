
#include <arbok/lemon/lemon.h>

#include <vector>

#include <lemon/list_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/min_cost_arborescence.h>

using namespace std;
using namespace arbok;

struct arbok::LemonImpl {
    LemonImpl(int n) : weights(graph), algo(graph, weights) {
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
    std::vector<Edge> reconstruct(int root) {
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
arbok::Lemon::Lemon(int n) : num_vertices(n), m_impl(make_unique<LemonImpl>(n)) { }
void Lemon::create_edge(int from, int to, int weight) { m_impl->create_edge(from,to,weight); }
long long Lemon::run(int root) { return m_impl->run(root); }
std::vector<Edge> Lemon::reconstruct(int root) { return m_impl->reconstruct(root); }
