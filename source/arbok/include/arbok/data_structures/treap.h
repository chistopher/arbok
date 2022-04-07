#pragma once

#include <cassert>
#include <utility>
#include <arbok/tarjan/tarjan.h> // TODO: We need this for the Edge data type, but maybe the Edge data type should be refactored into its own header due to encapsulation (treap shouldn't know Tarjan, per se...)

namespace arbok::treap {

    struct Node {
        Node *l = nullptr, *r = nullptr;
        long unsigned int y;
        Edge x, mn;
        int lz = 0;

        Node() = default;
        Node(int from, int to, int weight);
        Node(Node&&) = default;
        Node& operator=(const Node&) = default;
    };

    class allocator {
      public:
        allocator (int m) : node_object_allocation_(m), next_node_(0) {}
        Node* new_node(Node node) {
            assert(next_node_ < node_object_allocation_.size());
            node_object_allocation_[next_node_] = node;
            return &node_object_allocation_[next_node_++];
        }
      private:
        std::vector<Node> node_object_allocation_;
        int next_node_;
    };

    Edge mn(Node *v);

    void update(Node *v);

    void apply(Node *v, int lz);

    void push(Node *v);

    Node *merge(Node *l, Node *r);

    // returns a pair with the minimum node in the treap and the rest
    std::pair<Node *, Node *> split_min(Node *v);

} // end namespace arbok::treap
