#pragma once

#include <arbok/data_structures/edgelink.h>
#include <arbok/data_structures/compressed_tree.h>

struct FibHeapNode {
    arbok::EdgeLink data;

    // TODO all other fib member
};


namespace arbok {
    class ActiveForest {
    public:
        explicit ActiveForest(CompressedTree<int>& _co);

        void makeActive(EdgeLink link); // creates new fibheap handle or reuses the one in active_edge[link.from]
        EdgeLink deleteActiveEdge(int i); // returns the deleted edge object
        EdgeLink extractMin(int i); // extract min from heap of node i
        void mergeHeaps(int i, int j);

        CompressedTree<int>& co;
        std::vector<FibHeapNode*> active_edge; // for each node the active outgoing edge
        std::vector<std::list<FibHeapNode*>> active_sets; // for each node on path the active set heap represented by the root list

    private:
        auto home_heap(FibHeapNode* v) { return co.find(v->data.to); }
        auto value(FibHeapNode* v) { return v->data.weight + co.find_value(v->data.to); }

        // fheap methods
        void link();
        void cut();
        void meld();
        void move();
        // ...
    };

}