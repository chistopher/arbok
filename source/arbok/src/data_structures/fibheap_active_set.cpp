#include <arbok/data_structures/activeset.h>

namespace arbok
{

    /*unsigned long int FibHeapActiveSet::size() const
    {
        return static_cast<unsigned long int>(fheap_.size());
    }

    bool FibHeapActiveSet::empty() const
    {
        return fheap_.empty();
    } */

    EdgeLink FibHeapActiveSet::pop()
    {
        return fheap_.pop();
    }

    void FibHeapActiveSet::meld(std::shared_ptr<AbstractActiveSet> other)
    {
        std::shared_ptr<FibHeapActiveSet> other_as = std::static_pointer_cast<FibHeapActiveSet>(other);
        fheap_.meld(std::move(other_as.get()->fheap_));
        // TODO: Think about whether we also somehow need to invalide other_as, not only its fheap
    }

    std::shared_ptr<AbstractActiveSetHandle> FibHeapActiveSet::insert(EdgeLink v)
    {
        fheap::fibonacci_heap<EdgeLink>::handle h = fheap_.push(v);

        std::shared_ptr<FibHeapActiveSetHandle> handle = std::make_shared<FibHeapActiveSetHandle>();
        handle->handle = h;

        return std::static_pointer_cast<AbstractActiveSetHandle>(handle);
    }

    void FibHeapActiveSet::remove(std::shared_ptr<AbstractActiveSetHandle> handle)
    {
        std::shared_ptr<FibHeapActiveSetHandle> handle_ = std::static_pointer_cast<FibHeapActiveSetHandle>(handle);
        assert(handle_->handle); // don't do this on empty handle
        fheap::fibonacci_heap<EdgeLink>::handle &fheap_handle = handle_->handle.value();
        fheap_.remove(fheap_handle);
    }

    void FibHeapActiveSet::decreaseKey(std::shared_ptr<AbstractActiveSetHandle> handle, EdgeLink new_key)
    {
        std::shared_ptr<FibHeapActiveSetHandle> handle_ = std::static_pointer_cast<FibHeapActiveSetHandle>(handle);
        assert(handle_->handle); // don't do this on empty handle
        fheap::fibonacci_heap<EdgeLink>::handle &fheap_handle = handle_->handle.value();

        fheap_.decrease_key(fheap_handle, new_key);
    }

    void FibHeapActiveSet::unsafeSetKey(std::shared_ptr<AbstractActiveSetHandle> handle, EdgeLink new_key)
    {
        std::shared_ptr<FibHeapActiveSetHandle> handle_ = std::static_pointer_cast<FibHeapActiveSetHandle>(handle);
        assert(handle_->handle); // don't do this on empty handle
        fheap::fibonacci_heap<EdgeLink>::handle &fheap_handle = handle_->handle.value();

        fheap_.unsafe_setkey(fheap_handle, new_key);
    }

    void FibHeapActiveSet::steal(std::shared_ptr<AbstractActiveSetHandle> handle)
    {
        std::shared_ptr<FibHeapActiveSetHandle> handle_ = std::static_pointer_cast<FibHeapActiveSetHandle>(handle);
        assert(handle_->handle); // don't do this on empty handle
        fheap::fibonacci_heap<EdgeLink>::handle &fheap_handle = handle_->handle.value();

        fheap_.steal(fheap_handle);
    }

};
