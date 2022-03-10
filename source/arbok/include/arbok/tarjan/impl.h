
#include <arbok/data_structures/hollow_heap.h>
#include <arbok/data_structures/treap.h>
#include <arbok/tarjan/tarjan.h>
#include <queue>

namespace arbok {


class TarjanImpl {
public:
    virtual ~TarjanImpl() = default;
    virtual void create_edge(int from, int to, int weight) = 0;
    virtual Edge get_min_edge(int v, arbok::DSU& dsu) = 0; // returns original edge w/o reps or modified weights
    virtual void update_incoming_edge_weights(int v, int w) = 0;
    virtual void move_edges(int from, int to) = 0;
};

class SetImpl : public TarjanImpl {
public:
    SetImpl(int n);
    ~SetImpl() override = default;
    void create_edge(int from, int to, int weight) override;
    Edge get_min_edge(int v, arbok::DSU& dsu) override;
    void update_incoming_edge_weights(int v, int w) override;
    void move_edges(int from, int to) override;
private:
    std::vector<std::set<Edge>> managedSets;
    std::vector<int> offsets;
};

class PQImpl : public TarjanImpl {
public:
    PQImpl(int n);
    ~PQImpl() override = default;
    void create_edge(int from, int to, int weight) override;
    Edge get_min_edge(int v, arbok::DSU& dsu) override;
    void update_incoming_edge_weights(int v, int w) override;
    void move_edges(int from, int to) override;
private:
    using PQ = std::priority_queue<Edge, std::vector<Edge>, std::greater<>>;
    std::vector<PQ> managedSets;
    std::vector<int> offsets;
};

class MatrixImpl : public TarjanImpl {
public:
    MatrixImpl(int n);
    ~MatrixImpl() override = default;
    void create_edge(int from, int to, int weight) override;
    Edge get_min_edge(int v, arbok::DSU& dsu) override;
    void update_incoming_edge_weights(int v, int w) override;
    void move_edges(int from, int to) override;
private:
    std::vector<std::vector<Edge>> adj;
};

class TreapImpl : public TarjanImpl {
public:
    TreapImpl(int n);
    ~TreapImpl() override;
    void create_edge(int from, int to, int weight) override;
    Edge get_min_edge(int v, arbok::DSU& dsu) override;
    void update_incoming_edge_weights(int v, int w) override;
    void move_edges(int from, int to) override;
private:
    std::vector<treap::Node*> managedSets;
};

class HollowHeapImpl : public TarjanImpl {
  public:
    HollowHeapImpl(int n);
    ~HollowHeapImpl() override = default;
    void create_edge(int from, int to, int weight) override;
    Edge get_min_edge(int v, arbok::DSU &dsu) override;
    void update_incoming_edge_weights(int v, int w) override;
    void move_edges(int from, int to) override;

  private:
    using HH = otto::hollow_heap<Edge, std::less<>>;
    std::vector<HH> managedSets;
};
}
