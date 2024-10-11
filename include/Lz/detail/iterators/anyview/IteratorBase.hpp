#pragma once

#ifndef LZ_ANY_VIEW_ITERATOR_BASE_HPP
#define LZ_ANY_VIEW_ITERATOR_BASE_HPP

#include "Lz/detail/FakePointerProxy.hpp"

#include <iterator>
#include <memory>
#include <type_traits>

namespace lz {
namespace detail {
template<class Reference, class IterCat, class DiffType>
struct IteratorBase;

template<class Reference, class DiffType>
struct IteratorBase<Reference, std::forward_iterator_tag, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference dereference() = 0;

    virtual FakePointerProxy<Reference> arrow() = 0;

    virtual void increment() = 0;

    virtual bool eq(const IteratorBase& other) const = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;
};

template<class Reference, class DiffType>
struct IteratorBase<Reference, std::bidirectional_iterator_tag, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference dereference() = 0;

    virtual FakePointerProxy<Reference> arrow() = 0;

    virtual void increment() = 0;

    virtual void decrement() = 0;

    virtual bool eq(const IteratorBase& other) const = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;
};

template<class Reference, class DiffType>
struct IteratorBase<Reference, std::random_access_iterator_tag, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference dereference() = 0;

    virtual FakePointerProxy<Reference> arrow() = 0;

    virtual void increment() = 0;

    virtual void decrement() = 0;

    virtual void plusIs(DiffType n) = 0;

    virtual DiffType minus(const IteratorBase& other) const = 0;

    virtual bool eq(const IteratorBase& other) const = 0;

    virtual bool lt(const IteratorBase& other) const = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;
};
} // namespace detail
} // namespace lz
#endif // LZ_ANY_VIEW_ITERATOR_BASE_HPP