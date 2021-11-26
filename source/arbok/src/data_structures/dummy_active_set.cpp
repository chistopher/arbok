#include <arbok/data_structures/activeset.h>

namespace arbok
{

    /*unsigned long int DummyActiveSet::size() const
    {
        return static_cast<unsigned long int>(std::size(m_elems_));
    }

    bool DummyActiveSet::empty() const
    {
        return std::empty(m_elems_); 
    } */

    EdgeLink DummyActiveSet::pop()
    {
        // copy element out
        EdgeLink min_elem = *std::min_element(std::begin(m_elems_), std::end(m_elems_));;
        // delete it in list
        m_elems_.erase(std::min_element(std::begin(m_elems_), std::end(m_elems_)));
        return min_elem;
    }

    void DummyActiveSet::meld(std::shared_ptr<AbstractActiveSet> other)
    {
        std::shared_ptr<DummyActiveSet> other_as = std::static_pointer_cast<DummyActiveSet>(other);
        m_elems_.splice(std::end(m_elems_), other_as->m_elems_);
    }

    std::shared_ptr<AbstractActiveSetHandle> DummyActiveSet::insert(EdgeLink v)
    {
        m_elems_.emplace_back(v);

        std::shared_ptr<DummyActiveSetHandle> handle = std::make_shared<DummyActiveSetHandle>();
        handle->it = std::prev(std::end(m_elems_));
        handle->in_which_active_set = this;

        return std::static_pointer_cast<AbstractActiveSetHandle>(handle);
    }

    void DummyActiveSet::remove(std::shared_ptr<AbstractActiveSetHandle> handle)
    {
        std::shared_ptr<DummyActiveSetHandle> handle_ = std::static_pointer_cast<DummyActiveSetHandle>(handle);
        assert(handle_->it); // don't do this on empty handle
        assert(handle_->in_which_active_set != nullptr);
        assert(this == handle_->in_which_active_set);
        m_elems_.erase(handle_->it.value());
    }

    void DummyActiveSet::decreaseKey(std::shared_ptr<AbstractActiveSetHandle> handle, EdgeLink new_key)
    {
        std::shared_ptr<DummyActiveSetHandle> handle_ = std::static_pointer_cast<DummyActiveSetHandle>(handle);
        assert(handle_->it); // don't do this on empty handle
        assert(this == handle_->in_which_active_set);
        *(handle_->it.value()) = new_key;
    }

    void DummyActiveSet::steal(std::shared_ptr<AbstractActiveSetHandle> handle)
    {
        std::shared_ptr<DummyActiveSetHandle> handle_ = std::static_pointer_cast<DummyActiveSetHandle>(handle);
        assert(handle_->it); // don't do this on empty handle
        m_elems_.splice(std::end(m_elems_), handle_->in_which_active_set->m_elems_, handle_->it.value());
        handle_->in_which_active_set = this;
    }

};