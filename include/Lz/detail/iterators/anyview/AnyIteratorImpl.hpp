#pragma once

#ifndef LZ_ANY_VIEW_ITERATOR_IMPL_HPP
#define LZ_ANY_VIEW_ITERATOR_IMPL_HPP

#include "IteratorBase.hpp"

namespace lz {
namespace detail {
template<class Iter, class T, class IterCat, class Reference, class Pointer, class DiffType>
class AnyIteratorImpl;

template<class Iter, class T, class Reference, class Pointer, class DiffType>
class AnyIteratorImpl<Iter, T, std::input_iterator_tag, Reference, Pointer, DiffType> final
    : public IteratorBase<T, std::input_iterator_tag, Reference, Pointer, DiffType> {
    Iter _iter{};

    using AnyIteratorBase = IteratorBase<T, std::input_iterator_tag, Reference, Pointer, DiffType>;

public:
    using value_type = T;
    using refernce = Reference;
    using pointer = Pointer;
    using difference_type = DiffType;
    using iterator_category = std::input_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    Reference operator*() override {
        return *_iter;
    }

    typename std::add_const<Reference>::type operator*() const override {
        return *_iter;
    }

    Pointer operator->() override {
        return _iter.operator->();
    }

    typename std::add_const<Pointer>::type operator->() const override {
        return _iter.operator->();
    }

    AnyIteratorBase& operator++() override {
        ++_iter;
        return *this;
    }

    bool operator!=(const AnyIteratorBase& other) const override {
        return _iter != static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator==(const AnyIteratorBase& other) const override {
        return _iter == static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    std::shared_ptr<AnyIteratorBase> clone() const override {
        return std::make_shared<AnyIteratorImpl>(_iter);
    }
};

template<class Iter, class T, class Reference, class Pointer, class DiffType>
class AnyIteratorImpl<Iter, T, std::output_iterator_tag, Reference, Pointer, DiffType> final
    : public IteratorBase<T, std::output_iterator_tag, Reference, Pointer, DiffType> {
    Iter _iter{};

    using AnyIteratorBase = IteratorBase<T, std::output_iterator_tag, Reference, Pointer, DiffType>;

public:
    using value_type = T;
    using refernce = Reference;
    using pointer = Pointer;
    using difference_type = DiffType;
    using iterator_category = std::output_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    Reference operator*() override {
        return *_iter;
    }

    AnyIteratorBase& operator++() override {
        ++_iter;
        return *this;
    }
};

template<class Iter, class T, class Reference, class Pointer, class DiffType>
class AnyIteratorImpl<Iter, T, std::forward_iterator_tag, Reference, Pointer, DiffType> final
    : public IteratorBase<T, std::forward_iterator_tag, Reference, Pointer, DiffType> {
    Iter _iter{};

    using AnyIteratorBase = IteratorBase<T, std::forward_iterator_tag, Reference, Pointer, DiffType>;

public:
    using value_type = T;
    using refernce = Reference;
    using pointer = Pointer;
    using difference_type = DiffType;
    using iterator_category = std::forward_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    Reference operator*() override {
        return *_iter;
    }

    typename std::add_const<Reference>::type operator*() const override {
        return *_iter;
    }

    Pointer operator->() override {
        return _iter.operator->();
    }

    typename std::add_const<Pointer>::type operator->() const override {
        return _iter.operator->();
    }

    AnyIteratorBase& operator++() override {
        ++_iter;
        return *this;
    }

    bool operator!=(const AnyIteratorBase& other) const override {
        return _iter != static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator==(const AnyIteratorBase& other) const override {
        return _iter == static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    std::shared_ptr<AnyIteratorBase> clone() const override {
        return std::make_shared<AnyIteratorImpl>(_iter);
    }
};

template<class Iter, class T, class Reference, class Pointer, class DiffType>
class AnyIteratorImpl<Iter, T, std::bidirectional_iterator_tag, Reference, Pointer, DiffType> final
    : public IteratorBase<T, std::bidirectional_iterator_tag, Reference, Pointer, DiffType> {
    Iter _iter{};

    using AnyIteratorBase = IteratorBase<T, std::bidirectional_iterator_tag, Reference, Pointer, DiffType>;

public:
    using value_type = T;
    using refernce = Reference;
    using pointer = Pointer;
    using difference_type = DiffType;
    using iterator_category = std::bidirectional_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    Reference operator*() override {
        return *_iter;
    }

    typename std::add_const<Reference>::type operator*() const override {
        return *_iter;
    }

    Pointer operator->() override {
        return _iter.operator->();
    }

    typename std::add_const<Pointer>::type operator->() const override {
        return _iter.operator->();
    }

    AnyIteratorBase& operator++() override {
        ++_iter;
        return *this;
    }

    AnyIteratorBase& operator--() override {
        --_iter;
        return *this;
    }

    bool operator!=(const AnyIteratorBase& other) const override {
        return _iter != static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator==(const AnyIteratorBase& other) const override {
        return _iter == static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    std::shared_ptr<AnyIteratorBase> clone() const override {
        return std::make_shared<AnyIteratorImpl>(_iter);
    }
};

template<class Iter, class T, class Reference, class Pointer, class DiffType>
class AnyIteratorImpl<Iter, T, std::random_access_iterator_tag, Reference, Pointer, DiffType> final
    : public IteratorBase<T, std::random_access_iterator_tag, Reference, Pointer, DiffType> {
    Iter _iter{};

    using AnyIteratorBase = IteratorBase<T, std::random_access_iterator_tag, Reference, Pointer, DiffType>;

public:
    using value_type = T;
    using refernce = Reference;
    using pointer = Pointer;
    using difference_type = DiffType;
    using iterator_category = std::random_access_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    Reference operator*() override {
        return *_iter;
    }

    typename std::add_const<Reference>::type operator*() const override {
        return *_iter;
    }

    Pointer operator->() override {
        return _iter.operator->();
    }

    typename std::add_const<Pointer>::type operator->() const override {
        return _iter.operator->();
    }

    AnyIteratorBase& operator++() override {
        ++_iter;
        return *this;
    }

    AnyIteratorBase& operator--() override {
        --_iter;
        return *this;
    }

    bool operator!=(const AnyIteratorBase& other) const override {
        return _iter != static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator==(const AnyIteratorBase& other) const override {
        return _iter == static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    std::shared_ptr<AnyIteratorBase> clone() const override {
        return std::make_shared<AnyIteratorImpl>(_iter);
    }

    AnyIteratorBase& operator+=(DiffType n) override {
        _iter += n;
        return *this;
    }

    AnyIteratorBase& operator-=(DiffType n) override {
        _iter -= n;
        return *this;
    }

    virtual DiffType operator-(const AnyIteratorBase& other) const override {
        return _iter - static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator<(const AnyIteratorBase& other) const override {
        return _iter < static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator>(const AnyIteratorBase& other) const override {
        return _iter > static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator<=(const AnyIteratorBase& other) const override {
        return _iter <= static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool operator>=(const AnyIteratorBase& other) const override {
        return _iter >= static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    Reference operator[](DiffType offset) override {
        return _iter[offset];
    }

    typename std::add_const<Reference>::type operator[](DiffType offset) const override {
        return _iter[offset];
    }
};
} // namespace detail
} // namespace lz

#endif