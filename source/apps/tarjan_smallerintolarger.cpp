#include <iostream>
#include <set>
#include <queue>
#include <numeric>
#include <vector>
 
using namespace std;
 
struct edge {
    int from, to, weight;
    edge(int from, int to, int weight) : from(from), to(to), weight(weight) {}
    bool operator<(const edge &b) const {
        if (weight == b.weight) {
            return from < b.from;
        }
        return weight < b.weight;
    }
};
 
class ContractionManagementDSU {
    vector<int> parent;
    vector<set<edge>> edges;
    vector<long long> offsets;
    int find(int element) {
        if (parent[element] != element) {
            parent[element] = find(parent[element]);
        }
        return parent[element];
    }
public:
    ContractionManagementDSU(vector<vector<edge>>& G) {
        parent.resize(G.size());
        edges.resize(G.size());
        offsets.resize(G.size(), 0);
        iota(parent.begin(), parent.end(), 0);
        for (auto edgelistIt = G.begin(); edgelistIt != G.end(); edgelistIt++) {
            int i = edgelistIt - G.begin();
            edges[i] = set<edge>(edgelistIt->begin(), edgelistIt->end());
        }
    }
    void merge(int a, int b) {
        if (find(a) == find(b)) return;
        auto& setA = edges[find(a)];
        long long offsetA = offsets[find(a)];
        auto& setB = edges[find(b)];
        long long offsetB = offsets[find(b)];
 
        // Herausfinden, welches Set größer ist
        set<edge> *smallerSet = &setA;
        long long smallerOffset = offsetA;
        set<edge> *largerSet = &setB;
        long long largerOffset = offsetB;
        int smallerRep = find(a);
        int largerRep = find(b);
        if (setA.size() > setB.size()) {
            swap(largerSet, smallerSet);
            swap(largerOffset, smallerOffset);
            swap(largerRep, smallerRep);
        }
 
        // Offset anwenden auf smaller set, smaller into larger
        for (auto edgeIt = smallerSet->begin(); edgeIt != smallerSet->end(); ) {
 
            auto toMove = edgeIt++;
            auto e = smallerSet->extract(toMove);
 
            e.value().weight += smallerOffset - largerOffset;
            largerSet->insert(move(e));
            //largerSet->emplace(e.from, e.to, e.weight + smallerOffset - largerOffset);
        }
 
        parent[smallerRep] = largerRep; // largerRep is new representant
    }
    int operator[](int element) {
        // returns set representant
        return find(element);
    }
    void addWeightOffset(int element, long long offset) {
        offsets[find(element)] += offset;
    }
    long long getWeightOffset(int element) {
        return offsets[find(element)];
    }
    set<edge>* getEdges(int element) {
        return &edges[find(element)];
    }
};
 
class CycleDetectionDSU {
    vector<int> parent;
public:
    CycleDetectionDSU(vector<vector<edge>>& G) {
        parent.resize(G.size());
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int element) {
        if (parent[element] != element) {
            parent[element] = find(parent[element]);
        }
        return parent[element];
    }
    bool merge(int a, int b) {
        // nicht kommutativ
        if(find(a) == find(b)){
            // Cycle
            return true;
        }
        parent[find(a)] = find(b);
        return false;
    }
    int operator[](int element) {
        // returns set representant
        return find(element);
    }
};
 
class Tarjan {
    vector<vector<edge>>& _graph;
    long long _weight;
    vector<edge> _answer;
public:
    Tarjan(vector<vector<edge>>& G)
    :  _graph(G), _weight(0) {}
 
    void run(int root) {
        queue<int> q;
        ContractionManagementDSU co(_graph);
        CycleDetectionDSU cy(_graph);
        vector<int> pi(_graph.size(), -1);
 
        // put all nodes v != root in queue
        for (unsigned int vertex = 0; vertex < _graph.size(); vertex++) {
            if (__builtin_expect(static_cast<int>(vertex) != root,1)) q.push(vertex);
        }
 
        // while there is a node v in the queue
        while (!q.empty()) {
            int v = co[q.front()];
            q.pop();
 
            auto& newEdge = *co.getEdges(v)->begin();
            pi[v] = newEdge.from;
            _answer.push_back(newEdge);
 
            long long edgeRestCost = newEdge.weight + co.getWeightOffset(v);
            //assert(co.getWeightOffset(v) <= 0);
            //assert(edgeRestCost >= 0);
            _weight += edgeRestCost;
            co.addWeightOffset(v, -edgeRestCost);
 
            if (!cy.merge(newEdge.from, newEdge.to)) continue;
            // we did build a cycle:
 
            // merge incoming edges of cycle, contract to one node
            for (int cur = co[pi[v]]; cur != co[v]; cur = co[pi[cur]]) {
                //assert(cur != -1);
                co.merge(cur, v);
            }
 
            // delete self-loops of cycle
            while (co[co.getEdges(v)->begin()->from] == co[v]) {
                co.getEdges(v)->erase(co.getEdges(v)->begin());
            }
 
            // push contracted node
            q.push(co[v]);
        }
    }
    long long weight() {
        return _weight;
    }
};

