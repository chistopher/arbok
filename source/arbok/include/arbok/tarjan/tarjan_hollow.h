
#include <cassert>
#include <utility>

#include <arbok/tarjan/tarjan_base.h>
#include <arbok/data_structures/hollow_heap.h>

namespace arbok {

class TarjanHollow : public Tarjan<TarjanHollow> {
    using HH = otto::hollow_heap<std::pair<int,int>, std::less<>>;
    HH::allocator allocator_;
    std::vector<HH> managedSets;

public:
    TarjanHollow(int n, int m) : Tarjan<TarjanHollow>(n,m), allocator_(m)  {
        managedSets.reserve(n);
        for (int i = 0; i < n; ++i) {
            managedSets.emplace_back(&allocator_);
        }
    }

    void create_edge_internal(int /*from*/, int to, int weight, int idx) {
        managedSets[to].push({weight,idx});
    }

    std::pair<int,int> get_min_edge(int v) {
        assert(size(managedSets[v]));
        while (co.find(edges[managedSets[v].top().second].from) == v)
            managedSets[v].pop(); // delete self loops
        auto res = managedSets[v].top();
        managedSets[v].pop(); // extract the edge that is returned
        return res;
    }

    void update_incoming_edge_weights(int v, int w) {
        managedSets[v].apply_update(-w);
    }

    void move_edges(int from, int to) {
        managedSets[to].merge(managedSets[from]);
    }
};

}
