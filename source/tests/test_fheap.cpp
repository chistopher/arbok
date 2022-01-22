#include <gtest/gtest.h>
#include <arbok/data_structures/fibheap.h>
#include <vector>
#include <utility>
#include <memory>

using ::testing::Test;

namespace arbok {

// Test fixture required to access private priority queue
class FHeapTest : public ::testing::Test {
public:
    fheap::fibonacci_heap<int64_t> fheap_;
};

TEST_F(FHeapTest, InsertAndPop) {
    fheap::fibonacci_heap<int64_t>::handle h = fheap_.push(7);
    ASSERT_EQ(fheap_.pop(), 7);
}

TEST_F(FHeapTest, DiesOnEmptyHeap) {
    fheap::fibonacci_heap<int64_t>::handle h = fheap_.push(7);
    ASSERT_EQ(fheap_.pop(), 7);
    ASSERT_DEATH({
        fheap_.pop();
    }, "Assertion");
}

TEST_F(FHeapTest, NegativeKeys) {
    fheap::fibonacci_heap<int64_t>::handle h = fheap_.push(-5);
    h = fheap_.push(5);

    ASSERT_EQ(fheap_.pop(), -5);
    ASSERT_EQ(fheap_.pop(), 5);
}

TEST_F(FHeapTest, DecreaseKeyTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    auto handle = fheap_.push(42);
    fheap_.push(12);
    
    fheap_.decrease_key(handle, 9);

    ASSERT_EQ(fheap_.pop(), 9);

}

TEST_F(FHeapTest, HeapSort) {
    fheap::fibonacci_heap<int64_t>::handle h = fheap_.push(7);
    h = fheap_.push(3);
    h =fheap_.push(4);
    h =fheap_.push(12);
    h =fheap_.push(15);
    h = fheap_.push(static_cast<int64_t>(0));
    h =fheap_.push(1337);
    h =fheap_.push(42);
    h = fheap_.push(1);

    ASSERT_EQ(fheap_.pop(), 0);
    ASSERT_EQ(fheap_.pop(), 1);
    ASSERT_EQ(fheap_.pop(), 3);
    ASSERT_EQ(fheap_.pop(), 4);
    ASSERT_EQ(fheap_.pop(), 7);
    ASSERT_EQ(fheap_.pop(), 12);
    ASSERT_EQ(fheap_.pop(), 15);
    ASSERT_EQ(fheap_.pop(), 42);
    ASSERT_EQ(fheap_.pop(), 1337);
}

TEST_F(FHeapTest, BasicStealTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    fheap_.push(42);

    auto handle = fheap2.push(1337);
    fheap2.push(2);

    fheap_.steal(handle);

    ASSERT_EQ(fheap2.pop(), 2);
    ASSERT_EQ(fheap_.pop(), 42);
    ASSERT_EQ(fheap_.pop(), 1337);
}


TEST_F(FHeapTest, GetStealedTwiceTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    fheap::fibonacci_heap<int64_t> fheap3;

    fheap_.push(42);

    auto handle1 = fheap2.push(1337);
    fheap2.push(2);

    auto handle2 = fheap3.push(4711);

    fheap_.steal(handle1);
    fheap_.steal(handle2);

    fheap2.steal(handle2);

    ASSERT_EQ(fheap_.pop(), 42);
    ASSERT_EQ(fheap_.pop(), 1337);
    ASSERT_EQ(fheap2.pop(), 2);
    ASSERT_EQ(fheap2.pop(), 4711);
}

TEST_F(FHeapTest, StealAndInsertTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    fheap::fibonacci_heap<int64_t> fheap3;

    fheap_.push(1);
    auto handle2 = fheap_.push(5);

    auto handle1 = fheap2.push(2);
    fheap3.push(10);

    fheap3.steal(handle1);
    auto handle3 = fheap3.push(4);
    fheap3.steal(handle2);

    ASSERT_EQ(fheap3.pop(), 2);

    fheap2.steal(handle3);

    ASSERT_EQ(fheap2.pop(), 4);
}

TEST_F(FHeapTest, TestSomethingThatDoesCurrentlyNotWorkTest) {
    fheap::fibonacci_heap<int64_t> fheap2; // heap of 1
    fheap::fibonacci_heap<int64_t> fheap3; // heap of 0

    auto handle1 = fheap3.push(36983);
    auto handle3 = fheap3.push(447855);
    auto handle4 = fheap3.push(310109);
    auto handle5 = fheap3.push(289166);

    fheap2.steal(handle1);
    auto handle2 = fheap2.push(880873);

    fheap2.steal(handle3);
    fheap2.steal(handle4);
    fheap2.steal(handle5);

    ASSERT_EQ(fheap2.pop(), 36983);

    fheap2.unsafe_setkey(handle5, 65511);
    fheap_.steal(handle5); // heap of 4

    fheap2.unsafe_setkey(handle2, 904417);
    fheap_.steal(handle2); // THIS
}

TEST_F(FHeapTest, MeldAndStealTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    fheap::fibonacci_heap<int64_t> fheap3;

    fheap_.push(42);

    auto handle1 = fheap2.push(1337);
    fheap2.push(2);

    auto handle2 = fheap3.push(4711);
    fheap3.push(1);

    fheap2.meld(std::move(fheap3));

    fheap_.steal(handle1);
    fheap_.steal(handle2);

    ASSERT_EQ(fheap_.pop(), 42);
    ASSERT_EQ(fheap_.pop(), 1337);
    //Currently 4711 is apparently being moved back to its home heap, but why? Only childs of 1337 should be moved back to the home heap. 4711 is definetly not a child of 1337.
    //ASSERT_EQ(fheap_.pop(), 4711);
    ASSERT_EQ(fheap2.pop(), 1);
    ASSERT_EQ(fheap2.pop(), 2);
}

TEST_F(FHeapTest, StealFromTwoHeapsTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    fheap::fibonacci_heap<int64_t> fheap3;

    fheap_.push(42);

    auto handle1 = fheap2.push(1337);
    fheap2.push(2);

    auto handle2 = fheap3.push(4711);
    fheap3.push(2);

    fheap_.steal(handle1);
    fheap_.steal(handle2);

    ASSERT_EQ(fheap_.pop(), 42);
    ASSERT_EQ(fheap_.pop(), 1337);
    // Currently 4711 is apparently being moved back to its home heap, but why? Only childs of 1337 should be moved back to the home heap. 4711 is definetly not a child of 1337.
    ASSERT_EQ(fheap_.pop(), 4711);
    ASSERT_EQ(fheap2.pop(), 2);
    ASSERT_EQ(fheap3.pop(), 2);

}



TEST_F(FHeapTest, BasicMeldTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    fheap_.push(42);
    fheap2.push(1337);
    fheap2.push(2);

    fheap_.meld(std::move(fheap2));

    ASSERT_EQ(fheap_.pop(), 2);
    ASSERT_EQ(fheap_.pop(), 42);
    ASSERT_EQ(fheap_.pop(), 1337);

    // heap is empty now & still works
    fheap_.push(1);
    ASSERT_EQ(fheap_.pop(), 1);
}

TEST_F(FHeapTest, AdvancedMeldTest) {
    fheap::fibonacci_heap<int64_t> fheap2;
    fheap::fibonacci_heap<int64_t> fheap3;
    fheap::fibonacci_heap<int64_t> fheap4;

    fheap_.push(42);
    fheap2.push(1337);
    fheap2.push(2);
    fheap3.push(10);
    fheap3.push(11);
    fheap4.push(12);
    fheap4.push(1);

    fheap3.meld(std::move(fheap4));
    ASSERT_EQ(fheap3.pop(), 1);
    fheap3.push(14);

    fheap_.meld(std::move(fheap2));
    fheap_.meld(std::move(fheap3));

    ASSERT_EQ(fheap_.pop(), 2);
    ASSERT_EQ(fheap_.pop(), 10);
    ASSERT_EQ(fheap_.pop(), 11);
    ASSERT_EQ(fheap_.pop(), 12);
    ASSERT_EQ(fheap_.pop(), 14);
    ASSERT_EQ(fheap_.pop(), 42);
    ASSERT_EQ(fheap_.pop(), 1337);

    // heap is empty now & still works
    fheap_.push(1);
    ASSERT_EQ(fheap_.pop(), 1);
}


} // namespace arbok
