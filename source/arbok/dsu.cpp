
#include <arbok/dsu.h>

#include <utility>

using namespace std;
using namespace arbok;

const int INF = 1e9;

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

PersistentDSU::PersistentDSU(int n) : p(n, pair(-1, -INF)) {}
int PersistentDSU::find(int i, int t) {
    while (p[i].first >= 0 && p[i].second < t)
        i = p[i].first;
    return i;
}
bool PersistentDSU::same(int i, int j, int t) { return find(i, t) == find(j, t); }
int PersistentDSU::size(int i, int t) { return -p[find(i, t)].first; }
bool PersistentDSU::join(int i, int j, int t) {
    i = find(i), j = find(j);
    if (i == j) return false;
    if (p[j].first < p[i].first) swap(i, j);
    p[i].first += p[j].first, p[j] = {i, t};
    return true;
}
