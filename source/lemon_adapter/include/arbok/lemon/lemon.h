#pragma once

#include <vector>
#include <memory>

#include <arbok/tarjan/tarjan.h>

namespace arbok {

struct LemonImpl; // FWD

class Lemon {
public:
    Lemon(int n);
    ~Lemon();

    void create_edge(int from, int to, int weight);
    long long run(int root);
    std::vector<Edge> reconstruct(int root);

    int contractions = 0;
protected:
    const int num_vertices;
    std::unique_ptr<LemonImpl> m_impl; // PImpl pattern (https://en.cppreference.com/w/cpp/language/pimpl)
};

}  // end namespace arbok
