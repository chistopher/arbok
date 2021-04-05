#pragma once
#include <cstdint>
#include <vector>

#include "arbok/graph.h"

namespace arbok {

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class Tarjan {
 public:
  Tarjan(VertexIdentifier num_vertices) = delete;
  virtual ~Tarjan(){};  // virtual destructors need implementations
  virtual void create_edge(VertexIdentifier, VertexIdentifier, Weight) = 0;
  void run(VertexIdentifier root) final {
    queue<VertexIdentifier> q;
    vector<VertexIdentifier> pi(num_vertices, -1);
    // put all nodes v != root in queue
    for (VertexIdentifier vertex = 0; vertex < num_vertices; vertex++) {
        if (__builtin_expect(vertex != root,1)) q.push(vertex);
    }

    // while there is a node v in the queue
    while (!q.empty()) {
        VertexIdentifier v = co[q.front()];
        q.pop();

        auto& min_edge = get_min_edge(v):
        pi[v] = min_edge.from;

        Weight  w = get_edge_weight()


    }

  };
  Weight weight() const;
  virtual edge get_min_edge(VertexIdentifier v) = 0;

 protected:
  VertexIndentifier num_vertices;
};

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class SetTarjan : public Tarjan {
 public:
  SetTarjan() = default;
  SetTarjan(VertexIdentifier num_vertices) : co(num_vertices), cy(num_vetices), num_vertices(num_vertices) { };
  void create_edge(VertexIdentifier from, VertexIdentifier to, Weight weight) override {
      edge e(from, to, weight); // TODO: TEMPLATIZE EDGE
      co.add_set_element(to, e);
  };

 private:
  OffsetableSetManagingDSU<edge> co;
  CycleDetectionDSU cy;
};

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class MatrixTarjan : public Tarjan {
 public:
  MatrixTarjan() = default;
  MatrixTarjan(VertexIdentifier);
  void create_edge(VertexIdentifier, VertexIdentifier, Weight) override;
};

// TODO: TreapTarjan

}  // end namespace arbok
