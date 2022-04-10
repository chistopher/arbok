
#include <cassert>

#include <arbok/tarjan/tarjan_base.h>
#include <arbok/data_structures/treap.h>

namespace arbok {

class TarjanTreap : public Tarjan<TarjanTreap> {
    std::vector<treap::Node*> managedSets;
    treap::allocator allocator_;

public:
    TarjanTreap(int n, int m) : Tarjan<TarjanTreap>(n,m), managedSets(n, nullptr), allocator_{m} { }

    void create_edge_internal(int /*from*/, int to, int weight, int idx) {
        managedSets[to] = treap::merge(managedSets[to], allocator_.new_node(treap::Node(weight,idx)));
    }

    std::pair<int,int> get_min_edge(int v) {
        assert(managedSets[v] != nullptr);
        treap::Node* res = nullptr;
        while(res == nullptr) {
            std::tie(res, managedSets[v]) = treap::split_min(managedSets[v]); // res becomes min and m[v] keeps the rest
            assert(res);
            if(co.find(edges[res->x.second].from) == v) {
                //delete res;
                res = nullptr;
            }
        }
        auto e = res->x;
        //delete res;
        return e;
    }

    void update_incoming_edge_weights(int v, int w) {
        treap::apply(managedSets[v], -w);
    }

    void move_edges(int from, int to) {
        managedSets[to] = treap::merge(managedSets[from], managedSets[to]);
        managedSets[from] = nullptr;
    }
};

}
