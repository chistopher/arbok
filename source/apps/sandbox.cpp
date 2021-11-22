
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

#include <arbok/utils/paths.h>
#include <arbok/tarjan/tarjan.h>
#include <cassert>

using namespace std;
using namespace arbok;

void isArborescence(vector<Edge>& graph, vector<Edge>& arbo, long long claimed_weight, int n, int root) {
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
    edges.reserve(size(graph));
    for(auto e : graph) edges.emplace_back(e.from, e.to, e.weight);
    sort(begin(edges), end(edges));
    for(auto e : arbo)
        assert(binary_search(begin(edges), end(edges), tuple{e.from, e.to, e.orig_weight}));
}

int main() {
    ios::sync_with_stdio(false);
    cout.precision(3);
    cout << fixed;

    //auto testcase = DATA_DIR + "konect/convote.soap"s;
    auto testcase = DATA_DIR + "konect/slashdot-zoo.soap"s;
    ifstream inf(testcase);

    int n,m;
    inf>>n>>m;

    mt19937 gen(1337);
    uniform_int_distribution dist(1, 20);

    // read graph
    auto t1 = chrono::steady_clock::now();
    vector<Edge> graph;
    for(int i=0; i<m; ++i) {
        int from, to;
        inf>>from>>to;
        assert(0<=from && from<n);
        assert(0<=to   && to<n);

        int w;
        if(testcase.ends_with("wsoap")) inf>>w;
        else w = dist(gen);
        graph.push_back({from, to, w, w});
    }
    // make new supernode that is connected to all
    for(int i=0; i<n; ++i)
        graph.push_back({n,i,(int)1e9,(int)1e9});
    int root = n;

    // construct algo DS
    auto t11 = chrono::steady_clock::now();
    Tarjan alg(n+1,TarjanVariant::SET);
    for(auto e : graph) alg.create_edge(e.from, e.to, e.weight);

    auto t2 = chrono::steady_clock::now();
    auto res = alg.run(root);

    auto t3 = chrono::steady_clock::now();
    auto arbo = alg.reconstruct(root);
    auto t4 = chrono::steady_clock::now();
    isArborescence(graph, arbo, res, n+1, root);
    auto t5 = chrono::steady_clock::now();

    cout << "time loading " << chrono::duration_cast<chrono::duration<double>>(t11-t1).count() << endl;
    cout << "time build   " << chrono::duration_cast<chrono::duration<double>>(t2-t11).count() << endl;
    cout << "time algo    " << chrono::duration_cast<chrono::duration<double>>(t3-t2).count() << endl;
    cout << "time reconst " << chrono::duration_cast<chrono::duration<double>>(t4-t3).count() << endl;
    cout << "time valid   " << chrono::duration_cast<chrono::duration<double>>(t5-t4).count() << endl;

    cout << endl;
    cout << "n      =" << n << endl;
    cout << "m      =" << m << endl;
    cout << "w      =" << res << endl;
    cout << "w/1e9  =" << res/1'000'000'000 << endl;

    return 0;
}

