
#include <arbok/gabow/gabow.h>
#include <arbok/data_structures/activeset.h>

#include <optional>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <limits>

using namespace arbok;

Gabow::Gabow(int n,  GabowVariant variant) : num_vertices(n), co(n), incoming_edges(n), in_path(n, false), exit_list(n),passive_set(n), variant_(variant)  {
    in_which_active_set.resize(n,-1);
    growth_path.reserve(n); // cannot become larger
    growth_path_edges.reserve(n);

        for (int i = 0; i < n; i++) {
            active_set.push_back(new_active_set());
            active_set_handle.push_back(new_active_set_handle());
        }
    num_reps = num_vertices; // TODO remove this memeber
}

std::shared_ptr<AbstractActiveSet>  Gabow::new_active_set() {
    if (variant_ == GabowVariant::DUMMY) {
        return std::make_shared<DummyActiveSet>();
    } else if (variant_ == GabowVariant::FIB) {
        return std::make_shared<FibHeapActiveSet>();
    }
    return std::make_shared<DummyActiveSet>(); // default
}

std::shared_ptr<AbstractActiveSetHandle>  Gabow::new_active_set_handle() {
    if (variant_ == GabowVariant::DUMMY) {
        return std::make_shared<DummyActiveSetHandle>();
    } else if (variant_ == GabowVariant::FIB) {
        return std::make_shared<FibHeapActiveSetHandle>();
    }
    return std::make_shared<DummyActiveSetHandle>(); // default
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
    edges.emplace_back(from, to, weight, edge_id, co);

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

void Gabow::insert_vertex_into_activeset(int v, int u, int key) {
    std::cout << "inserting " << v << " (find = " << co.find(v) << ") into active set of " << u << " with key " << key << std::endl;
    assert(v == co.find(v));
    //v = co.find(v);
    assert(in_which_active_set[v] == -1);
    assert(!exit_list[v].empty());
    auto& edge = edges[exit_list[v].front()];
    std::cout << "edge weight = " << edge.currentWeight() << std::endl;

    assert(key == edge.currentWeight());
    auto handle = active_set[u]->insert(edge);
    active_set_handle[v] = handle;
    in_which_active_set[v] = u;
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
        insert_vertex_into_activeset(edge.from, root, edge.weight);
    }
}

// Algorithm 3 in Report
void Gabow::extendPath(int u) {
    std::cout << "extending path by find(" << co.find(u) << ") = " << u << std::endl;
    assert(in_path[u] == false);
    assert(co.find(u) == u); 
    in_path[u] = true;
    growth_path.push_back(u);

    assert(exit_list[u].empty());

    // lines 6-20 in report

    for (int edge_id : incoming_edges[u]) {
        std::cout << std::endl << "handling edge " << edge_id << " from " << edges[edge_id].from << " to " << edges[edge_id].to << std::endl;
        auto& edge = edges[edge_id];
        assert(edge.from == co.find(edge.from) || !in_path[edge.from]);
        int rep_x = co.find(edge.from);
        if (exit_list[rep_x].empty()) {
            add_edge_to_exit_list(rep_x, edge_id);
            insert_vertex_into_activeset(rep_x, u, edge.weight);
        } else {
            int front_edge_id = exit_list[rep_x].front();
            auto& front_edge = edges[front_edge_id];
            int vi = front_edge.to;
            if (vi != u) {
                std::cout << "exit list not empty, but edge in there goes to " << vi << ", not to " << u << " (edge id = " << front_edge_id << ")" << std::endl;
                int rep_vi = co.find(vi);
                insert_edge_into_passiveset(front_edge_id, rep_vi);
                assert(in_which_active_set[rep_x] != -1);
                assert(co.find(in_which_active_set[rep_x]) == rep_vi); // TODO this should fail after use *move* because we cannot maintain in_wich_active_set anymore
                std::cout << "moving " << rep_x << " represented by " << rep_x << " from heap of find(" << vi << ") = " << rep_vi << " to heap of " << u << " with new key " << edge.weight << std::endl;
                if (rep_x == 0) {
                    std::cout << "break here!" << std::endl;
                }
                add_edge_to_exit_list(rep_x, edge_id);
                transfer_active_status(front_edge,edge);
            } else {
                if (edge.weight < front_edge.weight) {
                    exit_list[rep_x].pop_front();
                    front_edge.exit_list_iter = std::nullopt;
                    add_edge_to_exit_list(rep_x, edge_id);
                    assert(in_which_active_set[rep_x] == u);
                    // we don't need to steal because this is a multi-edge and the heap elem is already in the right set
                    active_set[u]->decreaseKey(active_set_handle[rep_x], edge);
                }
            }
        }
    }

}

int Gabow::contractPathPrefix(int u) {
    std::cout << "contracting path prefix as we reached find(" << u << ") = " << co.find(u) << " again. growth path is " << growth_path.size() << " long." << std::endl;
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
        co.add_value(vi, -edge.currentWeight());
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
            if (first_edge.currentWeight() > edge.currentWeight()) {
                // we delete first_edge (x,vj) of x's exit list making the 2nd element (edge / (x,vi)) active
                // we reuse the active_set element of x which is currently (x,vj) located in the active set of vj
                // for this we move the active_set_element to the active set of vi and change the key of the element to (x,vi)
                assert(first_edge_to!=vi); // equivalent to "no multi-edges"
                assert(size(exit_list[rep_x])>=2); // namely first_edge and edge
                exit_list[rep_x].pop_front();
                first_edge.exit_list_iter = std::nullopt;
                assert(exit_list[rep_x].front()==edge_id); // next elem in x's exit list is edge
                transfer_active_status(first_edge, edge);
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
            int first_edge_id = exit_list[vi].front();
            auto& first_edge = edges[first_edge_id];
            first_edge.exit_list_iter = std::nullopt;
            exit_list[vi].pop_front();
            //std::cout << "removing " << rep_vi << " from active set of " << co.find(first_edge.e.to) << std::endl;
            assert(in_which_active_set[vi] != -1);
            assert(co.same(in_which_active_set[vi], first_edge.to));
            active_set[co.find(first_edge.to)]->remove(active_set_handle[vi]);
            active_set_handle[vi] = new_active_set_handle();
            in_which_active_set[vi] = -1;
        }
        assert(in_which_active_set[vi]==-1);
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
        active_set[new_root]->meld(active_set[vi]); // after this meld, in_which_active_set pointers still point to the vi instead of new_root. remember to make a lookup later
        //assert(active_set[vi].empty()); // TODO uncomment this as soon as we have .empty()
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
        EdgeLink edge = active_set[cur_root]->pop();
        int u = co.find(edge.from);
        assert(exit_list[u].front() == edge.id);
        assert(u != cur_root); // no self loop
        assert(co.find(in_which_active_set[u]) == cur_root);

        // clear out exit list of u
        // first the active edge ...
        in_which_active_set[u] = -1;
        active_set_handle[u] = new_active_set_handle(); // TODO how about a reset?
        exit_list[u].pop_front();
        // ... then all the passive edges
        for (int edge_id : exit_list[u]) {
            auto& edgelink = edges[edge_id];
            assert(edgelink.passive_set_iter); // edge is passive
            passive_set[co.find(edgelink.to)].erase(edgelink.passive_set_iter.value());
            edgelink.passive_set_iter = std::nullopt;
            edgelink.exit_list_iter.reset();
        }
        exit_list[u].clear();

        std::cout << std::endl <<  "removing minimum from active set of " << cur_root << " with weight " << edge.currentWeight() <<  std::endl;

        // reconstruction stuff
        int forest_id = static_cast<int>(std::size(chosen));
        chosen.push_back(edge.id);
        forest.push_back(forest_id); // new edge initially has no parent

        if (edge.weight < std::numeric_limits<int>::max())
            answer += edge.currentWeight();

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

void Gabow::transfer_active_status(const EdgeLink& source, const EdgeLink& target) {
    int x = co.find(source.from);
    assert(co.same(source.from, target.from)); // both edges should be in the exit list of the same node
    assert(in_which_active_set[x] != -1);
    assert(co.same(in_which_active_set[x], source.to)); //
    auto& source_set = active_set[co.find(source.to)];
    auto& target_set = active_set[co.find(target.to)];

    // we also tried decreaseKey here as previously used, but this also fails.
    source_set->unsafeSetKey(active_set_handle[x], target);
    target_set->steal(active_set_handle[x]);
    in_which_active_set[x] = co.find(target.to); // TODO remove later
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
