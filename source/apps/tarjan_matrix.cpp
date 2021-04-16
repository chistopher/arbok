#include <iostream>
#include <set>
#include <queue>
#include <numeric>
#include <vector>
#include <limits>

using namespace std;

constexpr int NO_EDGE = numeric_limits<int>::max();

class DSU {
    vector<int> parent;
    int find(int element) {
        if (parent[element] != element) {
            parent[element] = find(parent[element]);
        }
        return parent[element];
    }
public:
    DSU(int num_items) {
        parent.resize(num_items);
        iota(parent.begin(), parent.end(), 0);
    }
    bool merge(int a, int b) {
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
    vector<vector<int>>& adj;
    long long _weight;
public:
    Tarjan(vector<vector<int>>& adj)
    :  adj(adj), _weight(0) {}

    void run(int root) {
        queue<int> q;
        DSU co(adj.size());
        DSU cy(adj.size());
        vector<int> pi(adj.size(), -1);

        // put all nodes v != root in queue
        for (unsigned int vertex = 0; vertex < adj.size(); vertex++) {
            if (__builtin_expect(static_cast<int>(vertex) != root,1)) q.push(vertex);
        }

        // while there is a node v in the queue
        while (!q.empty()) {
            int v = co[q.front()];
            q.pop();

            //get cheapest incoming edge
            pair<int,int> mw_mo = {NO_EDGE, 0};
            for (int origin = 0; origin < adj.size(); origin++) {
                if (co[origin] == v) continue; // we don't care about self loops
                mw_mo = min(mw_mo, {adj[v][origin], co[origin]});
            }

            auto& [min_edge_weight, min_edge_origin] = mw_mo;
            pi[v] = min_edge_origin;
            _weight += min_edge_weight;

            // make all incoming edges of v cheaper
            for (int origin = 0; origin < adj.size(); origin++) {
                if (adj[v][origin] == NO_EDGE) continue;
                if (origin == v) continue;
                adj[v][origin] -= min_edge_weight;
            }

            if (!cy.merge(min_edge_origin, v)) continue;
            // we did build a cycle:
            // merge incoming edges of cycle, contract to one node

            // merge all columns into (v)
            for (int cur = co[pi[v]]; cur != co[v]; cur = co[pi[cur]]) {
                co.merge(cur, v); // merge cur into v
                for (int origin = 0; origin < adj.size(); origin++) {
                    adj[v][co[origin]] = min(adj[v][co[origin]], adj[cur][co[origin]]);
                }
            }

            // push contracted node
            q.push(co[v]);
        }
    }
    long long weight() {
        return _weight;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    // axis 0 is to, axis 1 is from
    // -1 represents no edge
    vector<vector<int>> adj;
    cin >> n;
    adj.resize(n+1, vector<int>(n+1, std::numeric_limits<int>::max()));
    for (int level = 1; level <= n; level++) {
        int shortcut_item, time;
        cin >> shortcut_item >> time;
        adj[level][shortcut_item] = time;
        for (int item = 0; item <= n; item++) {
            cin >> time;
            if (item != shortcut_item) adj[level][item] = time;
        }
    }

    Tarjan tarjan(adj);
    tarjan.run(0);
    cout << tarjan.weight() << endl;

    return 0;
}
