
// impl taken from https://github.com/yosupo06/library-checker-problems (jury solution)

#pragma once

#include <vector>
#include <memory>
#include <cassert>

namespace YosupoDS {

using ll = long long;
template <class T> using V = std::vector<T>;
template <class T> using VV = V<V<T>>;

struct E {
    int from, to;
    ll dist;
    int id;
};

struct PairingHeapAllAdd {
    struct Node;
    using NP = std::shared_ptr<Node>;
    struct Node {
        E e;
        ll offset = 0;
        NP head = nullptr, next = nullptr;
        Node(E _e) : e(_e) {}
    };
    NP n = nullptr;
    size_t length = 0;
    PairingHeapAllAdd() {}
    void push(E e) {
        length++;
        n = merge(n, NP(new Node(e)));
    }

    NP merge(NP x, NP y) {
        if (!x) return y;
        if (!y) return x;
        if (x->e.dist + x->offset > y->e.dist + y->offset) swap(x, y);
        y->offset -= x->offset;
        y->next = x->head;
        x->head = y;
        return x;
    }
    E front() { return n->e; }
    void removeFront() {
        assert(n);
        assert(length > 0);
        length--;
        NP x;
        NP s = n->head;
        while (s) {
            NP a, b;
            a = s;
            s = s->next;
            a->next = nullptr;
            a->offset += n->offset;
            if (s) {
                b = s;
                s = s->next;
                b->next = nullptr;
                b->offset += n->offset;
            }
            a = merge(a, b);
            assert(a);
            if (!x)
                x = a;
            else {
                a->next = x->next;
                x->next = a;
            }
        }
        n = nullptr;
        while (x) {
            NP a = x;
            x = x->next;
            n = merge(a, n);
        }
    }
    void meld(PairingHeapAllAdd& r) {
        length += r.length;
        n = merge(n, r.n);
        r.n = nullptr;
    }
    ll& offset() { return n->offset; }
};

}


namespace arbok {

class Yosupo {
private:
    int n;
    int m = 0;
    int c = 1, pc = int(n);
    std::vector<YosupoDS::PairingHeapAllAdd> heap;
    std::vector<YosupoDS::E> res;
    std::vector<int> tr; // reconstruction forest


public:
    Yosupo(int _n, int /*m*/) : n(_n), heap(2*_n), res(2*n), tr(2*_n,-1) {}

    void create_edge(int from, int to, int weight) {
        heap[to].push({from,to,(long long)weight, m++});
    }

    long long run(int root) {
        // union find
        std::vector<int> uf(2 * n, -1);
        auto find = [&](int i) {
            std::vector<int> st;
            while (uf[i] != -1) {
                st.push_back(i);
                i = uf[i];
            }
            for (int j : st) {
                uf[j] = i;
            }
            return i;
        };

        long long ans = 0;
        std::vector<int> used(2 * n);
        used[root] = 1;
        auto mark = [&](int p) {
            c++;
            while (used[p] == 0 || used[p] == c) {
                if (used[p] == c) {
                    // compress
                    int np = pc++;
                    int q = p;
                    do {
                        heap[q].offset() -= res[q].dist + heap[q].offset();
                        heap[np].meld(heap[q]);
                        tr[q] = uf[q] = np;
                        q = find(res[q].from);
                    } while (q != np);
                    p = np;
                }
                assert(used[p] == 0);
                used[p] = c;

                assert(find(p) == p);
                while (heap[p].length && find(heap[p].front().from) == p) {
                    heap[p].removeFront();
                }
                assert(heap[p].length);
                auto mi = heap[p].front();
                res[p] = mi;
                ans += mi.dist + heap[p].offset();
                p = find(mi.from);
            }
        };
        for (int i = 0; i < n; i++) {
            if (used[i]) continue;
            mark(i);
        }

        return ans;
    }

    std::vector<int> reconstruct(int root) {
        std::vector<bool> vis(pc);
        std::vector<int> arb;
        for (int i = pc - 1; i >= 0; i--) {
            if (i == root) continue;
            if (vis[i]) continue;
            int f = res[i].to;
            while (f != -1 && !vis[f]) {
                vis[f] = true;
                f = tr[f];
            }
            arb.push_back(res[i].to);
        }
        return arb;
    }

};

} // end arbok
