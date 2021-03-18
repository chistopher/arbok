
#include <vector>
#include <random>
#include <iostream>
#include <queue>

#include <arbok/treap.h>
#include <arbok/dsu.h>

using namespace std;
using namespace arbok;

using ll = long long;

static constexpr int INF = (int)1e9 + 5;

// this is a tarjan implementation with treaps and w/o answer reconstruction

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n, m; cin >> n >> m;
    vector<Node*> inc(n, nullptr);
    for(int i=0; i<m; ++i) {
        int a, b; ll w; cin >> a >> b >> w; --a, --b;
        if (a != b)
            inc[b] = merge(inc[b], new Node(w, a, b));
    }

    int r = 0;
    DSU dsu1(n), dsu2(n);
    queue<int> q;
    for(int v=0; v<n; ++v)
        if (v != r)
            q.emplace(v);
    vector<int> f(n, -1);
    ll ans = 0;

    while (size(q)) {
        int v = q.front(); q.pop();

        auto [vmin, vrest] = split_min(inc[v]); 
        if (!vmin)
            cout << "NO\n", exit(0);

        ll cost = vmin->x.weight;
        f[v] = dsu2.find(vmin->x.from);
        inc[v] = merge(vmin, vrest);
        ans += cost;
        apply(inc[v], -cost);

        if (dsu1.join(v, f[v]))
            continue;

        vector<int> circ = {v};
        while (1) {
            int next = dsu2.find(f[circ.back()]);
            if (next == dsu2.find(v)) break;
            dsu2.join(circ.back(), next);
            circ.push_back(next);
        }
        Node* merged = nullptr;
        for(auto& vi : circ)
            merged = merge(merged, inc[vi]), inc[vi] = nullptr;

        int vj = dsu2.find(v);
        inc[vj] = merged;

        while (1) { // TODO: do this when extracting to prevent shadow
            auto [vmin, vrest] = split_min(inc[vj]);
            if (!vmin || !dsu2.same(vj, vmin->x.from)) {
                inc[vj] = merge(vmin, vrest);
                break;
            }
            inc[vj] = vrest;
        }

        f[vj] = -1;
        q.emplace(vj);
    }

    cout << "YES\n" << ans << '\n';
    return 0;
}
