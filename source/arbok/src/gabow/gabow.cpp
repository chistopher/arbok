
#include <arbok/gabow/gabow.h>

#include <optional>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <limits>

using namespace arbok;

Gabow::Gabow(int n)
    : num_vertices(n)
    , co(n)
    , incoming_edges(n)
    , in_path(n, false)
    , exit_list(n)
    , passive_set(n)
    , active_forest(co)
{
    growth_path.reserve(n); // cannot become larger
    growth_path_edges.reserve(n);
    num_reps = num_vertices; // TODO remove this memeber
}

void Gabow::create_edge(int from, int to, int weight) {
    // This does two things:
    // a) insert EdgeLink objects into edges
    // b) build up edge list for each vertex in an incoming format (i.e. want to iterate over incoming edges later on)

    assert(from < num_vertices);
    assert(to < num_vertices);

    if (from == to) {
        //std::cout << "self loops are not allowed, skipping self loop for vertex " << from << " in graph creation" << std::endl;
        return;
    }

    int edge_id = static_cast<int>(edges.size()); // currently we assume we don't have too many edges to go out of int bounds...
    edges.emplace_back(from, to, weight, edge_id);

    incoming_edges[to].push_back(edge_id);
}


void Gabow::ensure_strongly_connected(int root) {
    // adds expensive edges from all vertices to root in case they don't have one yet
    std::unordered_set<int> can_reach_root;
    for (int edge_id : incoming_edges[root])  can_reach_root.insert(edges[edge_id].from);

    for (int i = 0; i < num_vertices; i++) {
        if (can_reach_root.contains(i)) continue;
        if (i == root) continue;
        create_edge(i, root, std::numeric_limits<int>::max());
    }
}

void Gabow::add_edge_to_exit_list(int v, int edge_id) {
    //std::cout << "adding edge from " << edges[edge_id].e.from << " to " << edges[edge_id].e.to << " to exit list from " << v << std::endl;
    assert(co.find(v) == co.find(edges[edge_id].from));
    exit_list[v].push_front(edge_id);
    edges[edge_id].exit_list_iter = exit_list[v].begin();
}

void Gabow::insert_edge_into_passiveset(int edge_id, int u) {
    //std::cout << "inserting edge " << edge_id << " from " << edges[edge_id].e.from << " to " << edges[edge_id].e.to << " into passive set of " << u << std::endl;
    passive_set[u].push_front(edge_id);
    edges[edge_id].passive_set_iter = passive_set[u].begin();
}

void Gabow::init_root(int root) {
    in_path[root] = true;
    growth_path.push_back(root);
    // for each incoming edge (v,s) of root, add (v,s) to the front of the exit list of root
    // and insert v into active set of s with key weight((v,s))
    for (int edge_id: incoming_edges[root]) {
        auto& edge = edges[edge_id]; // I hope this does not copy - not sure tho...
        add_edge_to_exit_list(edge.from, edge_id);
        active_forest.makeActive(edge);
    }
}

// Algorithm 3 in Report
void Gabow::extendPath(int u) {
    //std::cout << "extending path by find(" << co.find(u) << ") = " << u << std::endl;
    assert(in_path[u] == false);
    assert(co.find(u) == u); 
    in_path[u] = true;
    growth_path.push_back(u);

    assert(exit_list[u].empty());

    // lines 6-20 in report

    for (int edge_id : incoming_edges[u]) {
        //std::cout << std::endl << "handling edge " << edge_id << " from " << edges[edge_id].from << " to " << edges[edge_id].to << std::endl;
        auto& edge = edges[edge_id];
        assert(edge.from == co.find(edge.from) || !in_path[edge.from]);
        int rep_x = co.find(edge.from);
        if (exit_list[rep_x].empty()) {
            add_edge_to_exit_list(rep_x, edge_id);
            active_forest.makeActive(edge);
        } else {
            int front_edge_id = exit_list[rep_x].front();
            auto& front_edge = edges[front_edge_id];
            int vi = front_edge.to;
            if (vi != u) {
                int rep_vi = co.find(vi);
                insert_edge_into_passiveset(front_edge_id, rep_vi);
                add_edge_to_exit_list(rep_x, edge_id);
                active_forest.makeActive(edge);
            } else {
                if (edge.weight < front_edge.weight) { // we can use weight here since u was never contracted
                    exit_list[rep_x].pop_front();
                    front_edge.exit_list_iter = std::nullopt;
                    add_edge_to_exit_list(rep_x, edge_id);
                    active_forest.makeActive(edge);
                }
            }
        }
    }

}

int Gabow::contractPathPrefix(int u) {
    // std::cout << "contracting path prefix as we reached find(" << u << ") = " << co.find(u) << " again. growth path is " << growth_path.size() << " long." << std::endl;
    int rep_u = co.find(u);
    assert(in_path[rep_u]);
    assert(size(growth_path) == size(growth_path_edges));
    assert(size(growth_path_edges) == size(chosen_path));

    // TODO maybe we can merge some of these loops

    assert(find(begin(growth_path), end(growth_path), rep_u) != end(growth_path));
    int k = int(find(rbegin(growth_path), rend(growth_path), rep_u) - rbegin(growth_path));
    num_reps -= k; // TODO remove this

    for (int i = 0; i <= k; i++) {
        // the incoming edge that we chose for vertex vi corresponds to the same index in growth_path_edges ()
        int vi = growth_path[size(growth_path)-i-1];
        int edge_id = growth_path_edges[size(growth_path)-i-1];
        auto& edge = edges[edge_id];
        assert(co.find(edge.to) == vi);
        assert(co.find(vi) == vi); // all nodes on growth path are representatives
        co.add_value(vi, -currentWeight(edge,co));
    }

    assert(empty(passive_set[growth_path.back()]));
    for (int i = 1; i <= k; i++) {
        int vi = growth_path[size(growth_path)-i-1];
        assert(co.find(vi) == vi); // all nodes on growth path are representatives
        // invariant: after we are done with vi, all exit lists of nodes x have no passive edges from vertices v0,..,vi
        for (int edge_id : passive_set[vi]) { // not 100% sure whether rep here is correct, report says nothing, but I think it should be
            auto& edge = edges[edge_id];
            int rep_x = co.find(edge.from); // algo 4 line 8 of report says no DSU lookup, but text says so, and it makes sense to do so (I think)
            int first_edge_id = exit_list[rep_x].front();
            auto& first_edge = edges[first_edge_id];
            int first_edge_to = co.find(first_edge.to);
            
            assert(vi == co.find(edge.to));

            // the exit list of x should look like: (x,vj), (x,vi), ...
            // since all passive edges from nodes v0...v_{i-1} were deleted or became active
            if (currentWeight(first_edge,co) > currentWeight(edge,co)) {
                // we delete first_edge (x,vj) of x's exit list making the 2nd element (edge / (x,vi)) active
                // we reuse the active_set element of x which is currently (x,vj) located in the active set of vj
                // for this we move the active_set_element to the active set of vi and change the key of the element to (x,vi)
                assert(first_edge_to!=vi); // equivalent to "no multi-edges"
                assert(size(exit_list[rep_x])>=2); // namely first_edge and edge
                exit_list[rep_x].pop_front();
                first_edge.exit_list_iter = std::nullopt;
                assert(exit_list[rep_x].front()==edge_id); // next elem in x's exit list is edge
                active_forest.makeActive(edge);
            } else {
                assert(std::find(exit_list[rep_x].begin(), exit_list[rep_x].end(), edge_id) != exit_list[rep_x].end());
                assert(edge.exit_list_iter.has_value());
                assert(std::find(exit_list[rep_x].begin(), exit_list[rep_x].end(), edge_id) == edge.exit_list_iter.value());
                // TODO this is the only place we use `exit_list_iter`.
                //  we don't need it if edge is always the second one!
                //  think about multiedges here!
                //  should still work if edge order in exit_list[rep_x] has edges to vi in same order as they are in passive set[vi]
                exit_list[rep_x].erase(edge.exit_list_iter.value());
                edge.exit_list_iter = std::nullopt;
            }

            edge.passive_set_iter = std::nullopt;
        }

        // clean passive_set of rep_vi here (we cannot do that while iterating)
        passive_set[vi].clear();
    }

    for (int i = 1; i <= k; i++) {
        int vi = growth_path[size(growth_path)-i-1];
        assert(exit_list[vi].size() < 2); // exit list is empty or single element

        if (!exit_list[vi].empty()) {
            auto& first_edge = edges[exit_list[vi].front()];
            first_edge.exit_list_iter = std::nullopt;
            exit_list[vi].pop_front();
            active_forest.deleteActiveEdge(vi);
        }
        //assert(active_forest.active_edge[vi]==nullptr);
    }    

    // merge prefix in dsu
    for (int i = 1; i <= k; i++) {
        co.join(growth_path.back(), growth_path[size(growth_path)-i-1]);
    } 

    int new_root = co.find(growth_path.back());

    // meld all active sets of contracted prefix
    for (int i = 0; i <=k; i++) {
        int vi = growth_path[size(growth_path)-i-1];
        assert(co.find(vi) == new_root);
        assert(exit_list[vi].empty());
        assert(passive_set[vi].empty());
        in_path[vi] = false; // TODO do this somewhere else maybe
        if (vi == new_root) continue;
        active_forest.mergeHeaps(new_root, vi);
        //assert(active_forest.active_sets[vi].empty());
    }
    in_path[new_root] = true;

    // delete contracted path prefix (since path is saved reverse in memory we delete from back)
    for(int i=0;i<=k;++i) {
        growth_path.pop_back(), growth_path_edges.pop_back();
        // set the parent of the contracted edges in reconstruction forest
        forest[chosen_path.back()] = static_cast<int>(size(chosen)); // next iteration in run will find an incoming edge into the here contracted prefix; this edge will be the next in chosen
        chosen_path.pop_back();
    }
    growth_path.push_back(new_root);

    return new_root;
}

// every vertex must be reachable from root
long long Gabow::run(int root) {
    // TODO: test alternative approach that sets root randomly, maybe in the fastestspeedrun scenario always starting
    // at 0 might be inefficient, because we always traverse an high-cost edge backwards first.

    ensure_strongly_connected(root);
    init_root(root);
    int cur_root = root;

    long long answer = 0;
    // TODO: cannot use empty() here because fheap can't keep track of size()
    //while (!active_set[cur_root]->empty()) { // should be the same as "not everything contracted yet" because of strongly connected
    while (num_reps>1) { // should be the same as "not everything contracted yet" because of strongly connected

        assert(co.find(cur_root) == cur_root);
        // do not extract here because extend does this manually and contract does this when clearing out all exit lists of contracted vertices
        EdgeLink edge = active_forest.extractMin(cur_root);
        int u = co.find(edge.from);
        assert(exit_list[u].front() == edge.id);
        assert(u != cur_root); // no self loop

        // clear out exit list of u
        exit_list[u].pop_front(); // the active edge
        // ... then all the passive edges
        for (int edge_id : exit_list[u]) {
            auto& edgelink = edges[edge_id];
            assert(edgelink.passive_set_iter); // edge is passive
            passive_set[co.find(edgelink.to)].erase(edgelink.passive_set_iter.value());
            edgelink.passive_set_iter = std::nullopt;
            edgelink.exit_list_iter.reset();
        }
        exit_list[u].clear();

        // std::cout << std::endl <<  "removing minimum from active set of " << cur_root << " with weight " << edge.currentWeight() <<  std::endl;

        // reconstruction stuff
        int forest_id = static_cast<int>(std::size(chosen));
        chosen.push_back(edge.id);
        forest.push_back(forest_id); // new edge initially has no parent

        if (edge.weight < std::numeric_limits<int>::max())
            answer += currentWeight(edge,co);

        growth_path_edges.push_back(edge.id); // needed in both cases
        chosen_path.push_back(forest_id);

        if (!in_path[u]) {
            assert(edge.from == u); // edge.from is not a contracted vertex
            cur_root = u;
            extendPath(u);
        } else {
            cur_root = contractPathPrefix(u);
        }
    }

    for(int i=0; i<num_vertices; ++i) assert(co.find(i) == cur_root);

    // TODO: make sure that arboresence is _really_ rooted at root in phase 2 (reconstruction)
    return answer;
}

std::vector<Edge> Gabow::reconstruct(int root) {

    auto n = static_cast<int>(std::size(chosen));

    // build leafs arary (first chosen edge for each node)
    std::vector<int> leaf(num_vertices, -1);
    for(int i=0; i<n; ++i) {
        auto& edge = edges[chosen[i]];
        if(leaf[edge.to]==-1) leaf[edge.to] = i;
    }
    assert(none_of(begin(leaf), end(leaf), [](int l) { return l==-1; })); // assert each node has an incoming edge

    std::vector<int> res;
    std::vector del(n,false);
    for(int r=n-1; r>=0; --r) { // we exploit here that parent has always higher index; -> larger index first results in top-to-bottom traversal
        if(del[r]) continue;
        assert(forest[r]==n || del[forest[r]]); // we have a root (first check is due to last contraction having no incoming edge)
        auto& edge = edges[chosen[r]];
        if(edge.to != root) res.push_back(edge.id);
        auto leaf_edge_pos = leaf[edge.to];
        // we take the edge at position r and 'delete' the path from the leaf to the root from the forest
        del[r] = true;
        for(int i=leaf_edge_pos; i!=r; i=forest[i])
            del[i] = true;
    }

    assert(size(res)==num_vertices-1);

    std::vector<Edge> ret; // TODO change interface of reconstruct
    for(auto idx : res) {
        auto& e = edges[idx];
        ret.push_back({e.from, e.to, e.weight, e.weight});
    }

    return ret;
}
