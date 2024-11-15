#pragma once

#ifndef LZ_ANY_VIEW_ITERATOR_IMPL_HPP
#define LZ_ANY_VIEW_ITERATOR_IMPL_HPP

#include "IteratorBase.hpp"

namespace lz {
namespace detail {
template<class Iter, class T, class Reference, class IterCat, class DiffType>
class AnyIteratorImpl;

template<class Iter, class T, class Reference, class DiffType>
class AnyIteratorImpl<Iter, T, Reference, std::forward_iterator_tag, DiffType> final
    : public IteratorBase<Reference, std::forward_iterator_tag, DiffType> {
    Iter _iter{};

    static_assert(std::is_same<Reference, decltype(*_iter)>::value,
                  "The iterator operator* returns a different type than template parameter `Reference`. Try adding/removing "
                  "`&` to `Reference`");

    using AnyIteratorBase = IteratorBase<Reference, std::forward_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = FakePointerProxy<reference>;
    using difference_type = DiffType;
    using iterator_category = std::forward_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    reference dereference() override {
        return *_iter;
    }

    reference dereference() const override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    void increment() override {
        ++_iter;
    }

    bool eq(const AnyIteratorBase& other) const override {
        return _iter == static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    std::shared_ptr<AnyIteratorBase> clone() const override {
        return std::make_shared<AnyIteratorImpl>(_iter);
    }
};

template<class Iter, class T, class Reference, class DiffType>
class AnyIteratorImpl<Iter, T, Reference, std::bidirectional_iterator_tag, DiffType> final
    : public IteratorBase<Reference, std::bidirectional_iterator_tag, DiffType> {
    Iter _iter{};

    static_assert(std::is_same<Reference, decltype(*_iter)>::value,
                  "The iterator operator* returns a different type than template parameter `Reference`");

    using AnyIteratorBase = IteratorBase<Reference, std::bidirectional_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = FakePointerProxy<reference>;
    using difference_type = DiffType;
    using iterator_category = std::bidirectional_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    reference dereference() override {
        return *_iter;
    }

    reference dereference() const override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    void increment() override {
        ++_iter;
    }

    void decrement() override {
        --_iter;
    }

    bool eq(const AnyIteratorBase& other) const override {
        return _iter == static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    std::shared_ptr<AnyIteratorBase> clone() const override {
        return std::make_shared<AnyIteratorImpl>(_iter);
    }
};

template<class Iter, class T, class Reference, class DiffType>
class AnyIteratorImpl<Iter, T, Reference, std::random_access_iterator_tag, DiffType> final
    : public IteratorBase<Reference, std::random_access_iterator_tag, DiffType> {
    Iter _iter{};
    static_assert(std::is_same<Reference, decltype(*_iter)>::value,
                  "The iterator operator* returns a different type than template parameter `Reference`");

    using AnyIteratorBase = IteratorBase<Reference, std::random_access_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = FakePointerProxy<Reference>;
    using difference_type = DiffType;
    using iterator_category = std::random_access_iterator_tag;

    AnyIteratorImpl() = default;

    AnyIteratorImpl(Iter iter) : _iter(std::move(iter)) {
    }

    ~AnyIteratorImpl() override = default;

    reference dereference() override {
        return *_iter;
    }

    reference dereference() const override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    void increment() override {
        ++_iter;
    }

    void decrement() override {
        --_iter;
    }

    bool eq(const AnyIteratorBase& other) const override {
        return _iter == static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    void plusIs(DiffType n) override {
        _iter += n;
    }

    virtual DiffType minus(const AnyIteratorBase& other) const override {
        return _iter - static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    bool lt(const AnyIteratorBase& other) const override {
        return _iter < static_cast<const AnyIteratorImpl&>(other)._iter;
    }

    std::shared_ptr<AnyIteratorBase> clone() const override {
        return std::make_shared<AnyIteratorImpl>(_iter);
    }
};
} // namespace detail
} // namespace lz

#endif