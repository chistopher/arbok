
#include <cassert>
#include <utility>
#include <queue>

#include <arbok/tarjan/tarjan_base.h>

namespace arbok {

class TarjanPQ : public Tarjan<TarjanPQ> {
    using PQElem = std::pair<int,int>; // (weight, id)
    //using PQ = std::priority_queue<PQElem, std::vector<PQElem>, decltype([](auto a, auto b) { return a.first > b.first; })>;
    using PQ = std::priority_queue<PQElem, std::vector<PQElem>, std::greater<>>;
    std::vector<PQ> managedSets;
    std::vector<int> offsets;

public:
    TarjanPQ(int n, int m) : Tarjan<TarjanPQ>(n,m), managedSets(n), offsets(n,0) { }

    void create_edge_internal(int /*from*/, int to, int weight, int idx) {
        managedSets[to].emplace(weight,idx);
    }

    std::pair<int,int> get_min_edge(int v) {
        assert(size(managedSets[v]));
        while(co.find(edges[managedSets[v].top().second].from) == v)
            managedSets[v].pop(); // delete selfloops
        auto res = managedSets[v].top();
        managedSets[v].pop(); // extract the edge that is returned
        res.first -= offsets[v];
        return res;
    }

    void update_incoming_edge_weights(int v, int w) {
        offsets[v] += w;
    }

    void move_edges(int from, int to) {
        // make sure set of from is smaller than to
        auto& small = managedSets[from];
        auto& large = managedSets[to];
        if(size(small)>size(large)) {
            swap(small, large);
            std::swap(offsets[from], offsets[to]);
        }

        // smaller into larger while applying offset
        while(size(small)) {
            auto e = small.top();
            small.pop();
            e.first -= offsets[from] - offsets[to];
            large.push(e);
        }
    }
};

}
