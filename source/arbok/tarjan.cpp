
#include <arbok/tarjan.h>

#include <iostream>
#include <queue>
#include <numeric>
#include <vector>
#include <cassert>

using namespace std;
using namespace arbok;

struct Edge {
    int from, to, weight;
    bool operator<(const Edge &b) const {
        if (weight == b.weight) {
            return from < b.from;
        }
        return weight < b.weight;
    }
};

class arbok::TarjanImpl {
public:
    virtual ~TarjanImpl() = default;
    virtual void create_edge(int from, int to, int weight) = 0;
    virtual Edge get_min_edge(int v, DSU& dsu) = 0;
    virtual void update_incoming_edge_weights(int v, int w) = 0;
    virtual void move_edges(int from, int to) = 0;
};


class SetImpl : public TarjanImpl {
public:
    virtual ~SetImpl() = default;
    SetImpl(int _num_vertices) : co(_num_vertices){};
    void create_edge(int from, int to, int weight) override {
        Edge e{from, to, weight};
        co.add_set_element(to, e);
    }
    Edge get_min_edge(int v, DSU& dsu) override {
        return *co.getSetElements(v)->begin(); // TODO: Add Offset here (previously get_edge_weight)
    }
    void update_incoming_edge_weights(int v, int w) override { co.addOffset(v, -w); };
    void move_edges(int from, int to) override { co.merge(from, to); };

private:
    OffsetableSetManagingDSU<Edge> co; // TODO this should not be combined with a DSU
};

class MatrixImpl : public TarjanImpl {
public:
    virtual ~MatrixImpl() = default;
    MatrixImpl(int n) : NO_EDGE(numeric_limits<int>::max()) {
        adj.resize(n, vector(n, NO_EDGE));
    };
    void create_edge(int from, int to, int weight) override {
        adj[to][from] = weight;  // we save backwards edges in the adjacency matrix
    };
    Edge get_min_edge(int v, DSU& dsu) override {
        pair<int, int> mw_mo = {NO_EDGE, 0};
        // iterate over the adjacency matrix
        for (int origin = 0; origin < adj.size(); origin++) {
            if (dsu.find(origin) == v)
                continue;  // we don't care about self loops
            mw_mo = min(mw_mo, {adj[v][origin], origin});
        }
        auto& [min_edge_weight, min_edge_origin] = mw_mo;
        assert(min_edge_weight != NO_EDGE);
        return Edge{dsu.find(min_edge_origin), v, min_edge_weight};
    };
    void update_incoming_edge_weights(int v, int w) {
        for (int origin = 0; origin < adj.size(); origin++) {
            if (adj[v][origin] == NO_EDGE)
                continue;
            if (origin == v)
                continue;
            adj[v][origin] -= w;
        }
    };
    void move_edges(int from, int to) {
        assert(from != to);
        for (int origin = 0; origin < adj.size(); origin++) {
            adj[to][origin] = min(adj[to][origin], adj[from][origin]);
        }
        // outgoing edges are not updates because we find them also from non-reps in get-min-edge
    };

private:
    vector<vector<int>> adj;
    int NO_EDGE;
};

Tarjan::~Tarjan() = default;

Tarjan::Tarjan(int _num_vertices, TarjanVariant variant)
: num_vertices(_num_vertices)
, cy(_num_vertices)
, co(_num_vertices)
{
    if(variant == TarjanVariant::SET)
        m_impl = make_unique<SetImpl>(_num_vertices);
    if(variant == TarjanVariant::MATRIX)
        m_impl = make_unique<MatrixImpl>(_num_vertices);
    if(variant == TarjanVariant::TREAP)
        exit(0); // TODO implement Treap tarjan
}

void Tarjan::create_edge(int from, int to, int weight) {
    m_impl->create_edge(from, to, weight);
}

long long Tarjan::run(int root) {

    long long answer = 0;
    queue<int> q;
    vector<int> pi(num_vertices, -1);
    // put all nodes v != root in queue
    for (int vertex = 0; vertex < num_vertices; vertex++) {
        if (vertex != root)
            q.push(vertex);
    }

    // while there is a node v in the queue
    while (!q.empty()) {
        int v = co.find(q.front());
        q.pop();

        auto min_edge = m_impl->get_min_edge(v, co);
        pi[v] = min_edge.from;

        answer += min_edge.weight;
        m_impl->update_incoming_edge_weights(v, min_edge.weight);

        if (cy.join(min_edge.from, min_edge.to))
            continue;
        // we did build a cycle:

        // merge incoming edges of cycle, contract to one node
        int merged = v;
        for (int cur = co.find(pi[v]); cur != merged; cur = co.find(pi[cur])) {
            int from = cur, to = merged;
            co.join(cur, merged);
            if (co.find(merged)==cur) swap(from,to);
            m_impl->move_edges(from, to);
            merged = co.find(merged); // merged = to;
        }

        // push contracted node
        q.push(merged);
    }

    return answer;
}

std::vector<std::pair<int,int>> Tarjan::reconstruct() {
    // TODO
    return {};
}
