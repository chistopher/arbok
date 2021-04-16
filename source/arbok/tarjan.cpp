
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
    virtual void create_edge(int from, int to, int weight) = 0;
    virtual Edge get_min_edge(int v) = 0;
    virtual int get_edge_weight(int v, Edge e) = 0;
    virtual void update_incoming_edge_weights(int v, int w) = 0;
    virtual void merge_vertices(int a, int b, int rep) = 0; // I give this the 3rd rep parameter because this object should not do the DSU stuff
    virtual void delete_self_loops_of_cycle(int v) = 0;
};


class SetImpl : public TarjanImpl {
public:
    SetImpl(int _num_vertices) : co(_num_vertices){};
    void create_edge(int from, int to, int weight) override {
        Edge e{from, to, weight};
        co.add_set_element(to, e);
    }
    Edge get_min_edge(int v) override {
        return *co.getSetElements(v)->begin();
    }
    int get_edge_weight(int v, Edge e) override {
        return co.getOffset(v) + e.weight;
    }
    void update_incoming_edge_weights(int v, int w) override { co.addOffset(v, -w); };
    void merge_vertices(int a, int b, int rep) override { co.merge(a, b); }; // TODO use rep
    void delete_self_loops_of_cycle(int v) override {
        while (co[co.getSetElements(v)->begin()->from] == co[v]) {
            co.getSetElements(v)->erase(co.getSetElements(v)->begin());
        }
    }

private:
    OffsetableSetManagingDSU<Edge> co; // TODO this should not be combined with a DSU
};

class MatrixImpl : public TarjanImpl {
public:
    MatrixImpl(int n) : co(n), NO_EDGE(numeric_limits<int>::max()) {
        adj.resize(n, vector(n, NO_EDGE));
    };
    void create_edge(int from, int to, int weight) override {
        adj[to][from] = weight;  // we save backwards edges in the adjacency matrix
    };
    Edge get_min_edge(int v) {
        pair<int, int> mw_mo = {NO_EDGE, 0};
        // iterate over the adjacency matrix
        for (int origin = 0; origin < adj.size(); origin++) {
            if (co.find(origin) == v)
                continue;  // we don't care about self loops
            mw_mo = min(mw_mo, {adj[v][origin], co.find(origin)});
        }
        auto& [min_edge_weight, min_edge_origin] = mw_mo;
        return Edge{min_edge_origin, v, min_edge_weight};
    };
    int get_edge_weight(int, Edge e) { return e.weight; };
    void update_incoming_edge_weights(int v, int w) {
        for (int origin = 0; origin < adj.size(); origin++) {
            if (adj[v][origin] == NO_EDGE)
                continue;
            if (origin == v)
                continue;
            adj[v][origin] -= w;
        }
    };
    void merge_vertices(int cur, int v, int rep) {
        assert(cur != v);
        assert(rep==cur || rep==v);
        co.join(cur, v);  // merge cur into v
        for (int origin = 0; origin < adj.size(); origin++) {
            adj[v][co.find(origin)] = min(adj[v][co.find(origin)], adj[cur][co.find(origin)]);
        }
    };
    void delete_self_loops_of_cycle(int){/* NOP for MatrixTarjan */};

private:
    vector<vector<int>> adj;
    DSU co; // TODO remove. this should be done outside of this class
    int NO_EDGE;
};

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

int Tarjan::run(int root) {

    int answer = 0;
    queue<int> q;
    vector<int> pi(num_vertices, -1);
    answer = 0;
    // put all nodes v != root in queue
    for (int vertex = 0; vertex < num_vertices; vertex++) {
        if (vertex != root)
            q.push(vertex);
    }

    // while there is a node v in the queue
    while (!q.empty()) {
        int v = (q.front());
        q.pop();

        auto min_edge = m_impl->get_min_edge(v);
        pi[v] = min_edge.from;

        int edge_weight = m_impl->get_edge_weight(v, min_edge);
        answer += edge_weight;

        m_impl->update_incoming_edge_weights(v, edge_weight);

        if (!cy.join(min_edge.from, min_edge.to))
            continue;
        // we did build a cycle:

        // merge incoming edges of cycle, contract to one node
        int merged = v;
        for (int cur = co.find(pi[v]); cur != merged; cur = co.find(pi[cur])) {
            co.join(cur, merged);
            m_impl->merge_vertices(cur, merged, co.find(merged));
            merged = co.find(merged);
        }

        m_impl->delete_self_loops_of_cycle(merged);

        // push contracted node
        q.push(merged);
    }

    return answer;
}

std::vector<std::pair<int,int>> Tarjan::reconstruct() {
    // TODO
    return {};
}
