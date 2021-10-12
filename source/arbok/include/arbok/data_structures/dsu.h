#pragma once

#include <cstdint>
#include <set>
#include <vector>

namespace arbok {

struct DSU {
  std::vector<int> p;
  int num_partitions;
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

}  // namespace arbok
