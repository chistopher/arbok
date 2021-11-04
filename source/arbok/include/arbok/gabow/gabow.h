
#include <tuple>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <memory>
#include <optional>
#include <cassert>

#include <arbok/data_structures/dsu.h>
#include <arbok/data_structures/compressed_tree.h>

namespace arbok {

struct Edge {
    Edge(int _from, int _to, int _weight, int _orig_weight) : from(_from), to(_to), weight(_weight), orig_weight(_orig_weight) {};
    int from, to, weight, orig_weight;
    inline bool operator<(const Edge& rhs) const { return std::tie(weight,from) < std::tie(rhs.weight, rhs.from); }
    inline bool operator==(const Edge& rhs) const { return std::tie(weight,from) == std::tie(rhs.weight,rhs.from); }
    inline bool operator>(const Edge& rhs) const { return rhs < *this; }
};
static const Edge NO_EDGE = {(int)1e9, (int)1e9, (int)1e9, (int)1e9};

struct EdgeLink {
    EdgeLink(int from, int to, int weight, int _id, std::optional<std::list<int>::iterator> _exit_list_iter, std::optional<std::list<int>::iterator> _passive_set_iter, CompressedTree<int>& _dsu)
    : e(from, to, weight, weight), id(_id), exit_list_iter(_exit_list_iter), passive_set_iter(_passive_set_iter), dsu(_dsu) {};
    EdgeLink(const EdgeLink& rhs) = default; // default copy constructor
    EdgeLink& operator=(const EdgeLink& rhs) { e = rhs.e; id = rhs.id; exit_list_iter = rhs.exit_list_iter; passive_set_iter = rhs.passive_set_iter; return *this; } // cannot default this because of dsu reference
    Edge e;
    int id; // pos in edges
    std::optional<std::list<int>::iterator> exit_list_iter;
    std::optional<std::list<int>::iterator> passive_set_iter;
    CompressedTree<int>& dsu;
    auto currentWeight() const { return e.weight + dsu.find_value(e.to); }
    bool operator<(const EdgeLink& other) const { return currentWeight() < other.currentWeight(); }
};

class HackActiveSetDummy {
public:
    using value_type = EdgeLink;
    using iterator = std::list<value_type>::iterator;

    size_t size() const { return std::size(m_elems); }
    bool empty() const { return std::empty(m_elems); }
    const value_type& top() { return *std::min_element(begin(m_elems), end(m_elems)); }
    void pop() { m_elems.erase(std::min_element(begin(m_elems), end(m_elems))); }
    void meld(HackActiveSetDummy&& other) { m_elems.splice(end(m_elems), other.m_elems); }
    iterator insert(value_type v) { m_elems.emplace_back(v); return prev(end(m_elems)); }
    void remove(iterator it) { m_elems.erase(it); }
    void decreaseKey(iterator it, value_type new_elem) { *it = new_elem; } // this is a bit hacky
    void steal(iterator it, HackActiveSetDummy& other) { m_elems.splice(end(m_elems), other.m_elems, it); } // move it from other into *this
private:
    std::list<value_type> m_elems;
};

class Gabow {
public:
    Gabow(int n);
    ~Gabow() = default;

    void create_edge(int from, int to, int weight);

    long long run(int root);

    void debug();
    // std::vector<Edge> reconstruct();

protected:
    const int num_vertices;
    CompressedTree<int> co; // for actual merges and managing offsets

    std::vector<EdgeLink> edges; // all edges
    std::vector<std::vector<int>> incoming_edges; // adjacency list pointing into edges

    std::vector<int> growth_path;
    std::vector<int> growth_path_edges;
    std::vector<bool> in_path;

    std::vector<std::list<int>> exit_list; // stores the index into edges
    std::vector<std::list<int>> passive_set; // stores the index into edges
    std::vector<HackActiveSetDummy> active_set;
    std::vector<HackActiveSetDummy::iterator> active_set_pointer; // for each node, in which active set is it and the corresponding iterator
    std::vector<int> in_which_active_set; // TODO remove in final impl; this is just for debugging

    std::vector<int> chosen;

    void add_edge_to_exit_list(int v, int edge_id);
    void insert_vertex_into_activeset(int v, int u, int key); // insert v into u's AS
    void transfer_active_status(const EdgeLink& from, const EdgeLink& to);
    void insert_edge_into_passiveset(int edge_id, int u); // insert edge_id into u's PS
    void init_root(int root);
    void ensure_strongly_connected(int root);
    void extendPath(int u);
    int contractPathPrefix(int u);

};

}  // end namespace arbok

