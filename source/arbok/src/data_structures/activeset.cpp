#include <map>
#include <limits>
#include <memory>
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
    _pq.emplace(key, node);
}

void StdPQActiveSet::move_and_modify_key(NodeType node, AbstractActiveSet& target_set, KeyType new_key = std::numeric_limits<KeyType>::min()) {
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
    move_and_modify_key(node, *this, new_key);
}

void StdPQActiveSet::move(NodeType node, AbstractActiveSet& target_set) {
    move_and_modify_key(node, target_set);
}