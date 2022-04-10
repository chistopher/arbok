#include <arbok/utils/utils.h>

#include <iostream>
#include <tuple>
#include <queue>
#include <cassert>
#include <fstream>

using namespace std;

namespace arbok {

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

bool isArborescence(const Graph& graph, const vector<int>& arbo, long long claimed_weight, int root) {
    if(size(arbo)!=graph.n-1)
        return false;
    auto check_sum = 0ll;
    vector has_inc((size_t)graph.n, false);
    for(auto e_idx : arbo) {
        if(e_idx<0 || e_idx>=size(graph.edges))
            return false;
        auto& e = graph.edges[e_idx];
        check_sum += e.weight;
        if(has_inc[e.to])
            return false; // nodes has multiple incoming edges
        has_inc[e.to] = true;
    }
    // weight is same as claimed
    if(check_sum!=claimed_weight)
        return false;
    if(has_inc[root])
        return false;
    // each node has one incoming follows from size(arbo)==n-1, root has no inc, no node has more than one inc
    return true;
}

}