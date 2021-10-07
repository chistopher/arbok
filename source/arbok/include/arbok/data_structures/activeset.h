#pragma once
#include <cinttypes>
#include <queue>
#include <vector>
#include <utility>
#include <memory>

#include <arbok/gabow/gabow.h>
using NodeType = uint64_t; // TODO remove and define in gabow.h
using KeyType = uint64_t;
using pq_element_type = std::pair<KeyType,NodeType>;

class AbstractActiveSet {
public:
    virtual unsigned long int size() const = 0;
    virtual bool empty() const = 0;
    virtual std::pair<KeyType,NodeType> extractMin() = 0;
    virtual void meld(std::vector<std::shared_ptr<AbstractActiveSet>>& active_sets) = 0;
    virtual void insert(KeyType key, NodeType node) = 0;
    virtual void decreaseKey(NodeType node, KeyType new_key) = 0;
    virtual void move(NodeType node, AbstractActiveSet& target_set) = 0;
    virtual void move_and_modify_key(NodeType node, AbstractActiveSet& target_set, KeyType new_key) = 0;
};

class StdPQActiveSet: public AbstractActiveSet {
    friend class PQActiveSetTest;
public:
    virtual unsigned long int size() const;
    virtual bool empty() const;
    virtual std::pair<KeyType,NodeType> extractMin();
    virtual void meld(std::vector<std::shared_ptr<AbstractActiveSet>>& active_sets);
    virtual void insert(KeyType key, NodeType node);
    virtual void decreaseKey(NodeType node, KeyType new_key);
    virtual void move(NodeType node, AbstractActiveSet& target_set);
    virtual void move_and_modify_key(NodeType node, AbstractActiveSet& target_set, KeyType new_key);
private:
    std::priority_queue<pq_element_type, std::vector<pq_element_type>, std::greater<>> _pq;
};

