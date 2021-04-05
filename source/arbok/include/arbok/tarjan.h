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
    _weight = 0;
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

        Weight edge_weight = get_edge_weight(v, min_edge);
        _weight += edge_weight;
        
        update_incoming_edge_weights(v, wedge_weight);

        if (!cy.merge(min_edge.from, min_edge.to)) continue;
        // we did build a cycle:

        // merge incoming edges of cycle, contract to one node
        for (VertexIdentifier cur = co[pi[v]]; cur != co[v]; cur = co[pi[cur]]) merge_vertices(cur, v);

        delete_self_loops_of_cycle(v);

        // push contracted node
        q.push(co[v]);
    }

  };
  Weight weight() { return _weight; };
  virtual edge get_min_edge(VertexIdentifier v) = 0;
  virtual Weight get_edge_weight(VertexIdentifier v, edge e);
  virtual void update_incoming_edge_weights(VertexIndentifier v, Weight w) = 0;
  virtual void merge_vertices(VertexIdentifier a, VertexIdentifier b) = 0;
  virtual void delete_self_loops_of_cycle(VertexIdentifier v) = 0;

 protected:
  VertexIndentifier num_vertices;
  CycleDetectionDSU cy;
  Weight _weight;
};

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class SetTarjan : public Tarjan {
 public:
  SetTarjan() = default;
  SetTarjan(VertexIdentifier num_vertices) : co(num_vertices), cy(num_vetices), num_vertices(num_vertices), _weight(0) { };
  void create_edge(VertexIdentifier from, VertexIdentifier to, Weight weight) override {
      edge e(from, to, weight); // TODO: TEMPLATIZE EDGE
      co.add_set_element(to, e);
  };

  edge get_min_edge(VertexIdentifier v) { return *co.getSetElements(v)->begin(); };
  Weight get_edge_weight(VertexIdentifier v, edge e) { return  co.getOffset(v) + e.weight; };
  void update_incoming_edge_weights(VertexIndentifier v, Weight w) { co.addWeightOffset(v, -w); };
  void merge_vertices(VertexIdentifier a, VertexIdentifier b) { co.merge(a, b); };
  void delete_self_loops_of_cycle(VertexIdentifier v) {
    while (co[co.getEdges(v)->begin()->from] == co[v]) {
        co.getEdges(v)->erase(co.getEdges(v)->begin());
    }
  };
 private:
  OffsetableSetManagingDSU<edge> co;
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