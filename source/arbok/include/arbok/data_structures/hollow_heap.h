#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <exception>
#include <functional>
#include <vector>

namespace otto {
template <class T, class Compare = std::less<T>> class hollow_heap {
  public:
    using value_type = T;
    using value_compare = Compare;
    using size_type = std::size_t;
    using const_reference = const T &;

  protected:
    struct node; // fwd declaration
    class node_handle {
        friend hollow_heap;

      private:
        node_handle(node *x) : _node(x) {}
        node *_node;
    };
    struct node {
        node(value_type _key, uint_fast8_t _rank = 0)
            : key(_key), rank(_rank) {}
        value_type key;
        uint_fast8_t rank;
        node *second_parent = nullptr;
        bool hollow = false;
        std::vector<node *> children;
        int lazy_update = 0;
        bool operator<(const node &other) const {
            return Compare()(key, other.key);
        }
    };
    node *root = nullptr;
    size_type n = 0;
    node *link(node *a, node *b) {
        if (*a < *b) {
            a->children.push_back(b);
            push_update(b, -(a->lazy_update));
            return a;
        } else {
            b->children.push_back(a);
            push_update(a, -(b->lazy_update));
            return b;
        }
    }
    node *ranked_link(node *a, node *b) {
        node *winner = link(a, b);
        ++(winner->rank);
        return winner;
    }
    void destroy(node *a) { delete a; }
    void remove(node *x) {
        x->hollow = true;
        if (x == root) {
            int root_update = x->lazy_update;
            std::array<std::vector<node *>, 255> roots_by_rank;
            std::function<void(node *)> collect_roots = [&](node *u) {
                for (node *c : u->children) {
                    if (c->second_parent != nullptr) {
                        c->second_parent->children.push_back(c);
                        c->second_parent = nullptr;
                    } else if (c->hollow) {
                        collect_roots(c);
                    } else
                        roots_by_rank[c->rank].push_back(c);
                }
                destroy(u);
            };
            collect_roots(root);
            std::vector<node *> final_roots;
            for (int r = 0; r < 255; r++) {
                while (roots_by_rank[r].size() >= 2) {
                    node *a = roots_by_rank[r].back();
                    roots_by_rank[r].pop_back();
                    node *b = roots_by_rank[r].back();
                    roots_by_rank[r].pop_back();
                    roots_by_rank[r + 1].push_back(ranked_link(a, b));
                }
                if (roots_by_rank[r].size() == 1) {
                    final_roots.push_back(roots_by_rank[r].back());
                }
            }
            if (final_roots.size() == 0) {
                root = nullptr;
            } else {
                root = final_roots.back();
                final_roots.pop_back();
                for (node *fr : final_roots)
                    root = link(root, fr);

                // we can only do this because we never have hollow nodes
                push_update(root, root_update);
            }
        }
        --n; // update size of heap
    }
    node *decreaseKey(node *x, const value_type &new_key) {
        if (x == root) {
            x->key = new_key;
            return x;
        } else {
            x->hollow = true;
            node *new_node = new node(new_key, max(0, x->rank - 2));
            root = link(root, new_node);
            if (root != new_node) {
                x->second_parent = new_node;
            }
            return new_node;
        }
    }
    void push_update(node *x, int w) {
        x->key.weight += w;
        x->lazy_update += w;
    }

  public:
    using handle = node_handle;
    hollow_heap() = default;
    hollow_heap(const hollow_heap &) = delete;
    size_type size() const { return n; };
    bool empty() const { return n == 0; }
    const_reference top() const { return root->key; }
    node_handle push(const value_type &x) {
        node *new_node = new node(x);
        if (root == nullptr) {
            root = new_node;
        } else {
            root = link(root, new_node);
        }
        ++n;
        return node_handle(new_node);
    };
    void decreaseKey(node_handle &x, const value_type &new_key) {
        x._node = decreaseKey(x._node, new_key);
    }
    void remove(node_handle &x) { remove(x._node); }
    void pop() { remove(root); }
    void merge(hollow_heap &other) {
        if (other.root == nullptr) {
            return;
        }
        if (root == nullptr) {
            // we take over everything
            root = other.root;
            n = other.n;
        } else {
            if (root->rank == other.root->rank) {
                // we can only do ranked links with nodes of the same rank
                root = ranked_link(root, other.root);
            } else {
                root = link(root, other.root);
            }
            n += other.n;
        }

        // invalidate other heap
        other.root = nullptr;
        other.n = 0;
    }
    void apply_update(int w) {
        // this stuff works iff we never use decrease key here
        if (root == nullptr) {
            return;
        }

        // update edge weight of root node
        push_update(root, w);
    }
};
}; // namespace otto

