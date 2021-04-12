#pragma once

template <class VertexIdentifier = uint32_t, class Weight = int64_t>
class edge {
public:
  VertexIdentifier from, to;
  Weight weight;
  edge(VertexIdentifier _from, VertexIdentifier _to, Weight _weight) : from(_from), to(_to), weight(_weight) {}
  bool operator<(const edge &b) const {
    if (weight == b.weight) {
      return from < b.from;
    }
    return weight < b.weight;
  }
};