
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include <arbok/tarjan/tarjan.h>
#include <arbok/utils/paths.h>
#include <arbok/gabow/gabow.h>

using namespace std;
using ::testing::TestWithParam;
using ::testing::Values;

class CodeforcesTest : public TestWithParam<std::string> { };


void isArborescence(vector<arbok::Edge>& graph, vector<arbok::Edge>& arbo, long long claimed_weight, int n, int root) {
    // weight is same as claimed
    auto check_sum = 0ll;
    for(auto e : arbo) check_sum += e.orig_weight;
    ASSERT_EQ(check_sum, claimed_weight);

    // each node except root has one incoming edge
    ASSERT_EQ(size(arbo), n-1);
    vector has_inc(n, false);
    for(auto e : arbo) {
        assert(!has_inc[e.to]);
        has_inc[e.to] = true;
    }
    ASSERT_FALSE(has_inc[root]);

    // all arbo edges exists like this in the original graph
    vector<tuple<int,int,int>> edges;
    edges.reserve(size(graph));
    for(auto e : graph) edges.emplace_back(e.from, e.to, e.weight);
    sort(begin(edges), end(edges));
    for(auto e : arbo)
        ASSERT_TRUE(binary_search(begin(edges), end(edges), tuple{e.from, e.to, e.orig_weight}));
}

template<class Algo>
void testImpl(const string& testcase_name) {
    std::ifstream inp(std::string(DATA_DIR) + "fastestspeedrun/" + testcase_name + ".in"); \
    std::ifstream answer_file(std::string(DATA_DIR) + "fastestspeedrun/" + testcase_name + ".ans");

    vector<arbok::Edge> graph;

    int n; inp >> n;
    Algo alg(n+1);
    for (int level = 1; level <= n; level++) {
        int shortcut_item, time;
        inp >> shortcut_item >> time;
        alg.create_edge(shortcut_item, level, time);
        graph.push_back({shortcut_item, level, time, time});
        for (int item = 0; item <= n; item++) {
            inp >> time;
            if (item != shortcut_item) {
                alg.create_edge(item, level, time);
                graph.push_back({item, level, time, time});
            }
        }
    }

    long long ans;
    answer_file >> ans;

    auto res = alg.run(0);
    ASSERT_EQ(ans, res);

    auto arbo = alg.reconstruct(0);

    isArborescence(graph, arbo, ans, n+1, 0);
}

TEST_P(CodeforcesTest, MatrixTarjan) {
    testImpl<arbok::MatrixTarjan>(GetParam());
}

TEST_P(CodeforcesTest, SetTarjan) {
    testImpl<arbok::SetTarjan>(GetParam());
}

TEST_P(CodeforcesTest, TreapTarjan) {
    testImpl<arbok::TreapTarjan>(GetParam());
}

TEST_P(CodeforcesTest, PQTarjan) {
    testImpl<arbok::PQTarjan>(GetParam());
}

TEST_P(CodeforcesTest, Gabow) {
    testImpl<arbok::Gabow>(GetParam());
}

// big (and slow) testcase have been removed
INSTANTIATE_TEST_SUITE_P(Correctness, CodeforcesTest, Values(
    "001-small1",
    "002-small2",
    "003-small3",
    "004-small4",
    "005-small5",
    "006-itsybitsycycles04",
    "007-pad-small1",
    "008-pad-small2",
    "009-pad-small3",
    "010-pad-small4",
    "011-pad-small5",
    "012-small-maximal",
    "013-medium-maximal",
    "014-biggish-maximal",
    "015-manycycles2k5",
    "016-ones-and-twos1-2k5",
    "017-ones-and-twos2-2k5",
    "018-maxanswer",
    "019-self1",
    "020-self2",
    "021-chunks1",
    "022-chain1",
    "023-chain2",
    "024-chain3",
    "025-chain4",
    "026-rand1",
    "027-rand2",
    "028-largecyc1",
    "029-largecyc2",
    "030-manycycles1k",
    "031-manycycles2k",
    "032-chain5_2k5",
    "033-rand3_2k5",
    "034-maxcycle",
    "035-itsybitsycycles05",
    "036-itsybitsycycles11",
    "037-itsybitsycycles12",
    "038-particycles01",
    "039-particycles03",
    "040-tinycycles00",
    "041-tinycycles01",
    "042-tinycycles07",
    "043-littlecycles03",
    "044-littlecycles04",
    "045-littlecycles05",
    "046-minitest_alice_jeroenkill-hand",
    "047-challenge_gregor-hand"
));
