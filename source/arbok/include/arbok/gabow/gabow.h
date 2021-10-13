
#include <tuple>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <memory>
#include <optional>

#include <arbok/data_structures/dsu.h>
#include <arbok/data_structures/activeset.h>

namespace arbok {

/*struct OffsetDSU {
    std::vector<int> p; // ?
    //explicit OffsetDSU(int n);
    int find(int i);
    int find_value(int i);
    bool join(int i, int j);
};*/

struct Edge {
    Edge(int _from, int _to, int _weight, int _orig_weight) : from(_from), to(_to), weight(_weight), orig_weight(_orig_weight) {};
    int from, to, weight, orig_weight;
    inline bool operator<(const Edge& rhs) const { return std::tie(weight,from) < std::tie(rhs.weight, rhs.from); }
    inline bool operator==(const Edge& rhs) const { return std::tie(weight,from) == std::tie(rhs.weight,rhs.from); }
    inline bool operator>(const Edge& rhs) const { return rhs < *this; }
};
static const Edge NO_EDGE = {(int)1e9, (int)1e9, (int)1e9, (int)1e9};

class Gabow {
public:
    Gabow(int n);
    ~Gabow() = default;

    void create_edge(int from, int to, int weight);
    void init_root(int root);
    void ensure_strongly_connected();
    long long run(int root);

    void debug();
    // std::vector<Edge> reconstruct();

protected:
    const int num_vertices;
    DSU co; // for actual merges; TODO this needs to also manage offsets and needs to work with path compression // 2021 update: kp was wir hier meinten mit dem todo, das sollte ja eig so sein

    struct EdgeLink {
        EdgeLink(int from, int to, int weight, int _id, std::optional<std::list<int>::iterator> _exit_list_iter, std::optional<std::list<int>::iterator> _passive_list_iter) : e(from, to, weight, weight), id(_id), exit_list_iter(_exit_list_iter), passive_list_iter(_passive_list_iter) {};
        Edge e;
        int id; // pos in edges
        std::optional<std::list<int>::iterator> exit_list_iter;
        std::optional<std::list<int>::iterator> passive_list_iter;
        //std::optional<std::set<int>::iterator> active_set_iter; // active set = fib heap, so this does not make sense
    };

    std::vector<EdgeLink> edges; // all edges
    std::vector<std::vector<int>> incoming_edges; // adjacency list pointing into edges

    std::vector<int> growth_path;
    std::vector<bool> in_path;

    std::vector<std::list<int>> exit_list; // stores the index into edges
    std::vector<std::list<int>> passive_set; // stores the index into edges
    std::vector<std::shared_ptr<AbstractActiveSet>> active_set; // shared_ptr to avoid object-cutoff that would happen if we insert polymorphic objects here

    std::vector<Edge> chosen;
};

}  // end namespace arbok

