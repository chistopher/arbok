
#include <arbok/tarjan.h>

namespace arbok {


class TarjanImpl {
public:
    struct Edge {
        int from, to, weight;
        inline bool operator<(const Edge& rhs) const { return tie(weight,from) < tie(rhs.weight, rhs.from); }
    };
    virtual ~TarjanImpl() = default;
    virtual void create_edge(int from, int to, int weight) = 0;
    virtual Edge get_min_edge(int v, arbok::DSU& dsu) = 0;
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
    vector<vector<int>> adj;
};

}
