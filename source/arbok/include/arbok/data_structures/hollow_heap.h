#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <exception>
#include <functional>
#include <vector>

// not a real hollow heap, most of the defining features have been stripped
// (i.e. we don't need decrease key)
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
        node() = default;
        node(value_type _key, uint_fast8_t _rank = 0)
            : key(_key), rank(_rank) {}
        value_type key;
        uint_fast8_t rank;
        bool hollow = false;
        node *child = nullptr;
        node *right_sibling = nullptr;
        int lazy_update = 0;
        bool operator<(const node &other) const {
            return Compare()(key, other.key);
        }
    };

  public:
    class allocator {
      public:
        allocator (int m) : node_object_allocation_(m), next_node_(0) {}
        node* new_node(T x) {
            assert(next_node_ < node_object_allocation_.size());
            node_object_allocation_[next_node_].key = x;
            return &node_object_allocation_[next_node_++];
        }
      private:
        std::vector<node> node_object_allocation_;
        int next_node_;
    };

  protected:
    node *root = nullptr;
    size_type n = 0;
    allocator *allocator_;
    void push_child(node *parent, node *new_child) {
        if (parent->child == nullptr) {
            new_child->right_sibling = nullptr;
        } else {
            new_child->right_sibling = parent->child;
        }
        parent->child = new_child;
    }
    node *link(node *a, node *b) {
        if (*a < *b) {
            push_child(a, b);
            push_update(b, -(a->lazy_update));
            return a;
        } else {
            push_child(b, a);
            push_update(a, -(b->lazy_update));
            return b;
        }
    }
    node *ranked_link(node *a, node *b) {
        node *winner = link(a, b);
        ++(winner->rank);
        return winner;
    }
    void destroy(node *a) {
        //delete a;
    }
    void insert_and_rlink(node *c, std::array<node *, 255> &roots_by_rank) {
        if (roots_by_rank[c->rank] == nullptr) {
            roots_by_rank[c->rank] = c;
        } else {
            node *other_node_with_same_rank = roots_by_rank[c->rank];
            roots_by_rank[c->rank] = nullptr;
            insert_and_rlink(ranked_link(c, other_node_with_same_rank),
                             roots_by_rank);
        }
    }
    void ll_backwards_it(node *c, std::array<node *, 255> &roots_by_rank) {
        if (c->right_sibling != nullptr) {
            ll_backwards_it(c->right_sibling, roots_by_rank);
        }
        insert_and_rlink(c, roots_by_rank);
    }
    void extract_root() {
        std::array<node *, 255> roots_by_rank;
        roots_by_rank.fill(nullptr);
        if (root->child != nullptr) {
            ll_backwards_it(root->child, roots_by_rank);
        }
        node *cur = nullptr;
        for (int r = 0; r < 255; r++) {
            if (roots_by_rank[r] != nullptr) {
                if (cur == nullptr) {
                    cur = roots_by_rank[r];
                } else {
                    cur = link(cur, roots_by_rank[r]);
                }
            }
        }

        if (cur != nullptr) {
            push_update(cur, root->lazy_update);
            cur->right_sibling = nullptr;
        }
        destroy(root);
        root = cur;
        --n; // update size of heap
    }
    void push_update(node *x, int w) {
        x->key.weight += w;
        x->lazy_update += w;
    }

  public:
    using handle = node_handle;
    hollow_heap() = delete;
    hollow_heap(hollow_heap&&) = default;
    hollow_heap(allocator* allocator_p) : allocator_{allocator_p} {};
    hollow_heap(const hollow_heap &) = delete;
    size_type size() const { return n; };
    bool empty() const { return n == 0; }
    const_reference top() const { return root->key; }
    node_handle push(const value_type &x) {
        node *new_node = allocator_->new_node(x);
        // node *new_node = new node(x);
        if (root == nullptr) {
            root = new_node;
        } else {
            root = link(root, new_node);
        }
        ++n;
        return node_handle(new_node);
    };
    void pop() { extract_root(); }
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
        if (root == nullptr) {
            return;
        }

        // update edge weight of root node
        push_update(root, w);
    }
};

}; // namespace otto

