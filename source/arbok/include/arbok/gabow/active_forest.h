#pragma once

#include <list>

#include <arbok/data_structures/compressed_tree.h>

struct FibHeapNode; // FWD

namespace arbok {

    class ActiveForest {
    public:
        explicit ActiveForest(CompressedTree<int>& _co);
        ~ActiveForest();

        void makeActive(int from, int to, int weight, int id);
        void deleteActiveEdge(int i);
        int extractMin(int i); // extract min from heap of node i, returns edge id
        void mergeHeaps(int i, int j);

    private:
        // fheap methods
        std::list<FibHeapNode*>& home_heap(FibHeapNode* v);
        void removeFromCurrentList(FibHeapNode* v);
        void moveHome(FibHeapNode* v);
        void loseChild(FibHeapNode* v);
        int curWeight(FibHeapNode* v);

        CompressedTree<int>& co;
        std::vector<FibHeapNode*> active_edge; // for each node the active outgoing edge
        std::vector<std::list<FibHeapNode*>> active_sets; // for each node on path the active set heap represented by the root list
    };

}