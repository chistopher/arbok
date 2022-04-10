
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include <arbok/utils/paths.h>
#include <arbok/utils/utils.h>
#include <arbok/gabow/gabow.h>
#include <arbok/tarjan/tarjan_set.h>
#include <arbok/tarjan/tarjan_pq.h>
#include <arbok/tarjan/tarjan_matrix.h>
#include <arbok/tarjan/tarjan_treap.h>
#include <arbok/tarjan/tarjan_hollow.h>
#include <arbok/others/felerius.h>
#include <arbok/others/yosupo.h>
#include <arbok/others/atofigh.h>
#include <arbok/others/lemon.h>

using namespace std;
using ::testing::TestWithParam;
using ::testing::Values;

class CodeforcesTest : public TestWithParam<std::string> { };

template<class Algo>
void testImpl(const string& testcase_name) {
    std::ifstream answer_file(std::string(DATA_DIR) + "fastestspeedrun/" + testcase_name + ".ans");

    auto graph = arbok::fromFile(DATA_DIR + "fastestspeedrun/"s + testcase_name + ".wsoap");

    Algo alg(graph.n, (int)size(graph.edges));
    for(auto e : graph.edges)
        alg.create_edge(e.from, e.to, e.weight);

    long long ans;
    answer_file >> ans;

    auto res = alg.run(0);
    ASSERT_EQ(ans, res);

    auto arbo = alg.reconstruct(0);

    ASSERT_TRUE(isArborescence(graph, arbo, ans, 0));
}

TEST_P(CodeforcesTest, TarjanMatrix) { testImpl<arbok::TarjanMatrix>(GetParam()); }
TEST_P(CodeforcesTest, TarjanSet) { testImpl<arbok::TarjanSet>(GetParam()); }
TEST_P(CodeforcesTest, TarjanTreap) { testImpl<arbok::TarjanTreap>(GetParam()); }
TEST_P(CodeforcesTest, TarjanPQ) { testImpl<arbok::TarjanPQ>(GetParam()); }
TEST_P(CodeforcesTest, TarjanHollow) { testImpl<arbok::TarjanHollow>(GetParam()); }
TEST_P(CodeforcesTest, Gabow) { testImpl<arbok::Gabow>(GetParam()); }
TEST_P(CodeforcesTest, Felerius) { testImpl<arbok::Felerius>(GetParam()); }
TEST_P(CodeforcesTest, Yosupo) { testImpl<arbok::Yosupo>(GetParam()); }
TEST_P(CodeforcesTest, Atofigh) { testImpl<arbok::Atofigh>(GetParam()); }
TEST_P(CodeforcesTest, Lemon) { testImpl<arbok::Lemon>(GetParam()); }

INSTANTIATE_TEST_SUITE_P(SmallOnes, CodeforcesTest, Values(
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
    "019-self1",
    "020-self2",
    "021-chunks1",
    "022-chain1",
    "023-chain2",
    "024-chain3",
    "026-rand1",
    "027-rand2",
    "028-largecyc1",
    "029-largecyc2",
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

INSTANTIATE_TEST_SUITE_P(LargeOnes, CodeforcesTest, Values(
    "015-manycycles2k5",
    "016-ones-and-twos1-2k5",
    "017-ones-and-twos2-2k5",
    "018-maxanswer",
    "025-chain4",
    "030-manycycles1k",
    "031-manycycles2k",
    "032-chain5_2k5",
    "033-rand3_2k5",
    "034-maxcycle"
));
