#pragma once

#include <list>

#include <arbok/gabow/edgelink.h>

struct FibHeapNode; // FWD

namespace arbok {

    class ActiveForest {
    public:
        explicit ActiveForest(CompressedTree<int>& _co);
        ~ActiveForest();

        void makeActive(EdgeLink link); // creates new fibheap handle or reuses the one in active_edge[link.from]
        void deleteActiveEdge(int i);
        EdgeLink extractMin(int i); // extract min from heap of node i
        void mergeHeaps(int i, int j);

    private:
        // fheap methods
        std::list<FibHeapNode*>& home_heap(FibHeapNode* v);
        void removeFromCurrentList(FibHeapNode* v);
        void moveHome(FibHeapNode* v);
        void loseChild(FibHeapNode* v);

        CompressedTree<int>& co;
        std::vector<FibHeapNode*> active_edge; // for each node the active outgoing edge
        std::vector<std::list<FibHeapNode*>> active_sets; // for each node on path the active set heap represented by the root list
    };

}