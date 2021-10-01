
#include <arbok/gabow.h>

arbok::Gabow::Gabow(int n) : num_vertices(n) {

    for(int i=0; i<num_vertices; ++i) {
        active_set.emplace_back([this](int a, int b){
            // TODO do we need this find? I would say that we can guarantee active sets to contain only representatives
            auto& ea = edges[exit_list[co.find(a)].front()].e;
            auto& eb = edges[exit_list[co.find(b)].front()].e;
            return ea.weight + co.find_value(a) < eb.weight + co.find_value(b);
        });
    }

}
