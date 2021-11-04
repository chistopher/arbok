
#include <iostream>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

#include <arbok/utils/paths.h>
#include <arbok/gabow/gabow.h>

using namespace std;
using ::testing::TestWithParam;
using ::testing::Values;

class SpeedrunTest : public TestWithParam<std::string> { };

using MyTypes = ::testing::Types<arbok::Gabow>;
TYPED_TEST_SUITE(SpeedrunTest, MyTypes);

TEST_P(SpeedrunTest, Gabow) {
    std::string input_file_name = std::string(DATA_DIR) + "fastestspeedrun/" + GetParam() + ".in";
    std::string correct_answer_file_name = std::string(DATA_DIR) + "fastestspeedrun/" + GetParam() + ".ans";

    std::ifstream inp(input_file_name, ios::in);
    int n;
    inp >> n;
    arbok::Gabow alg(n+1);
    for (int level = 1; level <= n; level++) {
        int shortcut_item, time;
        inp >> shortcut_item >> time;
        alg.create_edge(shortcut_item, level, time);
        for (int item = 0; item <= n; item++) {
            inp >> time;
            if (item != shortcut_item) {
                alg.create_edge(item, level, time);
            }
        }
    }
    auto weight = alg.run(0);

    std::ifstream answer_file(correct_answer_file_name);
    long long answer; answer_file >> answer;
    ASSERT_EQ(answer, weight);
}

// big (and slow) testcase have been removed
INSTANTIATE_TEST_SUITE_P(Correctness, SpeedrunTest, Values(
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
    "025-chain4",
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
