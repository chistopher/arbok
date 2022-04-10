
// impl taken from https://judge.yosupo.jp/submission/61001
// also in https://github.com/Felerius/cp-lib (private repo)

#pragma once

#include <vector>
#include <utility>
#include <cassert>
#include <iostream>

namespace FeleriusDS {

struct Dsu {
    std::vector<int> p;

    explicit Dsu(int n) : p(n, -1) {}

    int find(int i) { return p[i] < 0 ? i : p[i] = find(p[i]); }
    bool same(int i, int j) { return find(i) == find(j); }
    int size(int i) { return -p[find(i)]; }
    bool join(int i, int j) {
        i = find(i), j = find(j);
        if (i == j) return false;
        if (p[i] > p[j]) std::swap(i, j);
        p[i] += p[j], p[j] = i;
        return true;
    }
};

struct RollbackDsu {
    std::vector<int> p;
    std::vector<std::pair<int, int>> joins;

    explicit RollbackDsu(int n) : p(n, -1) {}

    int find(int i) const { return p[i] < 0 ? i : find(p[i]); }
    bool same(int i, int j) const { return find(i) == find(j); }
    int size(int i) const { return -p[find(i)]; }
    int time() const { return int(std::size(joins)); }

    bool join(int i, int j) {
        i = find(i), j = find(j);
        if (i == j) return false;
        if (p[i] > p[j]) std::swap(i, j);
        joins.emplace_back(j, p[j]);
        p[i] += p[j], p[j] = i;
        return true;
    }

    void rollback(int t) {
        while (std::size(joins) > t) {
            auto [i, pi] = joins.back(); joins.pop_back();
            assert(p[p[i]] < 0);
            p[p[i]] -= pi;
            p[i] = pi;
        }
    }
};

} // end namespace FeleriusDS


namespace arbok {

class Felerius {
private:
    struct SkewHeapNode { int l, r, from, to, weight, lz; };
    std::vector<SkewHeapNode> nodes; // one skew heap node per edge of the input
    std::vector<int> heap;

    void apply(int i, int upd) { nodes[i].weight -= upd; nodes[i].lz += upd; }
    void push(int i) {
        if (nodes[i].l != -1) apply(nodes[i].l, nodes[i].lz);
        if (nodes[i].r != -1) apply(nodes[i].r, nodes[i].lz);
        nodes[i].lz = 0;
    }
    int merge(int u, int v) {
        if (u == -1 || v == -1) return (u == -1 ? v : u);
        if (nodes[v].weight < nodes[u].weight) std::swap(u, v);
        push(u);
        nodes[u].r = merge(nodes[u].r, v);
        std::swap(nodes[u].l, nodes[u].r);
        return u;
    }
    void pop(int v) {
        push(heap[v]);
        heap[v] = merge(nodes[heap[v]].l, nodes[heap[v]].r);
    }

public:
    std::vector<std::pair<int, int>> cycles; // data to reconstruct solution from
    FeleriusDS::RollbackDsu dsu_contract;
    std::vector<int> edge; // stores index of edge = index of corresponding heap node

    Felerius(int n, int /*m*/) : heap(n,-1), dsu_contract(n), edge(n,-1) {}

    void create_edge(int from, int to, int weight) {
        assert(0 <= from && from < size(heap) && 0 <= to && to < size(heap));
        nodes.push_back(SkewHeapNode{-1, -1, from, to, weight, 0});
        heap[to] = merge(heap[to], int(size(nodes)) - 1);
    }

    long long run(int root) {
        assert(0 <= root && root < size(heap));
        long long ans = 0;
        FeleriusDS::Dsu dsu_cyc(int(size(heap)));
        for(int i=0; i<size(heap); ++i) {
            if (i == root) continue;
            int v = i;
            while (true) {
                if (heap[v] == -1) std::cout << "ERROR" << std::endl, exit(1);
                edge[v] = heap[v];
                ans += nodes[edge[v]].weight;
                apply(edge[v], nodes[edge[v]].weight);
                if (dsu_cyc.join(v, dsu_contract.find(nodes[edge[v]].from)))
                    break;

                int vnext = dsu_contract.find(nodes[edge[v]].from), t = dsu_contract.time();
                while (dsu_contract.join(v, vnext)) {
                    heap[dsu_contract.find(v)] = merge(heap[v], heap[vnext]);
                    v = dsu_contract.find(v);
                    vnext = dsu_contract.find(nodes[edge[vnext]].from);
                }
                cycles.emplace_back(edge[v], t);

                while (heap[v] != -1 && dsu_contract.same(nodes[heap[v]].from, v))
                    pop(v);
            }
        }
        return ans;
    }

    std::vector<int> reconstruct(int /* root */) {
        for (auto it = rbegin(cycles); it != rend(cycles); ++it) {
            int vrepr = dsu_contract.find(nodes[it->first].to);
            dsu_contract.rollback(it->second);
            int vinc = dsu_contract.find(nodes[edge[vrepr]].to);
            edge[vinc] = std::exchange(edge[vrepr], it->first);
        }
        edge.erase(find(begin(edge),end(edge),-1));
        return edge;
    }

};

} // end arbok
