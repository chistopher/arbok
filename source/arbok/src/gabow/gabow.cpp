
#include <arbok/gabow/gabow.h>
#include <memory>
#include <optional>
#include <iostream>
#include <cassert>

using namespace arbok;

Gabow::Gabow(int n) : num_vertices(n), co(n), incoming_edges(n), in_path(n, false), exit_list(n),passive_set(n)  {
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
        if (edge.passive_list_iter) std::cout << " which has passive_list_iterator"; else std::cout << " which does not have passive_list_iterator";

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
    int edge_id = static_cast<int>(edges.size()); // currently we assume we don't have too many edges to go out of int bounds...
    edges.emplace_back(from, to, weight, edge_id, std::nullopt, std::nullopt);

    incoming_edges[to].push_back(edge_id);
}


void Gabow::ensure_strongly_connected() {
    // TODO
}

void Gabow::init_root(int root) {
    growth_path[root] = true;

    // for each incoming edge (v,s) of root, add (v,s) to the front of the exit list of root
    // and insert v into active set of s with key weight((v,s))
}

long long Gabow::run(int root) {
    ensure_strongly_connected();
    init_root(root);   
    while (co.num_partitions > 1) {
        // do stuff (c.f. algo 1 in report)
    }
    // TODO: make sure that arboresence is _really_ rooted at root in phase 2 (reconstruction)

    return 0;
}