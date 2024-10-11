#pragma once

#ifndef LZ_ANY_VIEW_HELPERS_HPP
#define LZ_ANY_VIEW_HELPERS_HPP

#include "Lz/detail/iterators/anyview/IteratorBase.hpp"

#include <iterator>

namespace lz {
namespace detail {

template<class T, class Reference, class IterCat, class DiffType>
class IteratorWrapper {
    using AnyIteratorBase = IteratorBase<Reference, IterCat, DiffType>;
    std::shared_ptr<AnyIteratorBase> _ptr{ nullptr };

public:
    using value_type = T;
    using reference = Reference;
    using pointer = FakePointerProxy<reference>;
    using difference_type = DiffType;
    using iterator_category = IterCat;

    IteratorWrapper() = default;

    IteratorWrapper(const std::shared_ptr<AnyIteratorBase>& ptr) : _ptr(ptr->clone()) {
    }

    IteratorWrapper(std::shared_ptr<AnyIteratorBase>&& ptr) : _ptr(std::move(ptr)) {
    }

    IteratorWrapper(const IteratorWrapper& other) : _ptr(other._ptr->clone()) {
    }

    IteratorWrapper& operator=(const IteratorWrapper& other) {
        if (this != &other) {
            _ptr = other._ptr->clone();
        }
        return *this;
    }

    IteratorWrapper& operator=(std::shared_ptr<AnyIteratorBase>&& ptr) {
        if (_ptr != ptr) {
            _ptr = std::move(ptr);
        }
        return *this;
    }

    reference operator*() {
        return _ptr->dereference();
    }

    typename std::add_const<reference>::type operator*() const {
        return _ptr->dereference();
    }

    pointer operator->() {
        return _ptr->arrow();
    }

    pointer operator->() const {
        return _ptr->arrow();
    }

    IteratorWrapper& operator++() {
        _ptr->increment();
        return *this;
    }

    IteratorWrapper operator++(int) {
        IteratorWrapper temp = *this;
        ++*this;
        return temp;
    }

    IteratorWrapper& operator--() {
        _ptr->decrement();
        return *this;
    }

    IteratorWrapper operator--(int) {
        IteratorWrapper temp = *this;
        --*this;
        return temp;
    }

    bool operator!=(const IteratorWrapper& other) const {
        return !(*this == other);
    }

    bool operator==(const IteratorWrapper& other) const {
        return _ptr->eq(*other._ptr);
    }

    bool operator<(const IteratorWrapper& other) const {
        return _ptr->lt(*other._ptr);
    }

    bool operator>(const IteratorWrapper& other) const {
        return other < *this;
    }

    bool operator<=(const IteratorWrapper& other) const {
        return !(other < *this);
    }

    bool operator>=(const IteratorWrapper& other) const {
        return !(*this < other);
    }

    IteratorWrapper& operator+=(const DiffType n) {
        _ptr->plusIs(n);
        return *this;
    }

    IteratorWrapper& operator-=(const DiffType n) {
        *this += (-n);
        return *this;
    }

    DiffType operator-(const IteratorWrapper& other) const {
        return _ptr->minus(*other._ptr);
    }

    IteratorWrapper operator-(const DiffType n) const {
        IteratorWrapper temp = *this;
        temp -= n;
        return temp;
    }

    IteratorWrapper operator+(const DiffType n) const {
        IteratorWrapper temp = *this;
        temp._ptr->plusIs(n);
        return temp;
    }

    Reference operator[](DiffType n) {
        return *(*this + n);
    }

    typename std::add_const<Reference>::type operator[](const DiffType n) const {
        return *(*this + n);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_ANY_VIEW_HELPERS_HPP