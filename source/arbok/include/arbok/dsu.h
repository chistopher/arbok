#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include "arbok/graph.h"

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

  void add_set_element(DSUKey element, SetKey set_element) {
    managedSets[find(element)].insert(set_element);
  }

  DSUKey operator[](DSUKey element) { return find(element); }
  void addOffset(DSUKey element, OffsetValue offset) { offsets[find(element)] += offset; }
  OffsetValue getOffset(DSUKey element) { return offsets[find(element)]; }
  set<SetKey>* getSetElements(DSUKey element) { return &managedSets[find(element)]; }
  void merge(DSUKey a, DSUKey b) {
    // TODO implement
  }

 private:
  vector<set<SetKey, SetCompare>> managedSets;
  vector<OffsetValue> offsets;
  vector<DSUKey> parent;
  DSUKey find(DSUKey element);
};

}  // namespace arbok
