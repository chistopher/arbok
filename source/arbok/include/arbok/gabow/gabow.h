
#include <vector>
#include <list>

#include <arbok/gabow/active_forest.h>

namespace arbok {

    class Gabow {
    public:
        Gabow(int n, int m);
        ~Gabow() = default;

        void create_edge(int from, int to, int weight);

        long long run(int root);

        std::vector<int> reconstruct(int root);

        int contractions = 0;
    protected:
        struct EdgeLink {
            int from, to, weight;
            bool ignore;
        };

        const int num_vertices;
        CompressedTree<int> co; // for actual merges and managing offsets

        std::vector<EdgeLink> edges;                  // all edges
        std::vector<std::vector<int>> incoming_edges; // adjacency list pointing into edges

        std::vector<int> growth_path;
        std::vector<int> path_edges; // edges on growth path but as ids into chosen
        std::vector<bool> in_path;

        std::vector<std::vector<int>> exit_list;   // stores the index into edges
        std::vector<std::vector<int>> passive_set; // stores the index into edges

        ActiveForest active_forest;

        // forest over chosen edges as outlined in reconstruction paper
        std::vector<int> chosen;      // chosen edges
        std::vector<int> forest;      // forest[i]: parent in reconstruction forest of edge edges[chosen[i]] (as ids into chosen)

        inline auto currentWeight(const EdgeLink& e) { return e.weight + co.find_value(e.to); }
        void extendPath(int u);
        int contractPathPrefix(int u);
        void contractCompletePath(int root);
    };

} // end namespace arbok
