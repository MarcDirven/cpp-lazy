#pragma once

#ifndef LZ_ANY_VIEW_ITERATOR_BASE_HPP
#define LZ_ANY_VIEW_ITERATOR_BASE_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"

#include <iterator>
#include <memory>
#include <type_traits>

namespace lz {
namespace detail {
template<class Reference, class IterCat, class DiffType>
struct iterator_base;

template<class Reference, class DiffType>
struct iterator_base<Reference, std::forward_iterator_tag, DiffType> {
    virtual ~iterator_base() = default;

    virtual Reference dereference() = 0;

    virtual Reference dereference() const = 0;

    virtual fake_ptr_proxy<Reference> arrow() = 0;

    virtual void increment() = 0;

    virtual bool eq(const iterator_base& other) const = 0;

    virtual std::shared_ptr<iterator_base> clone() const = 0;
};

template<class Reference, class DiffType>
struct iterator_base<Reference, std::bidirectional_iterator_tag, DiffType> {
    virtual ~iterator_base() = default;

    virtual Reference dereference() = 0;

    virtual Reference dereference() const = 0;

    virtual fake_ptr_proxy<Reference> arrow() = 0;

    virtual void increment() = 0;

    virtual void decrement() = 0;

    virtual bool eq(const iterator_base& other) const = 0;

    virtual std::shared_ptr<iterator_base> clone() const = 0;
};

template<class Reference, class DiffType>
struct iterator_base<Reference, std::random_access_iterator_tag, DiffType> {
    virtual ~iterator_base() = default;

    virtual Reference dereference() = 0;

    virtual Reference dereference() const = 0;

    virtual fake_ptr_proxy<Reference> arrow() = 0;

    virtual void increment() = 0;

    virtual void decrement() = 0;

    virtual void plus_is(DiffType n) = 0;

    virtual DiffType minus(const iterator_base& other) const = 0;

    virtual bool eq(const iterator_base& other) const = 0;

    virtual bool lt(const iterator_base& other) const = 0;

    virtual std::shared_ptr<iterator_base> clone() const = 0;
};
} // namespace detail
} // namespace lz
#endif // LZ_ANY_VIEW_ITERATOR_BASE_HPP