#pragma once

#ifndef LZ_ANY_VIEW_HELPERS_HPP
#define LZ_ANY_VIEW_HELPERS_HPP

#include "Lz/detail/iterators/anyview/AnyIteratorImpl.hpp"

#include <iterator>

namespace lz {
namespace detail {

template<class T, class IterCat, class Reference, class Pointer, class DiffType>
class IteratorWrapper {
    using AnyIteratorBase = IteratorBase<T, IterCat, Reference, Pointer, DiffType>;
    std::shared_ptr<AnyIteratorBase> _ptr{ nullptr };

public:
    using value_type = T;
    using reference = Reference;
    using pointer = Pointer;
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

    Reference operator*() {
        return **_ptr;
    }

    typename std::add_const<Reference>::type operator*() const {
        return **_ptr;
    }

    Pointer operator->() {
        return _ptr->operator->();
    }

    typename std::add_const<Pointer>::type operator->() const {
        return _ptr->operator->();
    }

    IteratorWrapper& operator++() {
        ++(*_ptr);
        return *this;
    }

    IteratorWrapper operator++(int) {
        IteratorWrapper temp = *this;
        ++(*_ptr);
        return temp;
    }

    IteratorWrapper& operator--() {
        --(*_ptr);
        return *this;
    }

    IteratorWrapper operator--(int) {
        IteratorWrapper temp = *this;
        --(*_ptr);
        return temp;
    }

    bool operator!=(const IteratorWrapper& other) const {
        return *_ptr != *other._ptr;
    }

    bool operator==(const IteratorWrapper& other) const {
        return *_ptr == *other._ptr;
    }

    bool operator<(const IteratorWrapper& other) const {
        return *_ptr < *other._ptr;
    }

    bool operator>(const IteratorWrapper& other) const {
        return *_ptr > *other._ptr;
    }

    bool operator<=(const IteratorWrapper& other) const {
        return *_ptr <= *other._ptr;
    }

    bool operator>=(const IteratorWrapper& other) const {
        return *_ptr >= *other._ptr;
    }

    IteratorWrapper& operator+=(DiffType n) {
        *_ptr += n;
        return *this;
    }

    IteratorWrapper& operator-=(DiffType n) {
        *_ptr -= n;
        return *this;
    }

    DiffType operator-(const IteratorWrapper& other) const {
        return *_ptr - *other._ptr;
    }

    IteratorWrapper operator-(DiffType n) const {
        IteratorWrapper temp = *this;
        temp -= n;
        return temp;
    }

    IteratorWrapper operator+(DiffType n) const {
        IteratorWrapper temp = *this;
        temp += n;
        return temp;
    }

    DiffType operator+(const IteratorWrapper& other) const {
        return *_ptr + *other._ptr;
    }

    Reference operator[](DiffType n) {
        return *(*_ptr + n);
    }

    typename std::add_const<Reference>::type operator[](DiffType n) const {
        return *(*_ptr + n);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_ANY_VIEW_HELPERS_HPP