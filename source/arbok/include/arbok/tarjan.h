#pragma once
#include <cstdint>
#include <vector>

namespace arbok {

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class Tarjan {
 public:
  Tarjan(VertexIdentifier) = delete;
  virtual ~Tarjan(){};  // virtual destructors need implementations
  virtual void create_edge(VertexIdentifier, VertexIdentifier, Weight) = 0;
  void run() final;
  Weight weight() const;

 protected:
  VertexIndentifier _num_vertices;
};

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class SetTarjan : public Tarjan {
 public:
  SetTarjan() = default;
  SetTarjan(VertexIdentifier);
  void create_edge(VertexIdentifier, VertexIdentifier, Weight) override;

 private:
  vector<vector<edge>> _graph;
}

template <class VertexIdentifier = uint32_t, class Weight = int32_t>
class MatrixTarjan : public Tarjan {
 public:
  MatrixTarjan() = default;
  MatrixTarjan(VertexIdentifier);
  void create_edge(VertexIdentifier, VertexIdentifier, Weight) override;
}

// TODO: TreapTarjan

}  // end namespace arbok
