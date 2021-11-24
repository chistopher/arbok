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
    using size_type = std::size_t;
protected:
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
            uint8_t _order
        ) : key(_key), left(_left), right(_right), parent(_parent), child(_child), is_loser(_is_loser), order(_order) { }

        // data
        value_type key;
        node *left;
        node *right;
        node *parent;
        node *child;
        bool is_loser;
        uint8_t order;

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
        void lose_child(node *which, node *root) {
            if (which == child) {
                if (child == child->left) child = nullptr; // no child left
                else child = lost->left; // replace child
            }
            if (parent == nullptr) return; // if we are in root we remain there
            if (!is_loser) is_loser = true;
            else {
                throw_in_root(root);
            }
        }
        void adopt_by(node *new_parent) {
            if (parent != nullptr) parent->order--;
            parent = new_parent;
            parent->child = this;
            if (parent) parent->order++;
            left_sibling = this;
            right_sibling = this;
        }
        void throw_in_root(node *root) {
            // tuck left and right sibling together
            if (left != nullptr) left->right = right;
            if (right != nullptr) right->left = left;

            // tell parent
            if (parent != nullptr) parent->lose_child(this, root);

            // go to root
            merge_brother(root);

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
        void decrease_key(const value_type& new_key, node *root) {

            assert(Compare(new_key, key)); // new key should be smaller than current key
            key = new_key; // this is a copy, i hope this is okay

            // if we have a parent and we have a larger key than it -> heap property violation
            if (parent != nullptr && Compare(key, parent->key))
                throw_in_root(root);
        }
    };

    // data
    size_type n = 0;
    node *root = nullptr;

    node *cleanup() { // cleans up and returns the top element
        // TODO
    }
public:
    using handle = node*;
    fibonacci_heap() = default;
    fibonacci_heap(const fibonacci_heap&) = delete;

    bool empty() const { return n == 0; }
    size_type size() const { return n; }

    value_type pop() {
        node *top_element = cleanup();
        value_type result_key = top_element->key;
        delete top_element;
        return reuslt_key;
    }
    handle push(const value_type& key) {
        handle new_node = new node{key, nullptr, nullptr, nullptr, nullptr, false, 0};
        if (empty()) {
            root = new_node;
            new_node->self_hug();
        } else {
            new_node->throw_in_root(root);
            // cleanup();
        }
        n++;
        return new_node;
    }
    void push(handle x) {
        if (root != nullptr) x->throw_in_root(root);
        else root = x;
        n++;
    }
    void steal(handle x) { // steal entire subtree out
        // TODO
    }
    void remove(handle x) { // remove a single element
        // TODO
    }
    void meld(fibonacci_heap&& other) {
        // TODO
    }
    void decrease_key(handle x, const value_type& new_key) {
        x->decrease_key(new_key, root);
    }
};

};
