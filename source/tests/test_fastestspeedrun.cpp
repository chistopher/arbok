
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <gtest/gtest.h>

#include <arbok/tarjan.h>
#include <arbok/cf_solution.h>
#include <arbok/paths.h>

using namespace std;
using ::testing::TestWithParam;
using ::testing::Values;

class CodeforcesTest : public TestWithParam<std::string> {
public:
    void SetUp() override {
        testcase_name = GetParam();
    }
protected:
    std::string testcase_name;
};

#define CorrectnessTest(variant) \
    std::string input_file_name = std::string(DATA_DIR) + "fastestspeedrun/" + testcase_name + ".in"; \
    std::string correct_answer_file_name = std::string(DATA_DIR) + "fastestspeedrun/" + testcase_name + ".ans"; \
    std::string answer_file_name = string("/tmp/") + testcase_name + ".out"; \
    \
    { \
    std::ifstream inp(input_file_name, ios::in); \
    std::ofstream out(answer_file_name, ios::out); \
    arbok::cf_solution::solve(inp, out, variant); \
    } \
    \
    std::ifstream answer_file(answer_file_name, ios::in); \
    std::ifstream correct_answer_file(correct_answer_file_name, ios::in); \
    long long correct_answer, answer; \
    correct_answer_file >> correct_answer; \
    answer_file >> answer; \
    ASSERT_EQ(correct_answer, answer);

TEST_P(CodeforcesTest, MatrixTarjan) {
    CorrectnessTest(arbok::TarjanVariant::MATRIX);
}

TEST_P(CodeforcesTest, SetTarjan) {
    CorrectnessTest(arbok::TarjanVariant::SET)
}

TEST_P(CodeforcesTest, TreapTarjan) {
    CorrectnessTest(arbok::TarjanVariant::TREAP)
}

TEST_P(CodeforcesTest, PQTarjan) {
    CorrectnessTest(arbok::TarjanVariant::PQ)
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
