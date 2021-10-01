#include <iostream>
#include <vector>
#include <tuple>
#include <memory>
#include <set>
#include <cassert>

using namespace std;

struct DSU {
    std::vector<int> p;
    explicit DSU(int n);
    int find(int i);
    bool same(int i, int j);
    int size(int i);
    bool join(int i, int j);
};

DSU::DSU(int n) : p(n, -1) {}
int DSU::find(int i) {
    return p[i] < 0 ? i : p[i] = find(p[i]);
}
bool DSU::same(int i, int j) { return find(i) == find(j); }
int DSU::size(int i) { return -p[find(i)]; }
bool DSU::join(int i, int j) {
    i = find(i), j = find(j);
    if (i == j) return false;
    if (p[i] > p[j]) swap(i, j);
    p[i] += p[j], p[j] = i;
    return true;
}

// TODO rename this to be more expressive; what this does is manage the edges
class TarjanImpl; // FWD
enum class TarjanVariant { SET, MATRIX, TREAP, PQ };
struct Edge {
    int from, to, weight, orig_weight;
    inline bool operator<(const Edge& rhs) const { return tie(weight,from) < tie(rhs.weight, rhs.from); }
    inline bool operator==(const Edge& rhs) const { return tie(weight,from) == tie(rhs.weight,rhs.from); }
    inline bool operator>(const Edge& rhs) const { return rhs < *this; }
};
static const Edge NO_EDGE = {(int)1e9, (int)1e9, (int)1e9, (int)1e9};

class Tarjan {
public:
    Tarjan(int n, TarjanVariant variant);
    ~Tarjan();

    void create_edge(int from, int to, int weight);
    long long run(int root);

protected:
    const int num_vertices;
    DSU cy; // for weakly connected components
    DSU co; // for actual merges

    // last time a representative appeared in the queue
    // this is the pos in inc where the most recent incoming edge is stored for this node
    vector<unsigned> queue_id;
    // edges that the algorithm picked in each iteration of the queue
    vector<Edge> inc;
    // forest over the edges in inc that stores parent indices
    std::vector<unsigned> forest;

    std::unique_ptr<TarjanImpl> m_impl; // PImpl pattern (https://en.cppreference.com/w/cpp/language/pimpl)
};



class TarjanImpl {
public:
    virtual ~TarjanImpl() = default;
    virtual void create_edge(int from, int to, int weight) = 0;
    virtual Edge get_min_edge(int v, DSU& dsu) = 0; // returns original edge w/o reps or modified weights
    virtual void update_incoming_edge_weights(int v, int w) = 0;
    virtual void move_edges(int from, int to) = 0;
};

class SetImpl : public TarjanImpl {
public:
    SetImpl(int n);
    ~SetImpl() override = default;
    void create_edge(int from, int to, int weight) override;
    Edge get_min_edge(int v, DSU& dsu) override;
    void update_incoming_edge_weights(int v, int w) override;
    void move_edges(int from, int to) override;
private:
    vector<set<Edge>> managedSets;
    vector<int> offsets;
};

Tarjan::~Tarjan() = default;

Tarjan::Tarjan(int n, TarjanVariant variant)
        : num_vertices(n)
        , cy(n)
        , co(n)
        , queue_id(n)
{
    inc.reserve(2*n);
    forest.reserve(2*n);
    if(variant == TarjanVariant::SET)
        m_impl = make_unique<SetImpl>(n);
}

void Tarjan::create_edge(int from, int to, int weight) {
    m_impl->create_edge(from, to, weight);
}

long long Tarjan::run(int root) {

    // put all nodes v != root in queue
    vector<int> q;
    q.reserve(2*num_vertices);
    for (int vertex = 0; vertex < num_vertices; vertex++)
        if (vertex != root)
            q.push_back(vertex);

    // while there is a node v in the queue
    long long answer = 0;
    for(int i=0; i<size(q); ++i) {
        int v = q[i];
        queue_id[v] = i;
        assert(v==co.find(v));

        const auto min_edge = m_impl->get_min_edge(v, co);
        assert(co.find(min_edge.from) != v); // no self-loops allowed here

        inc.push_back(min_edge); // inc edges are stored at queue_id[node] because same node can be rep multiple times
        forest.push_back(i); // at first each edge is a root in forest

        answer += min_edge.weight;
        m_impl->update_incoming_edge_weights(v, min_edge.weight);

        if (cy.join(min_edge.from, min_edge.to))
            continue;

        // we built a cycle. now merge incoming edges of cycle into one node
        int merged = v;
        auto next_in_cyc = [&](int last) { return co.find(inc[queue_id[last]].from); };
        for (int cur = next_in_cyc(v); cur != merged; cur = next_in_cyc(cur)) {
            int from = cur, to = merged;
            co.join(cur, merged);
            if (co.find(merged)==cur) swap(from,to);
            m_impl->move_edges(from, to);
            merged = to;

            forest[queue_id[cur]] = static_cast<unsigned>(size(q)); // size of queue is where the merged supernode will be in the queue
        }
        forest[i] = static_cast<unsigned>(size(q)); // edge to v is also part of cycle

        // push contracted node
        q.push_back(merged);
    }

    return answer;
}

SetImpl::SetImpl(int n) : managedSets(n), offsets(n,0) {
}

void SetImpl::create_edge(int from, int to, int weight) {
    managedSets[to].insert({from, to, weight, weight});
}

Edge SetImpl::get_min_edge(int v, DSU& dsu) {
    assert(size(managedSets[v]));
    while(dsu.find(managedSets[v].begin()->from) == v)
        managedSets[v].erase(managedSets[v].begin()); // delete selfloops
    auto res = *managedSets[v].begin();
    managedSets[v].erase(managedSets[v].begin()); // extract the edge that is returned
    res.weight -= offsets[v];
    return res;
}

void SetImpl::update_incoming_edge_weights(int v, int w) {
    offsets[v] += w;
}

void SetImpl::move_edges(int from, int to) {
    // make sure set of from is smaller than to
    auto& small = managedSets[from];
    auto& large = managedSets[to];
    if(size(small)>size(large)) {
        swap(small, large);
        swap(offsets[from], offsets[to]);
    }

    // smaller into larger while applying offset
    while(size(small)) {
        auto e = small.extract(begin(small));
        e.value().weight -= offsets[from] - offsets[to];
        large.insert(move(e));
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    //freopen("../data/fastestspeedrun/035-itsybitsycycles05.in", "r", stdin);

    int n;
    cin >> n;
    Tarjan tarjan(n+1, TarjanVariant::SET);
    // G.resize(n + 1);
    for (int level = 1; level <= n; level++) {
        int shortcut_item, time;
        cin >> shortcut_item >> time;
        tarjan.create_edge(shortcut_item, level, time);
        for (int item = 0; item <= n; item++) {
            cin >> time;
            if (item != shortcut_item) {
                tarjan.create_edge(item, level, time);
            }
        }
    }

    auto weight = tarjan.run(0);
    cout << weight << endl;
}
