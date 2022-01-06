
#include <tuple>
#include <vector>
#include <set>
#include <list>
#include <functional>
#include <memory>
#include <optional>
#include <cassert>

#include <arbok/tarjan/tarjan.h>
#include <arbok/data_structures/activeset.h>
#include <arbok/data_structures/edgelink.h>

namespace arbok
{

enum class GabowVariant { DUMMY, FIB };

    class Gabow
    {
    public:
        Gabow(int n, GabowVariant variant=GabowVariant::FIB);
        ~Gabow() = default;

        void create_edge(int from, int to, int weight);

        long long run(int root);

        std::vector<Edge> reconstruct(int root);

    protected:
        const int num_vertices;
        int num_reps;
        CompressedTree<int> co; // for actual merges and managing offsets

        std::vector<EdgeLink> edges;                  // all edges
        std::vector<std::vector<int>> incoming_edges; // adjacency list pointing into edges

        std::vector<int> growth_path;
        std::vector<int> growth_path_edges;
        std::vector<bool> in_path;

        std::vector<std::list<int>> exit_list;   // stores the index into edges
        std::vector<std::list<int>> passive_set; // stores the index into edges
        std::vector<std::shared_ptr<AbstractActiveSet>> active_set;
        std::vector<std::shared_ptr<AbstractActiveSetHandle>> active_set_handle; // for each node its handle
        std::vector<int> in_which_active_set;                                     // TODO remove in final impl; this is just for debugging

        // reconstruction
        // forest over chosen edges as outlined in reconstruction paper
        std::vector<int> chosen;      // chosen edges
        std::vector<int> forest;      // forest[i]: parent in reconstruction forest of edge edges[chosen[i]] (as ids into chosen)
        std::vector<int> chosen_path; // edges on growth path (just as growth_path_edges) but as ids into chosen

        void add_edge_to_exit_list(int v, int edge_id);
        void insert_vertex_into_activeset(int v, int u, int key); // insert v into u's AS
        void transfer_active_status(const EdgeLink &from, const EdgeLink &to);
        void insert_edge_into_passiveset(int edge_id, int u); // insert edge_id into u's PS
        void init_root(int root);
        void ensure_strongly_connected(int root);
        void extendPath(int u);
        int contractPathPrefix(int u);
    private:
        std::shared_ptr<AbstractActiveSet> new_active_set();
        std::shared_ptr<AbstractActiveSetHandle> new_active_set_handle();
        GabowVariant variant_;
    };

} // end namespace arbok
