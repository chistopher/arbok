#include <gtest/gtest.h>
#include <arbok/data_structures/activeset.h>
#include <vector>
#include <utility>

using ::testing::Test;

// Test fixture required to access private priority queue
class PQActiveSetTest : public ::testing::Test {
public:
    StdPQActiveSet as;
protected:
    decltype(as._pq)* getPQ() { return &(as._pq); }
};

TEST_F(PQActiveSetTest, InsertTest) {
    ASSERT_EQ(getPQ()->size(), 0);
    as.insert(1, 0);
    ASSERT_EQ(getPQ()->size(), 1);
    ASSERT_EQ(getPQ()->top().first, 1);
    ASSERT_EQ(getPQ()->top().second, 0);
}

TEST_F(PQActiveSetTest, ExtractMinTest) {
    ASSERT_EQ(getPQ()->size(), 0);
    as.insert(20, 2);
    as.insert(10, 1);
    as.insert(30, 3);
    ASSERT_EQ(getPQ()->size(), 3);
    auto min_el = as.extractMin();
    ASSERT_EQ(min_el.first, 10);
    ASSERT_EQ(min_el.second, 1);
    ASSERT_EQ(getPQ()->size(), 2);

    min_el = as.extractMin();
    ASSERT_EQ(min_el.first, 20);
    ASSERT_EQ(min_el.second, 2);
    ASSERT_EQ(getPQ()->size(), 1);

    as.insert(2, 4);

    min_el = as.extractMin();
    ASSERT_EQ(min_el.first, 2);
    ASSERT_EQ(min_el.second, 4);
    ASSERT_EQ(getPQ()->size(), 1);

}

TEST_F(PQActiveSetTest,  DecreaseKeyTest) {
    ASSERT_EQ(getPQ()->size(), 0);
    as.insert(20, 2);
    as.insert(10, 1);
    as.insert(30, 3);
    ASSERT_EQ(getPQ()->size(), 3);
    as.decreaseKey(2, 5);

    // size does not change
    ASSERT_EQ(getPQ()->size(), 3);

    // correct element is now on top
    ASSERT_EQ(getPQ()->top().second, 2);

    // check if element is extracted correctly
    auto min_el = as.extractMin();
    ASSERT_EQ(min_el.first, 5);
    ASSERT_EQ(min_el.second, 2);
}

TEST_F(PQActiveSetTest,  MeldTest) {
    StdPQActiveSet as2, as3;
    as.insert(10,1);
    as2.insert(20, 2);
    as3.insert(30, 3);

    std::vector<PQActiveSetTest> melding_sets;
    melding_sets.push_back(std::move(as2));
    melding_sets.push_back(std::move(as3));

    as.meld(melding_sets);
}

TEST_F(PQActiveSetTest,  MoveTest) {
    
}