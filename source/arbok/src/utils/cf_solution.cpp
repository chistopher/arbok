#include <arbok/utils/cf_solution.h>

#include <arbok/tarjan/tarjan.h>
#include <iostream>
#include <tuple>
#include <vector>

void arbok::cf_solution::solve(std::istream& inp, std::ostream& out, const arbok::TarjanVariant variant) {
    int n;
    inp >> n;
    std::vector<std::tuple<int, int, int>> edges;
    for (int level = 1; level <= n; level++) {
        int shortcut_item, time;
        inp >> shortcut_item >> time;
        edges.push_back({shortcut_item, level, time});
        for (int item = 0; item <= n; item++) {
            inp >> time;
            if (item != shortcut_item) {
                edges.push_back({item, level, time});
            }
        }
    }


    arbok::Tarjan tarjan(n+1, edges.size(), variant);
    for (auto [from, to, weight] : edges) {
        tarjan.create_edge(from, to, weight);
    }


    auto weight = tarjan.run(0);
    out << weight << std::endl;
}
