#pragma once

#ifndef LZ_ANY_VIEW_ITERATOR_BASE_HPP
#define LZ_ANY_VIEW_ITERATOR_BASE_HPP

#include <iterator>
#include <memory>
#include <type_traits>

namespace lz {
namespace detail {
template<class T, class IterCat, class Reference, class Pointer, class DiffType>
struct IteratorBase;

template<class T, class Reference, class Pointer, class DiffType>
struct IteratorBase<T, std::input_iterator_tag, Reference, Pointer, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference operator*() = 0;

    virtual typename std::add_const<Reference>::type operator*() const = 0;

    virtual Pointer operator->() = 0;

    virtual typename std::add_const<Pointer>::type operator->() const = 0;

    virtual IteratorBase& operator++() = 0;

    virtual bool operator!=(const IteratorBase& other) const = 0;

    virtual bool operator==(const IteratorBase& other) const = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;
};

template<class T, class Reference, class Pointer, class DiffType>
struct IteratorBase<T, std::output_iterator_tag, Reference, Pointer, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference operator*() = 0;

    virtual IteratorBase& operator++() = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;
};

template<class T, class Reference, class Pointer, class DiffType>
struct IteratorBase<T, std::forward_iterator_tag, Reference, Pointer, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference operator*() = 0;

    virtual typename std::add_const<Reference>::type operator*() const = 0;

    virtual Pointer operator->() = 0;

    virtual typename std::add_const<Pointer>::type operator->() const = 0;

    virtual IteratorBase& operator++() = 0;

    virtual bool operator!=(const IteratorBase& other) const = 0;

    virtual bool operator==(const IteratorBase& other) const = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;
};

template<class T, class Reference, class Pointer, class DiffType>
struct IteratorBase<T, std::bidirectional_iterator_tag, Reference, Pointer, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference operator*() = 0;

    virtual typename std::add_const<Reference>::type operator*() const = 0;

    virtual Pointer operator->() = 0;

    virtual typename std::add_const<Pointer>::type operator->() const = 0;

    virtual IteratorBase& operator++() = 0;

    virtual IteratorBase& operator--() = 0;

    virtual bool operator!=(const IteratorBase& other) const = 0;

    virtual bool operator==(const IteratorBase& other) const = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;
};

template<class T, class Reference, class Pointer, class DiffType>
struct IteratorBase<T, std::random_access_iterator_tag, Reference, Pointer, DiffType> {
    virtual ~IteratorBase() = default;

    virtual Reference operator*() = 0;

    virtual typename std::add_const<Reference>::type operator*() const = 0;

    virtual Pointer operator->() = 0;

    virtual typename std::add_const<Pointer>::type operator->() const = 0;

    virtual IteratorBase& operator++() = 0;

    virtual IteratorBase& operator--() = 0;

    virtual IteratorBase& operator+=(DiffType n) = 0;

    virtual IteratorBase& operator-=(DiffType n) = 0;

    virtual DiffType operator-(const IteratorBase& other) const = 0;

    virtual bool operator!=(const IteratorBase& other) const = 0;

    virtual bool operator==(const IteratorBase& other) const = 0;

    virtual bool operator<(const IteratorBase& other) const = 0;

    virtual bool operator>(const IteratorBase& other) const = 0;

    virtual bool operator<=(const IteratorBase& other) const = 0;

    virtual bool operator>=(const IteratorBase& other) const = 0;

    virtual std::shared_ptr<IteratorBase> clone() const = 0;

    virtual Reference operator[](DiffType n) = 0;

    virtual typename std::add_const<Reference>::type operator[](DiffType n) const = 0;
};
} // namespace detail
} // namespace lz
#endif // LZ_ANY_VIEW_ITERATOR_BASE_HPP