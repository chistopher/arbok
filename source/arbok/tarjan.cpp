
#include <arbok/tarjan.h>

#include <iostream>
#include <queue>
#include <numeric>
#include <vector>
#include <cassert>

#include "tarjan_impl.h"

using namespace std;
using namespace arbok;

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
        int v = q.front(); q.pop();
        assert(v==co.find(v));

        auto min_edge = m_impl->get_min_edge(v, co);
        min_edge.from = co.find(min_edge.from);
        assert(min_edge.from != v); // no self-loops allowed here

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
