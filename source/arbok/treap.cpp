
#include <arbok/treap.h>

#include <tuple>
#include <cmath>
#include <random>
#include <chrono>

using ll = long long;
using namespace std;

namespace arbok {

static mt19937 rng(1337);

Node::Node(int from, int to, int weight) : y(rng()), x{from, to, weight, weight}, mn{from, to, weight, weight} {}

Edge mn(Node *v) { return v ? v->mn : NO_EDGE; }
void update(Node *v) { v->mn = min({mn(v->l), v->x, mn(v->r)}); }

void apply(Node *v, int lz) {
    if (!v) return;
    v->x.weight += lz;
    v->mn.weight += lz;
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
        auto[vmin, vrest] = split_min(v->l);
        v->l = vrest;
        update(v);
        return {vmin, v};
    } else {
        auto[vmin, vrest] = split_min(v->r);
        v->r = vrest;
        update(v);
        return {vmin, v};
    }
}

} // end namespace arbok