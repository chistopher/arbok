#pragma once
#include <cstdint>
#include <iostream>
#include <set>
#include <queue>
#include <numeric>
#include <vector>
 
#include "arbok/graph.h"
#include "arbok/dsu.h"

using namespace std;

namespace arbok {

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class Tarjan {
 public:
  Tarjan(VertexIdentifier _num_vertices) : num_vertices(_num_vertices), cy(num_vertices), _weight(0) { };
  virtual ~Tarjan(){};  // virtual destructors need implementations
  virtual void create_edge(VertexIdentifier, VertexIdentifier, Weight) = 0;
  void run(VertexIdentifier root) {
    queue<VertexIdentifier> q;
    vector<VertexIdentifier> pi(num_vertices, -1);
    _weight = 0;
    // put all nodes v != root in queue
    for (VertexIdentifier vertex = 0; vertex < num_vertices; vertex++) {
        if (__builtin_expect(vertex != root,1)) q.push(vertex);
    }

    // while there is a node v in the queue
    while (!q.empty()) {
        VertexIdentifier v = identify(q.front()); 
        q.pop();

        auto& min_edge = get_min_edge(v);
        pi[v] = min_edge.from;

        Weight edge_weight = get_edge_weight(v, min_edge);
        _weight += edge_weight;
        
        update_incoming_edge_weights(v, edge_weight);

        if (!cy.merge(min_edge.from, min_edge.to)) continue;
        // we did build a cycle:

        // merge incoming edges of cycle, contract to one node
        for (VertexIdentifier cur = identify(pi[v]); cur != identify(v); cur = identify(pi[cur])) merge_vertices(cur, v);

        delete_self_loops_of_cycle(v);

        // push contracted node
        q.push(identify(v));
    }

  };
  Weight weight() { return _weight; };
  virtual VertexIdentifier identify(VertexIdentifier v) = 0;
  virtual edge& get_min_edge(VertexIdentifier v) = 0;
  virtual Weight get_edge_weight(VertexIdentifier v, edge e);
  virtual void update_incoming_edge_weights(VertexIdentifier v, Weight w) = 0;
  virtual void merge_vertices(VertexIdentifier a, VertexIdentifier b) = 0;
  virtual void delete_self_loops_of_cycle(VertexIdentifier v) = 0;

 protected:
  VertexIdentifier num_vertices;
  CycleDetectionDSU<> cy;
  Weight _weight;
};

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class SetTarjan : public Tarjan<VertexIdentifier, Weight> {
 public:
  SetTarjan() = default;
  SetTarjan(VertexIdentifier _num_vertices) : Tarjan<VertexIdentifier, Weight>(_num_vertices), co(_num_vertices) { };
  void create_edge(VertexIdentifier from, VertexIdentifier to, Weight weight) override {
      edge e(from, to, weight); // TODO: TEMPLATIZE EDGE
      co.add_set_element(to, e);
  };
  VertexIdentifier identify(VertexIdentifier v) { return co[v]; };
  edge& get_min_edge(VertexIdentifier v) { return *co.getSetElements(v)->begin(); };
  Weight get_edge_weight(VertexIdentifier v, edge e) { return  co.getOffset(v) + e.weight; };
  void update_incoming_edge_weights(VertexIdentifier v, Weight w) { co.addOffset(v, -w); };
  void merge_vertices(VertexIdentifier a, VertexIdentifier b) { co.merge(a, b); };
  void delete_self_loops_of_cycle(VertexIdentifier v) {
    while (co[co.getSetElements(v)->begin()->from] == co[v]) {
        co.getSetElements(v)->erase(co.getSetElements(v)->begin());
    }
  };
 private:
  OffsetableSetManagingDSU<edge> co;
};

/* template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class MatrixTarjan : public Tarjan {
 public:
  MatrixTarjan() = default;
  MatrixTarjan(VertexIdentifier);
  void create_edge(VertexIdentifier, VertexIdentifier, Weight) override;
}; */

// TODO: TreapTarjan

}  // end namespace arbok
