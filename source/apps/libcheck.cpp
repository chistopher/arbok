// https://judge.yosupo.jp/problem/directedmst
#pragma GCC optimize("fast-math")
// begin "cp-lib/prelude.hpp"
#include <bits/stdc++.h>
#ifdef LOCAL
#  include <dbg.h>
#else
#  define dbg(...) do {} while (0)
#endif

#define cp_lib_4th(_1, _2, _3, x, ...)  x
#define cp_lib_rep(i, l, r)             for (int i = (l); (i) < (r); ++(i))
#define cp_lib_rep0(i, r)               cp_lib_rep(i, 0, r)
#define rep(...)                        cp_lib_4th(__VA_ARGS__, cp_lib_rep, cp_lib_rep0, _)(__VA_ARGS__)
#define cp_lib_repr(i, r, l, ...)       for (int i = (r); (i) >= (l); --(i))
#define repr(...)                       cp_lib_repr(__VA_ARGS__, 0)
#define all(a)                          ::begin(a),::end(a)
#define trav(a, b)                      for (auto&& a : (b))

using namespace std;
using ll = long long;
using ld = long double;
[[maybe_unused]] static constexpr int INF = int(1e9 + 5);
[[maybe_unused]] static constexpr ll INFL = ll(INF) * INF;
template <class C> int sz(const C& c) { return int(::size(c)); }
// end "cp-lib/prelude.hpp"
// begin "cp-lib/io.hpp"
#include <unistd.h>

namespace cp_lib_io {
    constexpr int BUF_SIZE = 1 << 20;
    array<char, BUF_SIZE> ibuf, obuf;
    char *iptr = data(ibuf), *iend = iptr, *optr = data(obuf);

    template <class T, class = void> struct is_tuple_like : false_type {};
    template <class T> struct is_tuple_like<T, void_t<tuple_element_t<0, T>>> : true_type {};
    template <class T> struct is_std_array : false_type {};
    template <class T, size_t I> struct is_std_array<array<T, I>> : true_type {};

    void flush() { write(STDOUT_FILENO, begin(obuf), optr - begin(obuf)); optr = begin(obuf); }
    void ensure_write(int l) { if (end(obuf) - optr < l) flush(); }
    int _flush_atexit = []{ atexit(flush); return 0; }();
    void refill() {
        if (iend == end(ibuf))
            memmove(begin(ibuf), iptr, iend - iptr), iend += begin(ibuf) - iptr, iptr = begin(ibuf);
        iend += read(STDIN_FILENO, iend, end(ibuf) - iend);
    }
    void skip_ensure_read(int l) {
        do {
            while (iptr != iend && *iptr <= ' ') ++iptr;
            if (iend - iptr < l) refill();
        } while (*iptr <= ' ');
    }

    template <class T, class T2 = remove_reference_t<remove_cv_t<T>>>
    void print(T&& val) {
        if constexpr (is_same_v<T2, char>)
            ensure_write(2), *optr++ = val;
        else if constexpr (is_integral_v<T2> && !is_same_v<T2, bool>) {
            ensure_write(numeric_limits<T>::digits10 + 1 + is_signed_v<T2>);
            if (val < 0) { *optr++ = '-'; print(-make_unsigned_t<T2>(val)); return; }
            array<char, 128> tmp;
            char* tptr = end(tmp);
            do { *--tptr = char('0' + val % 10), val /= 10; } while (val);
            memcpy(optr, tptr, end(tmp) - tptr);
            optr += end(tmp) - tptr;
#if __cpp_lib_to_chars >= 201611
            } else if constexpr (is_floating_point_v<T2>) {
            ensure_write(64), optr = to_chars(optr, end(obuf), val, chars_format::fixed, 20).ptr;
#endif
        } else if constexpr (is_convertible_v<T, string_view>) {
            string_view sv(val);
            if (sz(sv) + 1 <= end(obuf) - optr)
                memcpy(optr, data(sv), sz(sv)), optr += sz(sv);
            else
                flush(), write(STDOUT_FILENO, data(sv), sz(sv));
        } else {
            if constexpr (is_same_v<T2, bool> || is_same_v<T2, vector<bool>::reference>)
                print(int(val));
            else if constexpr (is_tuple_like<T2>() && !is_std_array<T2>())
                apply([](auto&&... items) { (print(items), ...); }, forward<T>(val));
            else
                trav(item, val) print(item);
            return;
        }
        *optr++ = ' ';
    }

    template <class T>
    void read(T& val) {
        if constexpr (is_same_v<T, char>)
            skip_ensure_read(1), val = *iptr++;
        else if constexpr (is_same_v<T, bool>) {
            uint8_t ival; read(ival), val = bool(ival);
        } else if constexpr (is_integral_v<T>) {
            skip_ensure_read(numeric_limits<T>::digits10 + 1 + is_signed_v<T>);
            if (is_signed_v<T> && *iptr == '-') {
                ++iptr;
                make_unsigned_t<T> uval;
                read(uval);
                val = T(-uval);
            } else {
                val = 0;
                while (*iptr > ' ') val = 10 * val + (*iptr++ - '0');
            }
#if __cpp_lib_to_chars >= 201611
            } else if constexpr (is_floating_point_v<T>) {
            skip_ensure_read(128), iptr = from_chars(iptr, iend, val).ptr;
#endif
        } else if constexpr (is_same_v<T, string>) {
            skip_ensure_read(1);
            while (true) {
                auto* after = iptr;
                while (*after > ' ') ++after;
                copy(iptr, after, back_inserter(val));
                if ((iptr = after) == iend)
                    refill();
                else
                    break;
            }
        } else if constexpr (is_tuple_like<T>() && !is_std_array<T>())
            apply([](auto&... items) { (read(items), ...); }, val);
        else
            trav(item, val) read(item);
    }
}

template <class... Args>
void print(Args&&... args) { (cp_lib_io::print(forward<Args>(args)), ...); }

template <class... Args>
void println(Args&&... args) {
    (cp_lib_io::print(forward<Args>(args)), ...);
    *(cp_lib_io::optr - 1) = '\n';
}

template <class... Args>
void printlns(Args&&... args) {
    ((cp_lib_io::print(forward<Args>(args)), *(cp_lib_io::optr - 1) = '\n'), ...);
}

template <class... Args>
void read(Args&... args) { (cp_lib_io::read(args), ...); }
// end "cp-lib/io.hpp"
// begin "cp-lib/graph/min-cost-arborescence.hpp"
// begin "../_assert.hpp"
#ifdef CP_LIB_DEBUG
#define cp_lib_assert(expr) \
    do { if (!(expr)) { \
        ::cerr << "assertion failed: " << #expr << " (" << __FILE__ << ':' << __LINE__ << ")\n"; \
        ::abort(); \
    } } while (0)
#else
#define cp_lib_assert(expr)
#endif
// end "../_assert.hpp"

struct DSU {
    std::vector<int> p;
    explicit DSU(int n) : p(n, -1) {}
    int find(int i) { return p[i] < 0 ? i : p[i] = find(p[i]); }
    bool same(int i, int j) { return find(i) == find(j); }
    int size(int i) { return -p[find(i)]; }
    bool join(int i, int j) {
        i = find(i), j = find(j);
        if (i == j) return false;
        if (p[i] > p[j]) swap(i, j);
        p[i] += p[j], p[j] = i;
        return true;
    }
};

struct Edge {
    int from, to, weight;
    inline bool operator<(const Edge& rhs) const { return weight > rhs.weight; }
};

class Tarjan {
    const int num_vertices;
    DSU cy; // for weakly connected components
    DSU co; // for actual merges

    // last time a representative appeared in the queue
    // this is the pos in inc where the most recent incoming edge is stored for this node
    std::vector<unsigned> queue_id;
    // edges that the algorithm picked in each iteration of the queue
    std::vector<Edge> inc;
    // forest over the edges in inc that stores parent indices
    std::vector<unsigned> forest;

    // handle incoming edge sets
    std::vector<vector<Edge>> managedSets;
    std::vector<int> offsets;
    Edge get_min_edge(int v) {
        assert(size(managedSets[v]));
        auto it = end(managedSets[v]);
        while(co.find(managedSets[v][0].from) == v)
            pop_heap(begin(managedSets[v]),it), --it; // delete selfloops
        auto res = managedSets[v][0];
        pop_heap(begin(managedSets[v]),it); --it; // extract the edge that is returned
        managedSets[v].erase(it,end(managedSets[v]));
        res.weight -= offsets[v];
        return res;
    }
    void update_incoming_edge_weights(int v, int w) {
        offsets[v] += w;
    }
    void move_edges(int from, int to) {
        // make sure set of from is smaller than to
        auto& small = managedSets[from];
        auto& large = managedSets[to];
        if(size(small)>size(large)) {
            swap(small, large);
            swap(offsets[from], offsets[to]);
        }

        // smaller into larger while applying offset
        for(auto& e : small) {
            e.weight -= offsets[from] - offsets[to];
            large.push_back(e);
            push_heap(all(large));
        }
        small.clear();
    }

public:
    Tarjan(int n) : num_vertices(n) , cy(n) , co(n) , queue_id(n), managedSets(n), offsets(n,0) {
        inc.reserve(2*n);
        forest.reserve(2*n);
    }

    void create_edge(int from, int to, int weight) {
        managedSets[to].push_back({from, to, weight});
    };
    long long run(int root) {
        for(auto& heap : managedSets)
            make_heap(all(heap));

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

            const auto min_edge = get_min_edge(v);
            assert(co.find(min_edge.from) != v); // no self-loops allowed here

            inc.push_back(min_edge); // inc edges are stored at queue_id[node] because same node can be rep multiple times
            forest.push_back(i); // at first each edge is a root in forest

            answer += min_edge.weight;
            update_incoming_edge_weights(v, min_edge.weight);

            if (cy.join(min_edge.from, min_edge.to))
                continue;

            // we built a cycle. now merge incoming edges of cycle into one node
            int merged = v;
            auto next_in_cyc = [&](int last) { return co.find(inc[queue_id[last]].from); };
            for (int cur = next_in_cyc(v); cur != merged; cur = next_in_cyc(cur)) {
                int from = cur, to = merged;
                co.join(cur, merged);
                if (co.find(merged)==cur) swap(from,to);
                move_edges(from, to);
                merged = to;

                forest[queue_id[cur]] = static_cast<unsigned>(size(q)); // size of queue is where the merged supernode will be in the queue
            }
            forest[i] = static_cast<unsigned>(size(q)); // edge to v is also part of cycle

            // push contracted node
            q.push_back(merged);
        }

        return answer;
    }

    std::vector<int> reconstruct(int arbo_root) {
        assert(size(inc));
        assert(size(inc) == size(forest));
        auto n = static_cast<int>(size(inc));

        vector<int> res(num_vertices,-1);
        vector del(n,false);
        for(int r=n-1; r>=0; --r) { // we exploit here that parent has always higher index; -> larger index first results in top-to-bottom traversal
            if(del[r]) continue;
            assert(forest[r]==r || del[forest[r]]); // we have a root
            res[inc[r].to] = inc[r].from;
            int leaf = inc[r].to;
            assert(leaf != arbo_root);
            auto leaf_edge_pos = leaf - (arbo_root < leaf); // if arbo-root<leaf then the first incoming edge of leaf is at leaf-1
            // we take the edge at position r and 'delete' the path from the leaf to the root from the forest
            del[r] = true;
            for(int i=leaf_edge_pos; i!=r; i=forest[i])
                del[i] = true;
        }
        return res;
    }

};


int main() {
    int n, m, vr;
    read(n, m, vr);
    //cin>>n>>m>>vr;
    Tarjan alg(n);
    rep(_, m) {
        int u, v, w;
        read(u, v, w);
        //cin>>u>>v>>w;
        alg.create_edge(u, v, w);
    }

    auto cost = alg.run(vr);
    auto edges = alg.reconstruct(vr);

    println(cost);
    rep(i, n) print(i == vr ? vr : edges[i]);
    println();
}