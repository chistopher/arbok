#include <map>
#include <limits>
#include <arbok/data_structures/activeset.h>

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

void StdPQActiveSet::meld(std::vector<AbstractActiveSet>& active_sets) {
    for (auto& as : active_sets) {
        // when we coded this, we have been young and free
        StdPQActiveSet& _as = dynamic_cast<StdPQActiveSet&>(as); // wyld
        while (!_as._pq.empty()) {
            pq_element_type el = _as._pq.top();
            _as._pq.pop();
            _pq.push(el);
        }
    }
}

void StdPQActiveSet::insert(KeyType key, NodeType node) {
    _pq.emplace(key, node);
}

void StdPQActiveSet::_move_and_modify_key(NodeType node, AbstractActiveSet& target_set, KeyType new_key = std::numeric_limits<KeyType>::min()) {
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
    }
    
    for (auto& el : element_cache) _pq.emplace(el);
}

void StdPQActiveSet::decreaseKey(NodeType node, KeyType new_key) {
    _move_and_modify_key(node, *this, new_key);
//    std::vector<pq_element_type> element_cache;
//    while (!empty() && _pq.top().second != node) {
//        element_cache.push_back(std::move(const_cast<pq_element_type&>(_pq.top())));
//        _pq.pop();
//    }
//
//    if (!empty()) {
//        _pq.pop();
//        _pq.emplace(new_key, node);
//    }
//    
//    for (auto& el : element_cache) _pq.emplace(el);
}

void StdPQActiveSet::move(NodeType node, AbstractActiveSet& target_set) {
    _move_and_modify_key(node, target_set);
}