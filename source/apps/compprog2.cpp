
#include <vector>
#include <random>
#include <iostream>
#include <queue>
#include <map>
#include <cassert>

#include <arbok/data_structures/treap.h>
#include <arbok/data_structures/dsu.h>

using namespace std;
using namespace arbok;
using namespace arbok::treap;

#define sz(x)       ((int)::size(x))

#define rep(a, b)      for(int a = 0; a < (b); ++a)
#define reps(a, b, c)  for(int a = (b); a < (c); ++a)

using ll = long long;

const int INF = (int)1e9 + 5;

// this is a tarjan implementation with treaps and answer reconstruction with persistent DSUs

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n, m; cin >> n >> m;
    vector<Node*> inc(n, nullptr);
    map<pair<int,int>, ll> ecost;
    rep(_, m) {
        int a, b; ll w; cin >> a >> b >> w; --a, --b;
        if (a != b)
            inc[b] = merge(inc[b], new Node(static_cast<int>(w), a, b));
        auto it = ecost.emplace(pair(a, b), w).first;
        it->second = min(w, it->second);
    }

    int r = 0;
    DSU dsu1(n);
    PersistentDSU dsu2(n);
    queue<int> q;
    rep(v, n)
        if (v != r)
            q.emplace(v);
    vector f(n, pair(-1, -1));
    ll ans = 0;
    vector<pair<int,int>> cycles;

    while (sz(q)) {
        int v = q.front(); q.pop();

        auto [vmin, vrest] = split_min(inc[v]); 
        if (!vmin)
            cout << "NO\n", exit(0);

        ll cost = vmin->x.weight;
        f[v] = {vmin->x.from, vmin->x.to};
        inc[v] = merge(vmin, vrest);
        ans += cost;
        apply(inc[v], static_cast<int>(-cost));

        if (dsu1.join(v, dsu2.find(f[v].first)))
            continue;

        int vb = v, t = sz(cycles);
        while (1) {
            int vn = dsu2.find(f[vb].first);
            if (vn == dsu2.find(v)) break;
            dsu2.join(vb, vn, t);
            inc[v] = merge(inc[v], inc[vn]);
            vb = vn;
        }
        int vj = dsu2.find(v);
        swap(inc[v], inc[vj]);
        cycles.emplace_back(f[vj]);

        while (1) {
            auto [vmin, vrest] = split_min(inc[vj]);
            if (!vmin || !dsu2.same(vj, vmin->x.from)) {
                inc[vj] = merge(vmin, vrest);
                break;
            }
            inc[vj] = vrest;
        }

        f[vj] = {-1, -1};
        q.emplace(vj);
    }

    for (int i = sz(cycles) - 1; i >= 0; --i) {
        auto [old_from, old_to] = cycles[i];
        int vj = dsu2.find(old_to, i + 1);
        auto [from, to] = f[vj];
        int vin = dsu2.find(to, i);
        f[vj] = {old_from, old_to};
        f[vin] = {from, to};
    }

    ll decomp_ans = 0;
    reps(i, 1, n)
        decomp_ans += ecost[f[i]];
    assert(ans == decomp_ans);

    cout << "YES\n" << ans << '\n';
    return 0;
}
