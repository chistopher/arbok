#pragma once
#include <cstdint>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <vector>

#include "arbok/dsu.h"
#include "arbok/graph.h"

using namespace std;

namespace arbok {

template <class VertexIdentifier = uint32_t, class Weight = int64_t>
class Tarjan {
 public:
  Tarjan(VertexIdentifier _num_vertices)
      : num_vertices(_num_vertices), cy(num_vertices), _weight(0){};
  virtual ~Tarjan(){};  // virtual destructors need implementations
  virtual void create_edge(VertexIdentifier, VertexIdentifier, Weight) = 0;
  void run(VertexIdentifier root) {
    queue<VertexIdentifier> q;
    vector<VertexIdentifier> pi(num_vertices, -1);
    _weight = 0;
    // put all nodes v != root in queue
    for (VertexIdentifier vertex = 0; vertex < num_vertices; vertex++) {
      if (__builtin_expect(vertex != root, 1))
        q.push(vertex);
    }

    // while there is a node v in the queue
    while (!q.empty()) {
      VertexIdentifier v = identify(q.front());
      q.pop();

      auto min_edge = get_min_edge(v);
      pi[v] = min_edge.from;

      Weight edge_weight = get_edge_weight(v, min_edge);
      _weight += edge_weight;

      update_incoming_edge_weights(v, edge_weight);

      if (!cy.merge(min_edge.from, min_edge.to))
        continue;
      // we did build a cycle:

      // merge incoming edges of cycle, contract to one node
      for (VertexIdentifier cur = identify(pi[v]); cur != identify(v); cur = identify(pi[cur]))
        merge_vertices(cur, v);

      delete_self_loops_of_cycle(v);

      // push contracted node
      q.push(identify(v));
    }
  };
  Weight weight() { return _weight; };
  virtual VertexIdentifier identify(VertexIdentifier v) = 0;
  virtual edge<VertexIdentifier, Weight> get_min_edge(VertexIdentifier v) = 0;
  virtual Weight get_edge_weight(VertexIdentifier v, edge<VertexIdentifier, Weight> e) = 0;
  virtual void update_incoming_edge_weights(VertexIdentifier v, Weight w) = 0;
  virtual void merge_vertices(VertexIdentifier a, VertexIdentifier b) = 0;
  virtual void delete_self_loops_of_cycle(VertexIdentifier v) = 0;

 protected:
  VertexIdentifier num_vertices;
  CycleDetectionDSU<VertexIdentifier> cy;
  Weight _weight;
};

template <class VertexIdentifier = uint32_t, class Weight = int64_t>
class SetTarjan : public Tarjan<VertexIdentifier, Weight> {
 public:
  SetTarjan() = default;
  SetTarjan(VertexIdentifier _num_vertices)
      : Tarjan<VertexIdentifier, Weight>(_num_vertices), co(_num_vertices){};
  void create_edge(VertexIdentifier from, VertexIdentifier to, Weight weight) override {
    edge<VertexIdentifier, Weight> e(from, to, weight);
    co.add_set_element(to, e);
  };
  VertexIdentifier identify(VertexIdentifier v) { return co[v]; };
  edge<VertexIdentifier, Weight> get_min_edge(VertexIdentifier v) {
    return *co.getSetElements(v)->begin();
  };
  Weight get_edge_weight(VertexIdentifier v, edge<VertexIdentifier, Weight> e) {
    return co.getOffset(v) + e.weight;
  };
  void update_incoming_edge_weights(VertexIdentifier v, Weight w) { co.addOffset(v, -w); };
  void merge_vertices(VertexIdentifier a, VertexIdentifier b) { co.merge(a, b); };
  void delete_self_loops_of_cycle(VertexIdentifier v) {
    while (co[co.getSetElements(v)->begin()->from] == co[v]) {
      co.getSetElements(v)->erase(co.getSetElements(v)->begin());
    }
  };

 private:
  OffsetableSetManagingDSU<edge<VertexIdentifier, Weight>> co;
};

template <class VertexIdentifier = uint32_t, class Weight = int64_t>
class MatrixTarjan : public Tarjan<VertexIdentifier, Weight> {
 public:
  MatrixTarjan() = default;
  MatrixTarjan(VertexIdentifier _num_vertices)
      : Tarjan<VertexIdentifier, Weight>(_num_vertices),
        co(_num_vertices),
        NO_EDGE(numeric_limits<VertexIdentifier>::max()) {
    adj.resize(_num_vertices,
               vector<Weight>(_num_vertices, std::numeric_limits<VertexIdentifier>::max()));
  };
  void create_edge(VertexIdentifier from, VertexIdentifier to, Weight weight) override {
    adj[to][from] = weight;  // we save backwards edges in the adjacency matrix
  };
  VertexIdentifier identify(VertexIdentifier v) { return co[v]; };
  edge<VertexIdentifier, Weight> get_min_edge(VertexIdentifier v) {
    pair<Weight, VertexIdentifier> mw_mo = {NO_EDGE, 0};
    // iterate over the adjacency matrix
    for (VertexIdentifier origin = 0; origin < adj.size(); origin++) {
      if (co[origin] == v)
        continue;  // we don't care about self loops
      mw_mo = min(mw_mo, {adj[v][origin], co[origin]});
    }
    auto& [min_edge_weight, min_edge_origin] = mw_mo;
    return edge<VertexIdentifier, Weight>(min_edge_origin, v, min_edge_weight);
  };
  Weight get_edge_weight(VertexIdentifier, edge<VertexIdentifier, Weight> e) { return e.weight; };
  void update_incoming_edge_weights(VertexIdentifier v, Weight w) {
    for (VertexIdentifier origin = 0; origin < adj.size(); origin++) {
      if (adj[v][origin] == NO_EDGE)
        continue;
      if (origin == v)
        continue;
      adj[v][origin] -= w;
    }
  };
  void merge_vertices(VertexIdentifier cur, VertexIdentifier v) {
    co.merge(cur, v);  // merge cur into v
    for (VertexIdentifier origin = 0; origin < adj.size(); origin++) {
      adj[v][co[origin]] = min(adj[v][co[origin]], adj[cur][co[origin]]);
    }
  };
  void delete_self_loops_of_cycle(VertexIdentifier){/* NOP for MatrixTarjan */};

 private:
  vector<vector<Weight>> adj;
  CycleDetectionDSU<VertexIdentifier> co;
  VertexIdentifier NO_EDGE;
};
// TODO: TreapTarjan

}  // end namespace arbok
