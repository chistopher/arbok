
#include <vector>
#include <list>

#include <arbok/tarjan/tarjan.h>
#include <arbok/gabow/active_forest.h>

namespace arbok {

    class Gabow {
    public:
        Gabow(int n);
        ~Gabow() = default;

        void create_edge(int from, int to, int weight);

        long long run(int root);

        std::vector<Edge> reconstruct(int root);

        int contractions = 0;
    protected:
        struct EdgeLink {
            int from, to, weight;
            bool ignore = false;
        };

        const int num_vertices;
        int num_reps;
        CompressedTree<int> co; // for actual merges and managing offsets

        std::vector<EdgeLink> edges;                  // all edges
        std::vector<std::vector<int>> incoming_edges; // adjacency list pointing into edges

        std::vector<int> growth_path;
        std::vector<int> growth_path_edges;
        std::vector<bool> in_path;

        std::vector<std::vector<int>> exit_list;   // stores the index into edges
        std::vector<std::vector<int>> passive_set; // stores the index into edges

        ActiveForest active_forest;

        // reconstruction
        // forest over chosen edges as outlined in reconstruction paper
        std::vector<int> chosen;      // chosen edges
        std::vector<int> forest;      // forest[i]: parent in reconstruction forest of edge edges[chosen[i]] (as ids into chosen)
        std::vector<int> chosen_path; // edges on growth path (just as growth_path_edges) but as ids into chosen

        void add_edge_to_exit_list(int v, int edge_id);
        inline auto currentWeight(const EdgeLink& e) { return e.weight + co.find_value(e.to); }
        void ensure_strongly_connected(int root);
        void extendPath(int u);
        int contractPathPrefix(int u);
    };

} // end namespace arbok
