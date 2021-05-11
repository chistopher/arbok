
#pragma once

#include <utility>

#include <arbok/tarjan.h>

namespace arbok {

    struct Node {
        Node *l = nullptr, *r = nullptr;
        long unsigned int y;
        Edge x, mn;
        int lz = 0;

        Node(int from, int to, int weight);
    };

    Edge mn(Node *v);

    void update(Node *v);

    void apply(Node *v, int lz);

    void push(Node *v);

    Node *merge(Node *l, Node *r);

    // returns a pair with the minimum node in the treap and the rest
    std::pair<Node *, Node *> split_min(Node *v);

} // end namespace arbok
