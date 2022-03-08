
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <queue>
#include <map>

#include <arbok/utils/paths.h>
#include <arbok/tarjan/tarjan.h>
#include <arbok/gabow/gabow.h>
#include <cassert>

using namespace std;

struct ScopedTimer {
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    string m_desc;
    auto stop() {
        auto end = chrono::steady_clock::now();
        auto dur = chrono::duration_cast<chrono::milliseconds>(end-start).count();
        cout << m_desc << " finished in " << dur << " ms" << endl;
        m_desc.clear();
        return dur;
    }
    ScopedTimer(const string& desc) : m_desc(desc) {}
    ScopedTimer& operator=(ScopedTimer&& other) {
        swap(start,other.start);
        swap(m_desc,other.m_desc);
        return *this;
    }
    ~ScopedTimer() { if(!empty(m_desc)) stop(); }
};

struct Graph {
    struct Edge { int from, to, weight; };
    int n = 0;
    vector<Edge> edges;
    bool weighted = false;
    Graph() {}
    Graph(int n_, int m=0, bool weighted_=false) : n(n_), edges(m), weighted(weighted_) { }
};

Graph giantCC(const Graph& graph) {
    int n = graph.n;

    // make adj list
    vector<vector<int>> adj(n);
    for(auto [u,v,w] : graph.edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // compute ccs
    vector<int> cc(n,-1);
    queue<int> q;
    int num_ccs = 0;
    for(int i=0; i<n; ++i) {
        if(cc[i]!=-1) continue;
        cc[i] = num_ccs++;
        q.push(i);
        while(size(q)) {
            auto v = q.front(); q.pop();
            for(auto u : adj[v])
                if(cc[u]==-1)
                    cc[u] = cc[v], q.push(u);
        }
    }

    // count cc sizes
    vector<int> local_idx(n);
    vector<int> cc_size(num_ccs,0);
    for(int i=0; i<n; ++i)
        local_idx[i] = cc_size[cc[i]]++;

    // construct graph for giant
    auto it = max_element(begin(cc_size), end(cc_size));
    int giant_idx = int(it - begin(cc_size));
    Graph giant;
    giant.n = cc_size[giant_idx];
    giant.weighted = graph.weighted;
    for(auto [u,v,w] : graph.edges) {
        if(cc[u]!=giant_idx) continue;
        assert(cc[u] == giant_idx);
        assert(cc[v] == giant_idx);
        giant.edges.push_back({local_idx[u], local_idx[v], w});
    }

    return giant;
}

Graph fromFile(const string& file) {
    ifstream inf(file);
    if(!inf) cout << "failed to load " << file << endl, exit(1);
    bool weighted = file.ends_with("wsoap");
    int n,m;
    inf>>n>>m;
    Graph graph(n,m,weighted);
    for(auto& [from,to,w] : graph.edges) {
        inf>>from>>to;
        assert(0<=from && from<n);
        assert(0<=to   && to<n);
        if(weighted) inf>>w;
    }
    return graph;
}


void isArborescence(const Graph& graph, vector<arbok::Edge>& arbo, long long claimed_weight, int n, int root) {
    // weight is same as claimed
    auto check_sum = 0ll;
    for(auto e : arbo) check_sum += e.orig_weight;
    assert(check_sum==claimed_weight);

    // each node except root has one incoming edge
    assert(size(arbo)==n-1);
    vector has_inc(n, false);
    for(auto e : arbo) {
        assert(!has_inc[e.to]);
        has_inc[e.to] = true;
    }
    assert(!has_inc[root]);

    // all arbo edges exists like this in the original graph
    vector<tuple<int,int,int>> edges;
    edges.reserve(size(graph.edges));
    for(auto e : graph.edges) edges.emplace_back(e.from, e.to, e.weight);
    sort(begin(edges), end(edges));
    for(auto e : arbo)
        assert(binary_search(begin(edges), end(edges), tuple{e.from, e.to, e.orig_weight}));
}

map<string, string> parseArgs(int argc, char** argv) {
    map<string, string> params;
    for (int i = 1; i < argc; i++) {
        // Get current and next argument
        if (argv[i][0] != '-')
            continue;
        std::string arg = argv[i] + 1; // +1 to skip the -
        // advance one additional position if next is used
        std::string next = (i + 1 < argc ? argv[i++ + 1] : "");
        params[std::move(arg)] = std::move(next);
    }
    return params;
}

template<class Algo>
void run(map<string, string>& args) {

    auto input  = DATA_DIR + args["input"];

    ScopedTimer timer("load graph");
    auto graph = fromFile(input);
    timer.stop();
    cout << "n      =" << graph.n << endl;
    cout << "m      =" << size(graph.edges) << endl;

    timer = ScopedTimer("find giant cc");
    graph = giantCC(graph);
    timer.stop();

    cout << "n      =" << graph.n << endl;
    cout << "m      =" << size(graph.edges) << endl;

    if(!graph.weighted) {
        ScopedTimer scoped("add random weights");
        mt19937 gen(1337);
        uniform_int_distribution dist(1, 20);
        //graph.weighted = true;
        for(auto& [u,v,w] : graph.edges)
            w = dist(gen);
    } else {
        int mn = numeric_limits<int>::max();
        int mx = numeric_limits<int>::min();
        for(auto [u,v,w] : graph.edges) {
            mn = min(mn, w);
            mx = max(mx, w);
        }
        cout << "found weights in range [" << mn << ',' << mx << "]\n";
        if(mn<0) {
            cout << "\tcorrecting minimum" << endl;
            for(auto& [u,v,w] : graph.edges) w += mn;
        }
    }

    const int INF = 1e9;

    timer = ScopedTimer("add supernode");
    int root = graph.n;
    for(int i=0; i<graph.n; ++i)
        graph.edges.push_back({root,i,INF});
    graph.n++;

    // construct algo DS
    timer = ScopedTimer("construct algo DS");
    Algo alg(graph.n);
    for(auto e : graph.edges) alg.create_edge(e.from, e.to, e.weight);

    timer = ScopedTimer("run algo");
    auto res = alg.run(root);
    auto dur = timer.stop();

    timer = ScopedTimer("reconstruct");
    auto arbo = alg.reconstruct(root);

    timer = ScopedTimer("validate");
    isArborescence(graph, arbo, res, graph.n, root);
    timer.stop();

    if(!empty(args["csv"])) {
        ofstream ouf(args["csv"], ios::app);
        ouf << input
            << ',' << graph.n
            << ',' << size(graph.edges)
            << ',' << graph.weighted
            << ',' << args["algo"]
            << ',' << dur << endl;
    }

    cout << "n      =" << graph.n << endl;
    cout << "m      =" << size(graph.edges) << endl;
    cout << "merge  =" << alg.contractions << endl;
    cout << "w      =" << res << endl;
    cout << "w%1e9  =" << res%int(1e9) << endl;
    cout << "w/1e9  =" << res/1'000'000'000 << endl;

}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);

    map<string,string> defaults{
            {"input",   "konect/slashdot-zoo.soap"},
            {"csv",     ""},
            {"algo",    "pq"}
    };
    auto args = parseArgs(argc, argv);
    cout << "PARAMETER: " << endl;
    for(auto [key,val] : defaults) {
        if(args.contains(key)) continue;
        cout << key << " not given, default assumed (" << val << ")" << endl;
        args[key] = val;
    }
    for(auto [key,val] : args) {
        if(!defaults.contains(key)) cerr << "ERROR: unrecognized param: " << key, exit(1);
        cout << '\t' << key << ": " << val << endl;
    }

    auto algo = args["algo"];
    if(algo=="set") run<arbok::SetTarjan>(args);
    else if(algo=="matrix") run<arbok::MatrixTarjan>(args);
    else if(algo=="pq") run<arbok::PQTarjan>(args);
    else if(algo=="treap") run<arbok::TreapTarjan>(args);
    else if(algo=="gabow") run<arbok::Gabow>(args);
    else cerr << "invalid algo: " << algo, exit(1);

    return 0;
}

