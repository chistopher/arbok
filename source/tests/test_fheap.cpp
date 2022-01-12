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




} // namespace arbok
