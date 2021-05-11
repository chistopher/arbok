
#include <arbok/tarjan.h>

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
    vector<set<Edge>> managedSets;
    vector<int> offsets;
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
    vector<vector<Edge>> adj;
};

struct Node; // FWD for treap
class TreapImpl : public TarjanImpl {
public:
    TreapImpl(int n);
    ~TreapImpl() override;
    void create_edge(int from, int to, int weight) override;
    Edge get_min_edge(int v, arbok::DSU& dsu) override;
    void update_incoming_edge_weights(int v, int w) override;
    void move_edges(int from, int to) override;
private:
    vector<Node*> managedSets;
};

}
