
#include <arbok/gabow/gabow.h>

#include <iostream>
#include <cassert>

using namespace arbok;

Gabow::Gabow(int n, int /* m */)
    : num_vertices(n)
    , co(n)
    , incoming_edges(n)
    , exit_list(n)
    , passive_set(n)
    , active_forest(co)
{
    growth_path.reserve(n); // cannot become larger
    path_edges.reserve(n);
}

void Gabow::create_edge(int from, int to, int weight) {
    assert(0<=from && from < num_vertices);
    assert(0<=to && to < num_vertices);
    if (from != to)
        incoming_edges[to].push_back(int(size(edges)));
    edges.push_back({from, to, weight}); // we save even self loops to keep edge ids consistent with the outside
}

long long Gabow::run(int root) {
    long long answer = 0;
    std::vector<int> seen(num_vertices,-1); // for each vertex, the start of the path that discovered this vertex
    for(int v=0; v<num_vertices; ++v) {
        if(co.same(root,v)) continue;
        // we start a new path and u is the next vertex that should go on path
        for(int u = v; !co.same(root,u); ) {
            if(seen[u] == -1)
                extendPath(u), seen[u] = v;
            else
                u = contractPathPrefix(u);

            // choose next edge
            int edge_id = active_forest.getMin(u);
            auto& edge = edges[edge_id];
            answer += currentWeight(edge);
            int forest_id = static_cast<int>(std::size(chosen));
            chosen.push_back(edge_id);
            forest.push_back(forest_id); // new edge initially has no parent
            path_edges.push_back(forest_id);
            u = co.find(edge.from);
        }
        contractCompletePath(root);
    }

    return answer;
}

std::vector<int> Gabow::reconstruct(int root) {

    auto n = static_cast<int>(std::size(chosen));

    // build leafs arary (first chosen edge for each node)
    std::vector<int> leaf(num_vertices, -1);
    for(int i=0; i<n; ++i) {
        auto& edge = edges[chosen[i]];
        if(leaf[edge.to]==-1) leaf[edge.to] = i;
    }
    leaf[root] = -2;
    assert(none_of(begin(leaf), end(leaf), [](int l) { return l==-1; })); // assert each node has an incoming edge

    std::vector<int> res;
    std::vector del(n,false);
    for(int r=n-1; r>=0; --r) { // we exploit here that parent has always higher index; -> larger index first results in top-to-bottom traversal
        if(del[r]) continue;
        //assert(forest[r]==n || del[forest[r]]); // we have a root (first check is due to last contraction having no incoming edge)
        assert(forest[r]==r || del[forest[r]]); // we are a root or our parent was already deleted
        auto& edge = edges[chosen[r]];
        if(edge.to != root) res.push_back(chosen[r]);
        auto leaf_edge_pos = leaf[edge.to];
        // we take the edge at position r and 'delete' the path from the leaf to the root from the forest
        del[r] = true;
        for(int i=leaf_edge_pos; i!=r; i=forest[i])
            del[i] = true;
    }

    assert(size(res)==num_vertices-1);
    return res;
}

void Gabow::extendPath(int u) {
    assert(co.find(u) == u);
    growth_path.push_back(u);

    for (int edge_id : incoming_edges[u]) {
        auto& edge = edges[edge_id];
        int rep_x = co.find(edge.from);
        if(!exit_list[rep_x].empty()) {
            int front_edge_id = exit_list[rep_x].back();
            auto& front_edge = edges[front_edge_id];
            if(u!=front_edge.to)
                passive_set[co.find(front_edge.to)].push_back(front_edge_id); // keep both frontedge and edge
            else if(edge.weight < front_edge.weight) // we can use weight here since u was never contracted
                exit_list[rep_x].pop_back(); // discard previous front edge
            else
                continue; // discard current edge coz it's dominated
        }
        exit_list[rep_x].push_back(edge_id);
        active_forest.makeActive(edge.from,edge.to,edge.weight,edge_id);
    }
}

int Gabow::contractPathPrefix(int u) {
    contractions++;
    assert(co.find(u) == u);
    assert(size(growth_path) == size(path_edges));

    std::vector<int> prefix;
    do {
        auto vi = growth_path.back();
        assert(co.find(vi) == vi); // all nodes on growth path are representatives
        assert(co.find(edges[chosen[path_edges.back()]].to) == vi); // synced with gp edges
        prefix.push_back(vi);
        co.add_value(vi, -currentWeight(edges[chosen[path_edges.back()]]));
        forest[path_edges.back()] = static_cast<int>(size(chosen)); // next iteration in run will find an incoming edge into the here contracted prefix; this edge will be the next in chosen
        growth_path.pop_back();
        path_edges.pop_back();
    } while(prefix.back() != u);

    // delete outgoing edges
    for(auto vi : prefix) {
        if(empty(exit_list[vi])) continue;
        exit_list[vi].clear();
        active_forest.deleteActiveEdge(vi);
    }

    // merge dsu
    for(int vi : prefix) co.join(vi, u);
    int new_rep = co.find(u);
    growth_path.push_back(new_rep);

    // merge in active forest
    for(int vi : prefix)
        if (vi != new_rep)
            active_forest.mergeHeaps(new_rep, vi);

    // condense all edges into the prefix to at most 1 per origin
    for(int vi : prefix) {
        // if we are here then there are no passive edges to any node earlier in prefix than vi
        reverse(begin(passive_set[vi]),end(passive_set[vi])); // we want to iterate passive set in reverse order of insertion
        for(auto edge_id : passive_set[vi]) {
            auto& edge = edges[edge_id];
            int from = co.find(edge.from);
            if(from==new_rep) continue; // this was already deleted coz it was a self loop
            int first_edge_id = exit_list[from].back();
            auto& first_edge = edges[first_edge_id];
            // the exit list of 'from' should have 'edge' as the first passive edge (just after the active 'first_edge')
            // since all passive edges from nodes v0...v_{i-1} were deleted
            // with multi-edges this depends on the fact that edge order in exit_list[from] has edges to vi in same order as they are in passive_set[vi]
            assert(size(exit_list[from])>=2); // namely first_edge and edge
            assert(std::find(rbegin(exit_list[from]), rend(exit_list[from]), edge_id) == ++rbegin(exit_list[from]));
            exit_list[from].pop_back(); // we delete one of the two for sure
            if (currentWeight(first_edge) > currentWeight(edge))
                active_forest.makeActive(edge.from,edge.to,edge.weight,edge_id); // we keep edge (it becomes active)
            else
                exit_list[from].back() = first_edge_id; // this amounts to the same as deleting the second to last elem in the exit list
        }
        passive_set[vi].clear();
    }

    return new_rep;
}

void Gabow::contractCompletePath(int root) {
    auto root_rep = co.find(root);
    for(auto vi : growth_path) co.join(vi,root_rep);
    for(auto vi : growth_path) active_forest.mergeHeaps(root_rep,vi); // merge all into old root_rep
    if(root_rep != co.find(root_rep)) active_forest.mergeHeaps(co.find(root_rep), root_rep); // then move to new root_rep
    growth_path.clear();
    path_edges.clear();
}