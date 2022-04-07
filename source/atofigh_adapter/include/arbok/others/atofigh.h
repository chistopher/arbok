#pragma once

#include <vector>
#include <memory>

namespace arbok {

struct AtofighImpl; // FWD

class Atofigh {
public:
    Atofigh(int n, int m);
    ~Atofigh();

    void create_edge(int from, int to, int weight);
    long long run(int root);
    std::vector<int> reconstruct(int root);

protected:
    const int num_vertices;
    std::unique_ptr<AtofighImpl> m_impl; // PImpl pattern (https://en.cppreference.com/w/cpp/language/pimpl)
};

}  // end namespace arbok
