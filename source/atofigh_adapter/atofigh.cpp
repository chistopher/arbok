
#include <arbok/others/atofigh.h>

#include <vector>

using namespace std;
using namespace arbok;

#ifdef ATOFIGH

#include "edmonds_optimum_branching_impl.hpp"

#include <boost/graph/edge_list.hpp>

struct arbok::AtofighImpl {
    AtofighImpl(int n, int m) { }
    void create_edge(int from, int to, int weight) {
        m_edges.emplace_back(from,to);
        m_weights.push_back(weight);
    }
    long long run(int root) {
        array<int,1> roots{root};
        auto boost_edge_list = boost::edge_list(begin(m_edges),end(m_edges));
        edmonds_optimum_branching<false,true,true>(
                boost_edge_list,
                boost::typed_identity_property_map<int>(),
                m_weights.data(),
                begin(roots),
                end(roots),
                std::back_inserter(m_arbo));
        long long ans = 0;
        for(auto e_id : m_arbo)
            ans += m_weights[e_id];
        return ans;
    }
    std::vector<int> reconstruct(int root) {
        return {};
    }

    std::vector<std::pair<int,int>> m_edges;
    std::vector<long long> m_weights;
    std::vector<int> m_arbo;
};

// the adapter just passes through to the impl
Atofigh::~Atofigh() = default;
arbok::Atofigh::Atofigh(int n, int m) : num_vertices(n), m_impl(make_unique<AtofighImpl>(n, m)) { }
void Atofigh::create_edge(int from, int to, int weight) { m_impl->create_edge(from,to,weight); }
long long Atofigh::run(int root) { return m_impl->run(root); }
std::vector<int> Atofigh::reconstruct(int root) { return m_impl->reconstruct(root); }

#else

#include <iostream>
struct arbok::AtofighImpl { };
Atofigh::~Atofigh() = default;
arbok::Atofigh::Atofigh(int n, int m) : num_vertices(n) {
    std::cout << "ERROR: can't use lemon solver without lemon" << endl;
    exit(1);
}
void Atofigh::create_edge(int from, int to, int weight) { }
long long Atofigh::run(int root) { return 0; }
std::vector<int> Atofigh::reconstruct(int root) { return {}; }
#endif
