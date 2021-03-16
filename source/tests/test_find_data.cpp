
#include <gtest/gtest.h>

#include <arbok/paths.h>

using namespace std;

TEST(arbok, findsData) {
    auto data_dir = DATA_DIR;
    ASSERT_NE(data_dir, "");
}

