#pragma once

struct edge {
  int from, to, weight;
  edge(int from, int to, int weight) : from(from), to(to), weight(weight) {}
  bool operator<(const edge &b) const {
    if (weight == b.weight) {
      return from < b.from;
    }
    return weight < b.weight;
  }
};