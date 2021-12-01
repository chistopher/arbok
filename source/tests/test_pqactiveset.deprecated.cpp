#include <gtest/gtest.h>
#include <arbok/data_structures/activeset.h>
#include <vector>
#include <utility>
#include <memory>

using ::testing::Test;

namespace arbok {

// Test fixture required to access private priority queue
class PQActiveSetTest : public ::testing::Test {
public:
    StdPQActiveSet as;
protected:
    decltype(as._pq)* getPQ() { return &(as._pq); }
};

// Whitebox tests, i.e.,
// we access the priority queue directly to avoid re-testing extractMin several times

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

TEST_F(PQActiveSetTest, BasicMeldTest) {
    std::shared_ptr<StdPQActiveSet> as2 = std::make_shared<StdPQActiveSet>();
    std::shared_ptr<StdPQActiveSet> as3 = std::make_shared<StdPQActiveSet>();
    as.insert(10,1);
    as2->insert(5, 2);
    as3->insert(30, 3);

    std::vector<std::shared_ptr<AbstractActiveSet>> melding_sets;
    melding_sets.push_back(as2);
    melding_sets.push_back(as3);
    as.meld(melding_sets);

    ASSERT_EQ(getPQ()->size(), 3);
    ASSERT_EQ(as2->empty(), true);
    ASSERT_EQ(as3->empty(), true);
    ASSERT_EQ(getPQ()->top().second, 2);
}

TEST_F(PQActiveSetTest,  MeldEmptyASDoesNothingTest) {
    std::shared_ptr<StdPQActiveSet> as2 = std::make_shared<StdPQActiveSet>();
    as.insert(10,1);

    std::vector<std::shared_ptr<AbstractActiveSet>> melding_sets;
    melding_sets.push_back(as2);
    ASSERT_EQ(as2->empty(), true);
    ASSERT_EQ(getPQ()->size(), 1);
    as.meld(melding_sets);
    ASSERT_EQ(getPQ()->size(), 1);
    ASSERT_EQ(as2->empty(), true);

    ASSERT_EQ(getPQ()->top().second, 1);
}

TEST_F(PQActiveSetTest,  MeldDoesNotFailOnNullptrTest) {
    as.insert(10,1);
    std::vector<std::shared_ptr<AbstractActiveSet>> melding_sets;
    melding_sets.push_back(std::shared_ptr<StdPQActiveSet>());
    ASSERT_EQ(getPQ()->size(), 1);
    as.meld(melding_sets);
    ASSERT_EQ(getPQ()->size(), 1);
    ASSERT_EQ(getPQ()->top().second, 1);
}

TEST_F(PQActiveSetTest,  MeldDoesNotFailWithEmptyVectorTest) {
    as.insert(10,1);
    std::vector<std::shared_ptr<AbstractActiveSet>> melding_sets;
    ASSERT_EQ(getPQ()->size(), 1);
    as.meld(melding_sets);
    ASSERT_EQ(getPQ()->size(), 1);
    ASSERT_EQ(getPQ()->top().second, 1);
}

TEST_F(PQActiveSetTest,  MoveWorksWithNonCheapestElementTest) {
    StdPQActiveSet as2;
    as.insert(10, 1);
    as.insert(20, 2);
    as2.insert(5, 3);

    ASSERT_EQ(getPQ()->size(), 2);
    ASSERT_EQ(as2.size(), 1);
    ASSERT_EQ(getPQ()->top().second, 1);

    as.move(1, as2);

    ASSERT_EQ(getPQ()->size(), 1);
    ASSERT_EQ(getPQ()->top().second, 2);
    ASSERT_EQ(as2.size(), 2);
    ASSERT_EQ(as2.extractMin().second, 3);
}

TEST_F(PQActiveSetTest,  MoveWorksWithCheapestElementTest) {
    StdPQActiveSet as2;
    as.insert(10, 1);
    as.insert(20, 2);
    as2.insert(5, 3);

    ASSERT_EQ(getPQ()->size(), 2);
    ASSERT_EQ(as2.size(), 1);
    ASSERT_EQ(getPQ()->top().second, 1);

    as2.move(3, as);

    ASSERT_EQ(getPQ()->size(), 3);
    ASSERT_EQ(getPQ()->top().second, 3);
    ASSERT_EQ(as2.empty(), true);
}

TEST_F(PQActiveSetTest,  MoveWithKeyUpdateToMinimumTest) {
    StdPQActiveSet as2;
    as.insert(10, 1);
    as.insert(20, 2);
    as2.insert(30, 3);

    ASSERT_EQ(getPQ()->size(), 2);
    ASSERT_EQ(as2.size(), 1);
    ASSERT_EQ(getPQ()->top().second, 1);

    as2.move_and_modify_key(3, as, 5);

    ASSERT_EQ(getPQ()->size(), 3);
    ASSERT_EQ(getPQ()->top().second, 3);
    ASSERT_EQ(getPQ()->top().first, 5);
    ASSERT_EQ(as2.empty(), true);
}

TEST_F(PQActiveSetTest,  MoveWithKeyUpdateToNonMinimumTest) {
    StdPQActiveSet as2;
    as.insert(10, 1);
    as.insert(20, 2);
    as2.insert(30, 3);

    ASSERT_EQ(getPQ()->size(), 2);
    ASSERT_EQ(as2.size(), 1);
    ASSERT_EQ(getPQ()->top().second, 1);

    as2.move_and_modify_key(3, as, 4711);

    ASSERT_EQ(getPQ()->size(), 3);
    ASSERT_EQ(getPQ()->top().second, 1);
    ASSERT_EQ(getPQ()->top().first, 10);
    ASSERT_EQ(as2.empty(), true);

    as.extractMin();
    as.extractMin();

    ASSERT_EQ(getPQ()->top().second, 3);
    ASSERT_EQ(getPQ()->top().first, 4711);
}

}