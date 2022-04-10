
#include <arbok/data_structures/treap.h>

#include <random>

using ll = long long;
using namespace std;

namespace arbok::treap {

static mt19937 rng(1337);

constexpr pair<int,int> NO_EDGE{1e9+3,-1};

Node::Node(int weight, int idx) : y(rng()), x{weight,idx}, mn{weight,idx} {}

pair<int,int> mn(Node *v) { return v ? v->mn : NO_EDGE; }
void update(Node *v) { v->mn = min({mn(v->l), v->x, mn(v->r)}); }

void apply(Node *v, int lz) {
    if (!v) return;
    v->x.first += lz;
    v->mn.first += lz;
    v->lz += lz;
}

void push(Node *v) {
    apply(v->l, v->lz);
    apply(v->r, v->lz);
    v->lz = 0;
}

Node *merge(Node *l, Node *r) {
    if (!l) return r;
    if (!r) return l;

    if (l->y > r->y) {
        push(l);
        l->r = merge(l->r, r);
        update(l);
        return l;
    } else {
        push(r);
        r->l = merge(l, r->l);
        update(r);
        return r;
    }
}

pair<Node *, Node *> split_min(Node *v) {
    if (!v) return {nullptr, nullptr};
    push(v);

    if (v->x == v->mn) {
        auto rest = merge(v->l, v->r);
        v->l = v->r = nullptr;
        update(v);
        return {v, rest};
    } else if (mn(v->l) == v->mn) {
        auto [vmin, vrest] = split_min(v->l);
        v->l = vrest;
        update(v);
        return {vmin, v};
    } else {
        auto [vmin, vrest] = split_min(v->r);
        v->r = vrest;
        update(v);
        return {vmin, v};
    }
}

} // end namespace arbok