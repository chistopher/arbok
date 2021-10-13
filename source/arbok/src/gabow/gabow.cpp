
#include <arbok/gabow/gabow.h>
#include <memory>
#include <optional>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <limits>

using namespace arbok;

Gabow::Gabow(int n) : num_vertices(n), co(n), incoming_edges(n), in_path(n, false), exit_list(n),passive_set(n), active_set_pointer(n, -1)  {
    growth_path.reserve(n); // cannot become larger
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
        create_edge(i, root, std::numeric_limits<int>::max());
    }
}

void Gabow::add_edge_to_exit_list(int v, int edge_id) {
    exit_list[v].push_front(edge_id);
    edges[edge_id].exit_list_iter = exit_list[v].begin();
}

void Gabow::insert_vertex_into_activeset(int v, int u, int key) {
    assert(active_set_pointer[co.find(v)] == -1);
    active_set[u]->insert(key, v);
    active_set_pointer[co.find(v)] = v;
}

void Gabow::init_root(int root) {
    growth_path[root] = true;
    // for each incoming edge (v,s) of root, add (v,s) to the front of the exit list of root
    // and insert v into active set of s with key weight((v,s))
    for (int edge_id: incoming_edges[root]) {
        add_edge_to_exit_list(root, edge_id);
        auto& edge = edges[edge_id].e; // I hope this does not copy - not sure tho...
        insert_vertex_into_activeset(edge.from, root, edge.weight);
    }
}

// Algorithm 3 in Report
void Gabow::extendPath(int u) {
    assert(co.find(u) == u); 
    in_path[u] = true;
    
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
        auto& edgel = edges[edge_id];
        auto& edge = edgel.e;
        int rep_x = co.find(edge.from);
        int x = edge.from;
        if (exit_list[rep_x].empty()) {
            insert_vertex_into_activeset(x, u, edge.weight);
            add_edge_to_exit_list(rep_x, edge_id);
        } else {
            // TBD lines 12-20 in report
        }
    }

}

void Gabow::contractPathPrefix(int u) {
    //TBD
}

// every vertex must be reachable from root
long long Gabow::run(int root) {
    // TODO: test alternative approach that sets root randomly, maybe in the fastestspeedrun scenario always starting
    // at 0 might be inefficient, because we always traverse an high-cost edge backwards first.

    ensure_strongly_connected(root);
    init_root(root);

    int cur_root = root;

    while (co.num_partitions > 1) {
        // do stuff (c.f. algo 1 in report)
        auto min = active_set[cur_root]->extractMin();
        int u_hat = min.second;
        int chosen_edge = exit_list[co.find(u_hat)].front();
        int u = edges[chosen_edge].e.from;

        chosen.push_back(chosen_edge);
        assert(co.find(u) == co.find(u_hat));

        if (!in_path[co.find(u)]) {
            cur_root = u;
            extendPath(u);
        } else {
            contractPathPrefix(u);
        }
    }
    // TODO: make sure that arboresence is _really_ rooted at root in phase 2 (reconstruction)
    return 0;
}