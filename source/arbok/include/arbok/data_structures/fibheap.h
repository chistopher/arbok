#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

namespace arbok::fheap {

template <class T, class Compare = std::less<T> >
class fibonacci_heap {
public:
    using value_type = T;
    using value_compare = Compare;
protected:
    using home_heap_ptr_type = fibonacci_heap<T,Compare>*;
    class home_heap_dsf_node {
        friend fibonacci_heap;
    private:
        home_heap_dsf_node *next = nullptr; // endogeneous linked list
        home_heap_dsf_node *parent = nullptr;
        home_heap_ptr_type home_heap;
        home_heap_dsf_node *find() {
            if (parent == nullptr) return this;
            return parent = parent->find();
        }
        home_heap_dsf_node() = delete;
        explicit home_heap_dsf_node(const home_heap_ptr_type _home_heap) : home_heap(_home_heap) {}
    public:
        home_heap_ptr_type get_home_heap() {
            return find()->home_heap;
        }
    };
    class node {
        friend fibonacci_heap;
    private:
        // constructor
        explicit node(
            value_type _key,
            node *_left,
            node *_right,
            node *_parent,
            node *_child,
            bool _is_loser,
            uint8_t _order,
            home_heap_dsf_node *_home_wrapper
        ) : key(_key), left(_left), right(_right), parent(_parent), child(_child), is_loser(_is_loser), order(_order), home_wrapper(_home_wrapper) { }

        // data
        value_type key;
        node *left;
        node *right;
        node *parent;
        node *child;
        bool is_loser;
        uint8_t order;
        home_heap_dsf_node *home_wrapper;

        // methods
        void self_hug() {
            left = this;
            right = this;
        }
        void merge_brother(node *new_sibling) {
            if (parent != nullptr) parent->order--;
            parent = new_sibling->parent;
            if (parent != nullptr) parent->order++;
            right = new_sibling->right;
            left = new_sibling;
            right->left = this;
            new_sibling->right = this;
        }
        void lose_child(node *which, node *rt) {
            if (which == child) {
                if (child == child->left) child = nullptr; // no child left
                else child = which->left; // replace child
            }
            if (parent == nullptr) return; // if we are in root we remain there
            if (!is_loser) is_loser = true;
            else {
                throw_in_root(rt);
            }
        }
        void adopt_by(node *new_parent) {
            if (parent != nullptr) parent->order--;
            parent = new_parent;
            parent->child = this;
            if (parent) parent->order++;
            left = this;
            right = this;
        }
        void throw_in_root(node *rt) {
            // tuck left and right sibling together
            if (left != nullptr) left->right = right;
            if (right != nullptr) right->left = left;

            // tell parent
            if (parent != nullptr) parent->lose_child(this, rt);

            // go to root
            merge_brother(rt);

            is_loser = false;
        }
        void merge_over(node *other) {
            // we should be in the root list
            assert(parent == nullptr);
            // other should be in the root list
            assert(other->parent == nullptr);

            // remove other from root list
            other->left->right = other->right;
            other->right->left = other->left;

            order++;

            if (child != nullptr) { // if we already have a child
                other->merge_brother(child);
            } else { // no child
                other->adopt_by(this);
            }
        }
        void clear_parent(node *start_brother) {
            // this clears entire sibling list of parents
            parent = nullptr; // forget own parent
            if (right != start_brother) // end if we recursed through all brothers
                right->clear_parent(start_brother); // recurse to right brother to do the same
        }
        node *pop() { // remove ourself (node) from the root list, returns new root

            assert(parent == nullptr); // we should be in the root list

            if (child != nullptr) {
                // clear parent pointers of children
                child->clear_parent(child);
                // put child right of us

                // connect righthand sibling with righthand end of children
                child->left->right = right;
                right->left = child->left;

                // connect us with child on the right
                child->left = this;
                right = child;
            }

            if (left == this) // no new node
                return nullptr; // no root remaining, thus "empty"

            // remove ourselves
            left->right = right;
            right->left = left;

            return left; // return left sibling as new root
        }
        void unsafe_setkey(const value_type& new_key) {
            key = new_key;
        }
        void decrease_key(const value_type& new_key, node *rt) {
            if (!Compare()(new_key, key)) {
                // Gabow does weird things
                unsafe_setkey(new_key);
                return;
            }

            assert(Compare()(new_key, key)); // new key should be smaller than current key
            key = new_key; // this is a copy, i hope this is okay

            // if we have a parent and we have a smaller key than it -> heap
            // property violation
            if (parent != nullptr && Compare()(key, parent->key))
                throw_in_root(rt);
        }

        static void merge_lists(node *a, node *b) {
            assert(a != nullptr);
            assert(b != nullptr);
            /*if (a == nullptr && b == nullptr) return;
            
            if (a != nullptr && b == nullptr) return;

            if (a == nullptr && b != nullptr) {
                ptr_swap((void**)&a, (void**)&b);
                return;
            } */

            node *al = a;
            node *ar = a->left;
            node *bl = b;
            node *br = b->right;

            // new order: bl <-> al <...> ar <-> br
            al->left = bl;
            ar->right = br;
            br->left = ar;
            bl->right = al;
        }
        node *remove(bool children_stay = true) { // removes this from the fibonacci heap, returns new root
            if (children_stay && child != nullptr) {
                // clear children's pointers to us
                child->clear_parent(child);

                // TODO potentially we move some children into NOT-their-own home heap here
                // insert child list into root list
                node *childrens_root = child->home_heap()->root;
                if (childrens_root == nullptr) {
                    child->home_heap()->root = child;
                } else {
                    merge_lists(child, childrens_root);
                }
                child = nullptr;
            }
            if (parent != nullptr) {
                parent->lose_child(this, parent->home_heap()->root);
            } else {// we are a root
                left->right = right;
                right->left = left;
                // TODO: What do we do if home_heap()->root is pointing to us? I tried home_heap()->root = right but this breaks the sorting test in cleanup
            }
            parent = nullptr;

            // This is okay because the result is not used in the "distributed" fibheap
            return home_heap()->root;
        }
    public:
        home_heap_ptr_type home_heap() {
            return home_wrapper->get_home_heap();
        }
    };

    // data
    node *root = nullptr;
    home_heap_dsf_node *first_home_heap_dsf_node = new home_heap_dsf_node(this);
    home_heap_dsf_node *last_home_heap_dsf_node = first_home_heap_dsf_node;

    void reset() {
        root = nullptr;
        first_home_heap_dsf_node = new home_heap_dsf_node(this);
        last_home_heap_dsf_node = first_home_heap_dsf_node;
    }

    template <class F> void traverse_horizontal(node *start, F &&f) {
      // note that the callback function "f" is allowed to mutate its argument
      if (start != nullptr) {
          node *next = start;
          std::vector<node *> nodes;
          do {
              nodes.push_back(next);
          } while ((next = next->right) != start);
          for (node *n : nodes)
              f(n);
      }
    }

    node *cleanup() { // cleans up and returns the top element
        // we called pop, so there must be a root
        assert(root != nullptr);
        // find top element in rootlist
        node *top_element = root;
        traverse_horizontal(root, [&](node *n) {
            if (Compare()(n->key, top_element->key)) {
                top_element = n;
            }
        });

        // merge children of top element into root list
        traverse_horizontal(top_element->child, [&](node *n) {
            n->self_hug(); // so we don't mutate shit here
            n->parent = nullptr;
            n->home_heap()->steal(n);
        });

        // remove top element from root list
        top_element->left->right = top_element->right;
        top_element->right->left = top_element->left;
        root = top_element->left;
        if (top_element->left == top_element) {
            // if top_element is root element -> it must be the only element
            // since we remove it, we have no root left and are done
            root = nullptr;
            return top_element;
        }

        // I boldly assume that we'll never have
        // more than ~10^20 (F_99) elements in our heap
        std::array<node *, 100> order_rep;
        std::fill(order_rep.begin(), order_rep.end(), nullptr);

        {
            node *cur = root;
            while (true) {
                node *existing = order_rep[cur->order];
                if (existing != nullptr) {
                    // a node with the same order as cur exists
                    if (cur == existing) {
                        root = cur;
                        break;
                    }
                    order_rep[cur->order] = nullptr; // clear the spot
                    assert(cur != nullptr);
                    if (Compare()(existing->key, cur->key)) {
                        // cur's key is bigger
                        existing->merge_over(cur);
                        cur = existing;
                    } else {
                        // existing's key is bigger
                        cur->merge_over(existing);
                    }
                } else {
                    // no node of the same order as cur exists
                    order_rep[cur->order] = cur;
                    cur = cur->right;
                }
            }
        }

        return top_element;
    }
public:
    using handle = node*;
    fibonacci_heap() = default;
    fibonacci_heap(const fibonacci_heap&) = delete;
    ~fibonacci_heap() {
        home_heap_dsf_node *next = first_home_heap_dsf_node;
        while (next != nullptr) {
            auto tmp = next;
            next = next->next;
            delete tmp;
        }
    }

    value_type pop() {
        node *top_element = cleanup(); // cleanup cleans up and returns us the top node
        value_type result_key = top_element->key;
        delete top_element;
        return result_key;
    }

    handle push(const value_type& key) {
        handle new_node = new node{key, nullptr, nullptr, nullptr, nullptr, false, 0, first_home_heap_dsf_node};
        if (root == nullptr) {
            root = new_node;
            new_node->self_hug();
        } else {
            new_node->throw_in_root(root);
            // cleanup();
        }
        return new_node;
    }
    void push(handle x) {
        if (root != nullptr) x->throw_in_root(root);
        else {
            root = x;
            root->self_hug();
        }
    }
    void steal(handle x) { // steal entire subtree from home heap of x, push it into ourself
        x->remove(false);
        push(x);
    }
    void remove(handle x) { // remove a single element, invalidates handle
        // fibheap::remove cannot be used in the "distributed" fibheap
        root = x->remove(); // set new root
        delete x;
    }
    void meld(fibonacci_heap&& other) {
        last_home_heap_dsf_node->next = other.first_home_heap_dsf_node;
        last_home_heap_dsf_node = other.last_home_heap_dsf_node;
        if (root == nullptr) {
            root = other.root;
            other.reset();
            return;
        } else if (other.root == nullptr) {
            other.reset();
            return;
        }
        
        node::merge_lists(root, other.root);
        other.reset();
    }

    void decrease_key(handle x, const value_type& new_key) {
        x->decrease_key(new_key, root);
    }
    void unsafe_setkey(handle x, const value_type& new_key) {
        x->unsafe_setkey(new_key);
    }
};

};
