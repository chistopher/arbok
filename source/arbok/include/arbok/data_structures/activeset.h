#pragma once
#include <cinttypes>
#include <queue>
#include <vector>
#include <utility>
#include <memory>

#include <arbok/data_structures/fibheap.h>
#include <arbok/data_structures/edgelink.h>

namespace arbok
{

    class AbstractActiveSetHandle
    {
    };

    class AbstractActiveSet
    {
    public:
        virtual unsigned long int size() const = 0;
        virtual bool empty() const = 0;
        virtual EdgeLink pop() = 0;
        virtual void meld(std::shared_ptr<AbstractActiveSet> other) = 0;
        virtual std::shared_ptr<AbstractActiveSetHandle> insert(EdgeLink v) = 0;
        virtual void remove(std::shared_ptr<AbstractActiveSetHandle> handle) = 0;
        // hacky: decrease key should update the EdgeLink at it to be equivalent to the new_key edgelink, i.e., Node->EdgeLink = new_key
        virtual void decreaseKey(std::shared_ptr<AbstractActiveSetHandle> handle, EdgeLink new_key) = 0;
        virtual void steal(std::shared_ptr<AbstractActiveSetHandle> handle) = 0;
    };

    class FibHeapActiveSetHandle : public AbstractActiveSetHandle
    {
    public:
        fheap::fibonacci_heap<EdgeLink>::handle handle;
    };

    class FibHeapActiveSet : public AbstractActiveSet
    {
    public:
        virtual unsigned long int size() const;
        virtual bool empty() const;
        virtual EdgeLink pop();
        virtual void meld(std::shared_ptr<AbstractActiveSet> other);
        virtual std::shared_ptr<AbstractActiveSetHandle> insert(EdgeLink v);
        virtual void remove(std::shared_ptr<AbstractActiveSetHandle> handle);
        // hacky: decrease key should update the EdgeLink at it to be equivalent to the new_key edgelink, i.e., Node->EdgeLink = new_key
        virtual void decreaseKey(std::shared_ptr<AbstractActiveSetHandle> handle, EdgeLink new_key);
        virtual void steal(std::shared_ptr<AbstractActiveSetHandle> handle);

    private:
        fheap::fibonacci_heap<EdgeLink> fheap_;
    };

    class DummyActiveSet : public AbstractActiveSet
    {
    public:
        virtual unsigned long int size() const;
        virtual bool empty() const;
        virtual EdgeLink pop();
        virtual void meld(std::shared_ptr<AbstractActiveSet> other);
        virtual std::shared_ptr<AbstractActiveSetHandle> insert(EdgeLink v);
        virtual void remove(std::shared_ptr<AbstractActiveSetHandle> handle);
        // hacky: decrease key should update the EdgeLink at it to be equivalent to the new_key edgelink, i.e., Node->EdgeLink = new_key
        virtual void decreaseKey(std::shared_ptr<AbstractActiveSetHandle> handle, EdgeLink new_key);
        virtual void steal(std::shared_ptr<AbstractActiveSetHandle> handle);

    private:
        std::list<EdgeLink> m_elems_;
    };

    class DummyActiveSetHandle : public AbstractActiveSetHandle
    {
    public:
        std::list<EdgeLink>::iterator it;
        DummyActiveSet *in_which_active_set; // cannot use smart ptr here because we need to convert this to smart ptr which is not straightforward
    };

}