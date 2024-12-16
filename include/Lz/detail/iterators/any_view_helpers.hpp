#pragma once

#ifndef LZ_ANY_VIEW_HELPERS_HPP
#define LZ_ANY_VIEW_HELPERS_HPP

#include "Lz/detail/iterators/anyview/iterator_base.hpp"
#include "Lz/iter_base.hpp"

#include <iterator>

namespace lz {
namespace detail {

template<class T, class Reference, class IterCat, class DiffType>
class iterator_wrapper : public iter_base<iterator_wrapper<T, Reference, IterCat, DiffType>, Reference, fake_ptr_proxy<Reference>,
                                          DiffType, IterCat> {

    using any_iter_base = iterator_base<Reference, IterCat, DiffType>;
    std::shared_ptr<any_iter_base> _ptr{ nullptr };

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = IterCat;

    iterator_wrapper() = default;

    iterator_wrapper(const std::shared_ptr<any_iter_base>& ptr) : _ptr(ptr->clone()) {
    }

    iterator_wrapper(std::shared_ptr<any_iter_base>&& ptr) : _ptr(std::move(ptr)) {
    }

    iterator_wrapper(const iterator_wrapper& other) : _ptr(other._ptr->clone()) {
    }

    iterator_wrapper& operator=(const iterator_wrapper& other) {
        if (this != &other) {
            _ptr = other._ptr->clone();
        }
        return *this;
    }

    iterator_wrapper& operator=(std::shared_ptr<any_iter_base>&& ptr) {
        if (_ptr != ptr) {
            _ptr = std::move(ptr);
        }
        return *this;
    }

    typename std::add_const<reference>::type operator*() const {
        return _ptr->dereference();
    }

    pointer operator->() const {
        return _ptr->arrow();
    }

    iterator_wrapper& increment() {
        _ptr->increment();
    }

    iterator_wrapper& decrement() {
        _ptr->decrement();
    }

    bool eq(const iterator_wrapper& other) const {
        return _ptr->eq(*other._ptr);
    }

    void plus_is(const DiffType n) {
        _ptr->plusIs(n);
    }

    DiffType difference(const iterator_wrapper& other) const {
        return _ptr->minus(*other._ptr);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_ANY_VIEW_HELPERS_HPP