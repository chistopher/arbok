#pragma once

struct edge {
  int from, to, weight;
  edge(int _from, int _to, int _weight) : from(_from), to(_to), weight(_weight) {}
  bool operator<(const edge &b) const {
    if (weight == b.weight) {
      return from < b.from;
    }
    return weight < b.weight;
  }
};