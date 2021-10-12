
#include <arbok/gabow/gabow.h>
#include <memory>

using namespace arbok;

Gabow::Gabow(int n) : num_vertices(n), co(n), in_path(n, false), exit_list(n), passive_set(n) {
    growth_path.reserve(n); // cannot become larger
    active_set.reserve(n);

    for (int i = 0; i < num_vertices; i++) {
        active_set.push_back(std::make_shared<StdPQActiveSet>()); // cannot do this in initializer list cause it would only create a single object
    }
}

void Gabow::create_edge(int from, int to, int weight) {
    // a) insert EdgeLink objects into edges
    // b) build up edge list for each vertex in an incoming format (i.e. want to iterate over incoming edges later on)
    // the edge list probably again should point into the edges vector (or we use smart pointers, but that would be incoherent with the current setup)
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
}