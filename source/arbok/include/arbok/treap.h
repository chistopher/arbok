
#pragma once

#include <utility>

namespace arbok {

    struct Edge {
        long long weight;
        int from, to;
        bool operator<(const Edge &o) const;
        bool operator==(const Edge &o) const;
    };

    const Edge MAXE = {(long long)1e18, (int)1e9, (int)1e9};

    struct Node {
        Node *l = nullptr, *r = nullptr;
        long unsigned int y;
        Edge x, mn;
        long long lz = 0;

        Node(long long weight, int from, int to);
    };

    Edge mn(Node *v);

    void update(Node *v);

    void apply(Node *v, long long lz);

    void push(Node *v);

    Node *merge(Node *l, Node *r);

    // returns a pair with the minimum node in the treap and the rest
    std::pair<Node *, Node *> split_min(Node *v);

} // end namespace arbok
