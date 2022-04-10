
#include <cassert>
#include <utility>

#include <arbok/tarjan/tarjan_base.h>

namespace arbok {

class TarjanMatrix : public Tarjan<TarjanMatrix> {
    using Elem = std::pair<int,int>; // weight from
    std::vector<std::vector<Elem>> adj;
    static constexpr int INF = 1e9+3;

public:
    TarjanMatrix(int n, int m) : Tarjan<TarjanMatrix>(n,m), adj(n, std::vector<Elem>(n,{INF,-1})) { }

    void create_edge_internal(int from, int to, int weight, int idx) {
        adj[to][from] = min(adj[to][from], {weight,idx});  // we save backwards edges in the adjacency matrix
    }

    std::pair<int,int> get_min_edge(int v) {
        Elem mn = {INF,-1};
        // iterate over the adjacency matrix
        for (int origin = 0; origin < num_vertices; origin++)
            if (co.find(origin) != v) // we don't care about self loops
                mn = min(mn, adj[v][origin]);
        assert(mn.second!=-1);
        return mn;
    }

    void update_incoming_edge_weights(int v, int w) {
        for (int origin = 0; origin < num_vertices; origin++)
            if (adj[v][origin].second != -1) // TODO why was v skipped here before?
                adj[v][origin].first -= w;
    }

    void move_edges(int from, int to) {
        assert(from != to);
        for (int origin = 0; origin < num_vertices; origin++)
            adj[to][origin] = min(adj[to][origin], adj[from][origin]);
        // outgoing edges are not updates because we find them also from non-reps in get-min-edge
    }
};

}
