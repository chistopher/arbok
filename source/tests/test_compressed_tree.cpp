
#include <gtest/gtest.h>

#include <random>
#include <numeric>
#include <arbok/data_structures/compressed_tree.h>

using namespace std;

TEST(CompressedTreeTest, correct_find) {

    int n = 1000;
    arbok::CompressedTree<int> dsu(n);

    mt19937 gen;
    vector perm(n-1,0);
    std::iota(begin(perm), end(perm), 0);
    shuffle(begin(perm), end(perm), gen);
    for(auto i : perm)
        dsu.join(i,n-1);

    auto root= dsu.find(0);
    for(int i=0;i<n;++i)
        ASSERT_EQ(root, dsu.find(i));
}


TEST(CompressedTreeTest, correct_value) {

    int n = 1'000;
    int q = 8'000;

    mt19937 gen;
    uniform_real_distribution<> dist(0,1);
    uniform_int_distribution<> node_dist(0,n-1);
    enum class OPERATION { JOIN, FIND, ADD_VALUE };
    vector<tuple<OPERATION,int,int>> ops(q);
    for(auto& [type,node,value] : ops) {
        auto u = dist(gen);
        if(u>0.5) type = OPERATION::ADD_VALUE;
        else if(u>0.30) type = OPERATION::FIND;
        else type = OPERATION::JOIN;
        node = node_dist(gen);
        value = node_dist(gen);
    }

    shuffle(begin(ops), end(ops), gen);

    // init DS and naive impl
    arbok::CompressedTree<int> dsu(n);
    vector<int> parent(n);
    iota(begin(parent),end(parent),0);
    vector<int> value(n,0);

    // apply queries to both structures
    for(auto [type, node, val] : ops) {
        if(type==OPERATION::JOIN) {
            cout << "join " << node << ' ' << val << endl;
            auto joined = dsu.join(node,val);
            ASSERT_EQ(joined, parent[node] != parent[val]);
            auto root1 = parent[node];
            auto root2 = parent[val];
            for(auto& p : parent) if(p==root2) p = root1;
        } else if(type==OPERATION::ADD_VALUE) {
            cout << "add " << node << ' ' << val << endl;
            dsu.add_value(node,val);
            auto root = parent[node];
            for(int i=0;i<n;++i) if(parent[i]==root) value[i] += val;
        } else {
            cout << "find " << node << endl;
            ASSERT_EQ(dsu.find_value(node), value[node]);
        }
    }

    for(int i=0;i<n;++i)
        ASSERT_EQ(value[i], dsu.find_value(i));
}

