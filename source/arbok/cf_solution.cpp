#include "arbok/cf_solution.h"

#include "arbok/tarjan.h"
#include <iostream>

void arbok::cf_solution::solve(std::istream& inp, std::ostream& out, const arbok::TarjanVariant variant) {
    int n;
    inp >> n;
    arbok::Tarjan tarjan(n+1, variant);
    for (int level = 1; level <= n; level++) {
        int shortcut_item, time;
        inp >> shortcut_item >> time;
        tarjan.create_edge(shortcut_item, level, time);
        for (int item = 0; item <= n; item++) {
            inp >> time;
            if (item != shortcut_item) {
                tarjan.create_edge(item, level, time);
            }
        }
    }

    auto weight = tarjan.run(0);
    out << weight << endl;
}
