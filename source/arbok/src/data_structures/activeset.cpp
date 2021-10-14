#include <map>
#include <limits>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <arbok/data_structures/activeset.h>

namespace arbok {

unsigned long int StdPQActiveSet::size() const {
    return _pq.size();
}

bool StdPQActiveSet::empty() const {
    return _pq.empty();
}

std::pair<KeyType,NodeType> StdPQActiveSet::extractMin() {
    auto m = _pq.top();
    _pq.pop();
    return m;
}

void StdPQActiveSet::meld(std::vector<std::shared_ptr<AbstractActiveSet>>& active_sets) {
    for (std::shared_ptr<AbstractActiveSet> as : active_sets) {
        if (as.get() == nullptr) continue;
        while (!as->empty()) {
            std::pair<KeyType,NodeType> el = as->extractMin();
            _pq.push(static_cast<pq_element_type>(el));
        }
    }
}

void StdPQActiveSet::insert(KeyType key, NodeType node) {
    std::cout << "inserting node " << node << " with key " << key << std::endl;
    _pq.emplace(key, node);
    std::cout << "new size: " << _pq.size() << std::endl;
}

void StdPQActiveSet::move_and_modify_key(NodeType node, AbstractActiveSet& target_set, KeyType new_key = std::numeric_limits<KeyType>::min()) {
    std::cout << "moving node " << node << " with new key " << new_key << std::endl;
    std::vector<pq_element_type> element_cache;
    while (!empty() && _pq.top().second != node) {
        element_cache.push_back(std::move(const_cast<pq_element_type&>(_pq.top())));
        _pq.pop();
    }
    
    if (!empty()) {
        auto el = _pq.top();
        _pq.pop();
        if (new_key == std::numeric_limits<KeyType>::min()) new_key = el.first;
        target_set.insert(new_key, node);
    } else {
        std::cerr << "FATAL: Tried to move node " << node << " which does not exist in heap! Exiting." << std::endl;
        std::exit(-1);
    }
    
    for (auto& el : element_cache) _pq.emplace(el);
}

void StdPQActiveSet::decreaseKey(NodeType node, KeyType new_key) {
    move_and_modify_key(node, *this, new_key);
}

void StdPQActiveSet::move(NodeType node, AbstractActiveSet& target_set) {
    move_and_modify_key(node, target_set);
}

void StdPQActiveSet::remove(NodeType node) {
    // i know this it lots of duplicated code atm

    if (_pq.empty()) std::cerr << "holy shit" << std::endl;

    std::vector<pq_element_type> element_cache;
    while (!empty() && _pq.top().second != node) {
        element_cache.push_back(std::move(const_cast<pq_element_type&>(_pq.top())));
        _pq.pop();
    }
    
    if (!empty()) {
        _pq.pop();
    } else {
        std::cerr << "FATAL: Tried to remove node " << node << " which does not exist in heap! Element cache:" << std::endl;
        for (auto& elem : element_cache) {
            std::cout << elem.second << " ";
        }
        std::cout << std::endl;
        std::cout << "Exiting," << std::endl;
        
        std::exit(-1);
    }
    
    for (auto& el : element_cache) _pq.emplace(el);
}

}