
#include <iostream>
#include <fstream>
#include <random>
#include <cassert>
#include <chrono>
#include <map>

#include <arbok/utils/paths.h>
#include <arbok/lemon/lemon.h>

using namespace std;
using namespace arbok;

struct Timer {
    map<string, chrono::steady_clock::time_point> begin;
    string last;
    void start(const string& desc) { last = desc; begin[desc] = chrono::steady_clock::now(); }
    auto stop(const string& desc) {
        auto end = chrono::steady_clock::now();
        auto dur = chrono::duration_cast<chrono::milliseconds>(end-begin[desc]).count();
        cout << desc << " finished in " << dur << " ms" << endl;
        return dur;
    }
    auto stop() { return stop(last); }
};
Timer t;

struct Graph {
    struct Edge { int from, to, weight; };
    int n = 0;
    vector<Edge> edges;
    bool weighted = false;
    Graph() {}
    Graph(int n_, int m=0, bool weighted_=false) : n(n_), edges(m), weighted(weighted_) { }
};

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

int main() {
    ios::sync_with_stdio(false);

    // auto graph = fromFile(DATA_DIR + "konect/slashdot-zoo.soap"s);
    auto graph = fromFile(DATA_DIR + "konect/flickr-growth.soap"s);

    if(!graph.weighted) {
        mt19937 gen(1337);
        uniform_int_distribution dist(1, 20);
        graph.weighted = true;
        for(auto& [u,v,w] : graph.edges)
            w = dist(gen);
    }

    int root = graph.n;
    int INF = 1e9;
    for(int i=0; i<graph.n; ++i)
        graph.edges.push_back({root,i,INF});
    graph.n++;

    long long res;
    t.start("lemon");
    {
        arbok::Lemon alg(graph.n);
        for (auto[u, v, w]: graph.edges)
            alg.create_edge(u, v, w);
        res = alg.run(root);
    }
    t.stop("lemon");
    cout << res << endl;
    t.start("tarjan");
    {
        arbok::PQTarjan alg(graph.n);
        for (auto[u, v, w]: graph.edges)
            alg.create_edge(u, v, w);
        res = alg.run(root);
    }
    t.stop("tarjan");
    cout << res << endl;

    return 0;
}

