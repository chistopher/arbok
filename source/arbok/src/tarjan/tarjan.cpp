
#include <arbok/tarjan/tarjan.h>

#include <iostream>
#include <queue>
#include <numeric>
#include <vector>
#include <cassert>

#include <arbok/tarjan/impl.h>

using namespace std;
using namespace arbok;

Tarjan::~Tarjan() = default;

Tarjan::Tarjan(int n, TarjanVariant variant)
: num_vertices(n)
, cy(n)
, co(n)
, queue_id(n)
{
    inc.reserve(2*n);
    forest.reserve(2*n);
    if(variant == TarjanVariant::SET)
        m_impl = make_unique<SetImpl>(n);
    if(variant == TarjanVariant::MATRIX)
        m_impl = make_unique<MatrixImpl>(n);
    if(variant == TarjanVariant::TREAP)
        m_impl = make_unique<TreapImpl>(n);
    if(variant == TarjanVariant::PQ)
        m_impl = make_unique<PQImpl>(n);
    if (variant == TarjanVariant::HH)
        m_impl = make_unique<HollowHeapImpl>(n);
}

void Tarjan::create_edge(int from, int to, int weight) {
    m_impl->create_edge(from, to, weight);
}

long long Tarjan::run(int root) {

    // put all nodes v != root in queue
    vector<int> q;
    q.reserve(2*num_vertices);
    for (int vertex = 0; vertex < num_vertices; vertex++)
        if (vertex != root)
            q.push_back(vertex);

    // while there is a node v in the queue
    long long answer = 0;
    for(int i=0; i<size(q); ++i) {
        int v = q[i];
        queue_id[v] = i;
        assert(v==co.find(v));

        const auto min_edge = m_impl->get_min_edge(v, co);
        assert(co.find(min_edge.from) != v); // no self-loops allowed here

        inc.push_back(min_edge); // inc edges are stored at queue_id[node] because same node can be rep multiple times
        forest.push_back(i); // at first each edge is a root in forest

        answer += min_edge.weight;
        m_impl->update_incoming_edge_weights(v, min_edge.weight);

        if (cy.join(min_edge.from, min_edge.to))
            continue;

        contractions++;
        // we built a cycle. now merge incoming edges of cycle into one node
        int merged = v;
        auto next_in_cyc = [&](int last) { return co.find(inc[queue_id[last]].from); };
        for (int cur = next_in_cyc(v); cur != merged; cur = next_in_cyc(cur)) {
            int from = cur, to = merged;
            co.join(cur, merged);
            if (co.find(merged)==cur) swap(from,to);
            m_impl->move_edges(from, to);
            merged = to;

            forest[queue_id[cur]] = static_cast<unsigned>(size(q)); // size of queue is where the merged supernode will be in the queue
        }
        forest[i] = static_cast<unsigned>(size(q)); // edge to v is also part of cycle

        // push contracted node
        q.push_back(merged);
    }

    return answer;
}

std::vector<Edge> Tarjan::reconstruct(int arbo_root) {
    assert(size(inc));
    assert(size(inc) == size(forest));
    auto n = static_cast<int>(size(inc));

    // check that the first n-1 picked edges are to nodes [0,n-1] \ {arbo_root}
    for(int i=0; i<num_vertices-1; ++i)
        assert(inc[i].to == i + (arbo_root<=i));

    vector<Edge> res;
    vector del(n,false);
    for(int r=n-1; r>=0; --r) { // we exploit here that parent has always higher index; -> larger index first results in top-to-bottom traversal
        if(del[r]) continue;
        assert(forest[r]==r || del[forest[r]]); // we have a root
        res.push_back(inc[r]);
        int leaf = inc[r].to;
        assert(leaf != arbo_root);
        auto leaf_edge_pos = leaf - (arbo_root < leaf); // if arbo-root<leaf then the first incoming edge of leaf is at leaf-1
        // we take the edge at position r and 'delete' the path from the leaf to the root from the forest
        del[r] = true;
        for(int i=leaf_edge_pos; i!=r; i=forest[i])
            del[i] = true;
    }

    assert(size(res)==num_vertices-1);

    return res;
}
