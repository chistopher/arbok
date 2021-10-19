
#include <arbok/gabow/gabow.h>
#include <algorithm>
#include <memory>
#include <optional>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <limits>

using namespace arbok;

Gabow::Gabow(int n) : num_vertices(n), co(n), offsets(n, 0), incoming_edges(n), in_path(n, false), exit_list(n),passive_set(n), active_set_pointer(n, -1)  {
    growth_path.reserve(n); // cannot become larger
    growth_path_edges.reserve(n);
    active_set.reserve(n);
    // TODO: somehow parametrize this in the type of active set used... not sure whether one can pass class objects as arguments in c++, otherwise it could be done using if statements and passing a string, I guess
    for (int i = 0; i < num_vertices; i++) {
        active_set.push_back(std::make_shared<StdPQActiveSet>()); // cannot do this in initializer list cause it would only create a single object
    }
}

void Gabow::debug() {
    std::cout << "size of incoming_edges: " << incoming_edges.size() << std::endl;
    std::cout << "size of incoming_edges[0]: " << incoming_edges[0].size() << std::endl;
    for (auto& edge: edges) {
        std::cout << "found edge id " << edge.id << " from " << edge.e.from << " to " << edge.e.to << " with weight " << edge.e.weight;
        if (edge.exit_list_iter) std::cout << " which has exit_list_iterator"; else std::cout << " which does not have exit_list_iterator";
        if (edge.passive_set_iter) std::cout << " which has passive_list_iterator"; else std::cout << " which does not have passive_list_iterator";

        std::cout << std::endl;
    }

    for (int i = 0; i < num_vertices; i++) {
        for (int edge_it : incoming_edges[i]) {
            std::cout << "found incoming edge of " << i << " originating at " << edges[edge_it].e.from << " and going to " << edges[edge_it].e.to << std::endl;
        }
    }
}

void Gabow::create_edge(int from, int to, int weight) {
    // This does two things:
    // a) insert EdgeLink objects into edges
    // b) build up edge list for each vertex in an incoming format (i.e. want to iterate over incoming edges later on)

    assert(from < num_vertices);
    assert(to < num_vertices);

    if (from == to) {
        std::cout << "self loops are not allowed, skipping self loop for vertex " << from << " in graph creation" << std::endl;
        return;
    }

    int edge_id = static_cast<int>(edges.size()); // currently we assume we don't have too many edges to go out of int bounds...
    edges.emplace_back(from, to, weight, edge_id, std::nullopt, std::nullopt);

    incoming_edges[to].push_back(edge_id);
}


void Gabow::ensure_strongly_connected(int root) {
    // adds expensive edges from all vertices to root in case they don't have one yet
    std::unordered_set<int> can_reach_root;
    for (int edge_id : incoming_edges[root])  can_reach_root.insert(edges[edge_id].e.from);

    for (int i = 0; i < num_vertices; i++) {
        if (can_reach_root.contains(i)) continue;
        if (i == root) continue;
        create_edge(i, root, std::numeric_limits<int>::max());
    }
}

void Gabow::add_edge_to_exit_list(int v, int edge_id) {
    std::cout << "adding edge from " << edges[edge_id].e.from << " to " << edges[edge_id].e.to << " to exit list from " << v << std::endl;
    assert(co.find(v) == co.find(edges[edge_id].e.from));
    exit_list[v].push_front(edge_id);
    edges[edge_id].exit_list_iter = exit_list[v].begin();
}

void Gabow::insert_vertex_into_activeset(int v, int u, int key) {
    std::cout << "inserting " << v << " (find = " << co.find(v) << ") into active set of " << u << " with key " << key << " (existing pointer: " << active_set_pointer[co.find(v)]  << ")" << std::endl;
    assert(active_set_pointer[co.find(v)] == -1);
    active_set[u]->insert(key, v);
    active_set_pointer[co.find(v)] = v;
}

void Gabow::insert_edge_into_passiveset(int edge_id, int u) {
    std::cout << "inserting edge " << edge_id << " from " << edges[edge_id].e.from << " to " << edges[edge_id].e.to << " into passive set of " << u << std::endl;
    passive_set[u].push_front(edge_id);
    edges[edge_id].passive_set_iter = passive_set[u].begin();
}

void Gabow::init_root(int root) {
    in_path[root] = true;
    growth_path.push_back(root);
    // for each incoming edge (v,s) of root, add (v,s) to the front of the exit list of root
    // and insert v into active set of s with key weight((v,s))
    for (int edge_id: incoming_edges[root]) {
        auto& edge = edges[edge_id].e; // I hope this does not copy - not sure tho...
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
    
    
    // lines 2-5 in report: data structure cleanup
    for (int edge_id : exit_list[u]) {
        auto& edgelink = edges[edge_id];
        if (edgelink.passive_set_iter) {
            // if edge is passive
            passive_set[co.find(edgelink.e.to)].erase(edgelink.passive_set_iter.value());
            edgelink.passive_set_iter = std::nullopt;
        }
    }
    exit_list[u].clear();

    // lines 6-20 in report

    for (int edge_id : incoming_edges[u]) {
        //std::cout << std::endl << "handling edge " << edge_id << " from " << edges[edge_id].e.from << " to " << edges[edge_id].e.to << std::endl;
        auto& edgel = edges[edge_id];
        auto& edge = edgel.e;
        int rep_x = co.find(edge.from);
        int x = edge.from;
        if (exit_list[rep_x].empty()) {
            insert_vertex_into_activeset(x, u, edge.weight);
            add_edge_to_exit_list(rep_x, edge_id);
        } else {
            int front_edge_id = exit_list[rep_x].front();
            auto& front_edge = edges[front_edge_id];
            int vi = front_edge.e.to;
            if (vi != u) {
                //std::cout << "exit list not empty, but edge in there goes to " << vi << ", not to " << u << " (edge id = " << front_edge_id << ")" << std::endl;
                int rep_vi = co.find(vi);
                insert_edge_into_passiveset(front_edge_id, rep_vi);
                int as_ptr = active_set_pointer[rep_x];
                assert(as_ptr > -1);
                //std::cout << "moving " << rep_x << " represented by " << as_ptr << " from heap of find(" << vi << ") = " << rep_vi << " to heap of " << u << " with new key " << edge.weight << std::endl;
                active_set[rep_vi]->move_and_modify_key(as_ptr, *active_set[u], edge.weight);
                add_edge_to_exit_list(rep_x, edge_id);
            } else {    
                if (edge.weight < front_edge.e.weight) {
                    exit_list[rep_x].pop_front();
                    front_edge.exit_list_iter = std::nullopt;
                    add_edge_to_exit_list(rep_x, edge_id);
                    int as_ptr = active_set_pointer[rep_x];
                    assert(as_ptr > -1);
                    active_set[u]->decreaseKey(as_ptr, edge.weight);
                }
            }
        }
    }

}

int Gabow::contractPathPrefix(int u) {
    std::cout << "contracting path prefix as we reached find(" << u << ") = " << co.find(u) << " again. growth path is " << growth_path.size() << " long." << std::endl;
    int rep_u = co.find(u);
    assert(in_path[rep_u]); 

    // reverse growth path such that growth_path[0] == v0 in report == latest root of growth path
    // TODO: not sure whether this reverse kills our runtime, if it goes in O(n) over the GP
    // dunno if we are allowed to do so here
    std::reverse(growth_path.begin(), growth_path.end());
    std::reverse(growth_path_edges.begin(), growth_path_edges.end());

    // DEBUG PRINT GROWTH PATH
    std::cout << "gp: ";
    for (int i = 0; i < growth_path.size(); i++) {
        std::cout << co.find(growth_path[i]) << " ";
    }
    std::cout << std::endl;

     std::cout << "gp edges: ";
    for (int i = 0; i < growth_path_edges.size(); i++) {
        std::cout << growth_path_edges[i] << " ";
    }
    std::cout << std::endl;
    // END
    

    int k = -1;
    for (int i = 0; i < growth_path.size(); i++) {
        if (rep_u == co.find(growth_path[i])) {
            k = i;
            break;
        }
    }

    assert(k > -1);
    std::cout << "k = " << k << std::endl;
    
    for (int i = 0; i <= k; i++) {
        // the incoming edge that we chose for vertex vi corresponds to the same index in growth_path_edges ()
        int vi = growth_path[i];
        int edge_id = growth_path_edges[i];
        auto& edge = edges[edge_id];

        int rep_vi = co.find(vi);
        offsets[rep_vi] = -(edge.e.weight + offsets[rep_vi]);
    }

    for (int i = 1; i <= k; i++) {
        int vi = growth_path[i];
        int rep_vi = co.find(vi);
        for (int edge_id : passive_set[rep_vi]) { // not 100% sure whether rep here is correct, report says nothing, but I think it should be
            auto& edge = edges[edge_id];
            int rep_x = co.find(edge.e.from); // algo 4 line 8 of report says no DSU lookup, but text says so, and it makes sense to do so (I think)
            int first_edge_id = exit_list[rep_x].front();
            auto& first_edge = edges[first_edge_id];
            
            assert(rep_vi == co.find(edge.e.to));

            if (first_edge.e.weight + offsets[co.find(first_edge.e.to)] > edge.e.weight + offsets[rep_vi]) {
                exit_list[rep_x].pop_front();
                first_edge.exit_list_iter = std::nullopt;
                active_set[co.find(first_edge.e.to)]->move_and_modify_key(active_set_pointer[rep_x], *active_set[co.find(edge.e.to)], edge.e.weight);
            } else {
                assert(std::find(exit_list[rep_x].begin(), exit_list[rep_x].end(), edge_id) != exit_list[rep_x].end());
                exit_list[rep_x].remove(edge_id);
                edge.exit_list_iter = std::nullopt;
            }

            edge.passive_set_iter = std::nullopt;
        }

        // clean passive_set of rep_vi here (we cannot do that while iterating)
        passive_set[rep_vi].clear();
    }

    for (int i = 1; i <= k; i++) {
        int vi = growth_path[i];
        int rep_vi = co.find(vi); // report does not state DSU lookup here but again I think it makes sense

        assert(exit_list[rep_vi].size() < 2); // exit list is empty or single element

        if (!exit_list[rep_vi].empty()) {
            int first_edge_id = exit_list[rep_vi].front();
            auto& first_edge = edges[first_edge_id];
            first_edge.exit_list_iter = std::nullopt;
            exit_list[rep_vi].pop_front();
            std::cout << "removing " << active_set_pointer[rep_vi] << " from active set of " << co.find(first_edge.e.to) << std::endl;
            active_set[co.find(first_edge.e.to)]->remove(active_set_pointer[rep_vi], false);
            active_set_pointer[rep_vi] = -1;
        }
    }    

    for (int i = 1; i <= k; i++) {
        co.join(growth_path[0], growth_path[i]);
    } 

    // DEBUG remove later
    int last_v = -1;
    for (int i = 0; i <= k; i++) {
        //std::cout << "growth path vertex " << i << " is now represented by " << co.find(growth_path[i]) << std::endl;
        bool same = last_v == -1 || last_v == co.find(growth_path[i]);
        assert(same);
        last_v = co.find(growth_path[i]);
    }

    int new_root = co.find(growth_path[0]);
    std::cout << "contracted vertices into " << new_root << std::endl;

    std::vector<std::shared_ptr<AbstractActiveSet>> active_sets_to_meld;

    for (int i = 0; i <=k; i++) {
        int vi = growth_path[i];
        if (vi == new_root) continue;
        active_sets_to_meld.push_back(active_set[vi]);
    }
    
    active_set[new_root]->meld(active_sets_to_meld);
    active_set_pointer[new_root] = -1;

    assert(exit_list[new_root].empty());
    assert(passive_set[new_root].empty());

    // in the following, we rebuild the growth path
    // i.e. if the growth path before was 3 2 0, and we reached 2 again, and contract into 42,
    // it is now 42 0
    // however, how it is currently implemented MAYBE this kills our worst-case runtime
    // because we have to iterate over the GP
    // this should be fixed, but as I don't know how to slice vectors in constant time
    // somebody else maybe can think about this
    // maybe its also okay to have O(n) over the growth path (I think so because we also need to find k in O(n))

    std::vector<int> new_growth_path;
    
    for (int j = growth_path.size() - 1; j > k; j--) { // restore growth path suffix
        new_growth_path.push_back(growth_path[j]);
    }
    new_growth_path.push_back(new_root);

    growth_path = std::move(new_growth_path);

    std::vector<int> new_growth_path_edges;
    for (int j = growth_path_edges.size() - 1; j > k; j--) { // restore growth path suffix
        new_growth_path_edges.push_back(growth_path_edges[j]);
    }
    growth_path_edges = std::move(new_growth_path_edges);

    // DEBUG PRINT GROWTH PATH
    std::cout << "new gp: ";
    for (int i = growth_path.size() - 1; i >= 0; i--) {
        std::cout << co.find(growth_path[i]) << " ";
    }
    std::cout << std::endl;

    std::cout << "new gp edges: ";
    for (int i = growth_path_edges.size() - 1; i >= 0; i--) {
        std::cout << growth_path_edges[i] << " ";
    }
    std::cout << std::endl;
    // END

    std::fill(in_path.begin(), in_path.end(), false);
    for (auto v: growth_path) in_path[v] = true;

    // TODO: we did not update offsets[new_root].
    // we should do this somehow, but I don't know how...

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
    while (co.num_partitions > 1) {
       
        auto min = active_set[cur_root]->extractMin();
        int u_hat = min.second;
        int u_hat_rep = co.find(u_hat);
        int chosen_edge = exit_list[u_hat_rep].front();
        int u = edges[chosen_edge].e.from;

        std::cout << std::endl <<  "removing minimum from active set of " << cur_root << " with weight " << edges[chosen_edge].e.weight << ", offset " << offsets[u_hat_rep] <<  std::endl;

        active_set_pointer[u_hat_rep] = -1;

        chosen.push_back(chosen_edge);

        if (edges[chosen_edge].e.orig_weight < std::numeric_limits<int>::max()) answer += edges[chosen_edge].e.weight + offsets[u_hat_rep];

        // TODO remove this later
        if (co.find(u) != co.find(u_hat)) {
            std::cerr << "assertion error. cur_root = " << cur_root << ", u = " << u << ", find(u) = " << co.find(u) << ", u_hat = " << u_hat << ", find(u_hat) = " << co.find(u_hat)  << ", edge weight = " << min.first << std::endl;
        }

        assert(co.find(u) == co.find(u_hat));
        assert(co.find(u) != cur_root); // no self loop

        growth_path_edges.push_back(chosen_edge); // needed in both cases

        if (!in_path[co.find(u)]) {
            cur_root = u;
            extendPath(u);
        } else {
            cur_root = contractPathPrefix(u);
        }
    }
    // TODO: make sure that arboresence is _really_ rooted at root in phase 2 (reconstruction)
    return answer;
}