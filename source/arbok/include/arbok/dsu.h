#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include "arbok/graph.h"

using namespace std;

namespace arbok {

struct DSU {
  std::vector<int> p;
  explicit DSU(int n);
  int find(int i);
  bool same(int i, int j);
  int size(int i);
  bool join(int i, int j);
};

struct PersistentDSU {
  std::vector<std::pair<int, int>> p;
  explicit PersistentDSU(int n);
  int find(int i, int t = 1e9);
  bool same(int i, int j, int t = 1e9);
  int size(int i, int t);
  bool join(int i, int j, int t);
};

// SetKey is currently required to have  a "weight" variable, which should (TODO) be defined using a C++20 concept
template <class SetKey, class SetCompare = std::less<SetKey>, class DSUKey = uint32_t,
          class OffsetValue = int64_t>
class OffsetableSetManagingDSU {
 public:
  explicit OffsetableSetManagingDSU(DSUKey num_elements) {
    parent.resize(num_elements);
    offsets.resize(num_elements, 0);
    managedSets.resize(num_elements);
    iota(parent.begin(), parent.end(), 0);
  };

  void add_set_element(DSUKey element, SetKey set_element) { managedSets[find(element)].insert(set_element); }
  DSUKey operator[](DSUKey element) { return find(element); }
  void addOffset(DSUKey element, OffsetValue offset) { offsets[find(element)] += offset; }
  OffsetValue getOffset(DSUKey element) { return offsets[find(element)]; }
  set<SetKey>* getSetElements(DSUKey element) { return &managedSets[find(element)]; }
  void merge(DSUKey a, DSUKey b) {
    if (find(a) == find(b)) return;
    auto& setA = managedSets[find(a)];
    OffsetValue offsetA = offsets[find(a)];
    auto& setB = managedSets[find(b)];
    OffsetValue offsetB = offsets[find(b)];

    // Herausfinden, welches Set größer ist
    set<SetKey> *smallerSet = &setA;
    OffsetValue smallerOffset = offsetA;
    set<SetKey> *largerSet = &setB;
    OffsetValue largerOffset = offsetB;
    DSUKey smallerRep = find(a);
    DSUKey largerRep = find(b);
    if (setA.size() > setB.size()) {
        swap(largerSet, smallerSet);
        swap(largerOffset, smallerOffset);
        swap(largerRep, smallerRep);
    }

    // Offset anwenden auf smaller set, smaller into larger
    for (auto edgeIt = smallerSet->begin(); edgeIt != smallerSet->end(); ) {

        auto toMove = edgeIt++;
        auto e = smallerSet->extract(toMove);

        e.value().weight += smallerOffset - largerOffset;
        largerSet->insert(move(e));
        //largerSet->emplace(e.from, e.to, e.weight + smallerOffset - largerOffset);
    }

    parent[smallerRep] = largerRep; // largerRep is new representant
  }

 private:
  vector<set<SetKey, SetCompare>> managedSets;
  vector<OffsetValue> offsets;
  vector<DSUKey> parent;
  DSUKey find(DSUKey element);
};

template <class DSUKey = uint32_t>
class CycleDetectionDSU {
    vector<DSUKey> parent;
public:
    CycleDetectionDSU(DSUKey num_elements) {
        parent.resize(num_elements);
        iota(parent.begin(), parent.end(), 0);
    }
    bool merge(DSUKey a, DSUKey b) {
        // nicht kommutativ
        if(find(a) == find(b)){
            // Cycle
            return true;
        }
        parent[find(a)] = find(b);
        return false;
    }
    int operator[](DSUKey element) {
        // returns set representant
        return find(element);
    }
private:
    int find(DSUKey element) {
        if (parent[element] != element) {
            parent[element] = find(parent[element]);
        }
        return parent[element];
    }
};


}  // namespace arbok
