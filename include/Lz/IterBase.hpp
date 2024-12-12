#pragma once

#ifndef LZ_ITERATOR_HPP
#define LZ_ITERATOR_HPP

#include "Lz/detail/Traits.hpp"

#include <iterator>

namespace lz {
struct DefaultSentinel {};

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class S = Derived>
struct IterBase;

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct IterBase<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag, S> {
public:
    using iterator_category = std::forward_iterator_tag;
    using Sentinel = S;

    Derived& operator++() {
        static_cast<Derived&>(*this).increment();
        return static_cast<Derived&>(*this);
    }

    Derived operator++(int) {
        Derived copy = static_cast<Derived&>(*this);
        static_cast<Derived&>(*this).increment();
        return copy;
    }

    Reference operator*() const {
        return static_cast<const Derived&>(*this).dereference();
    }

    Reference operator*() {
        return static_cast<Derived&>(*this).dereference();
    }

    Pointer operator->() const {
        return static_cast<const Derived&>(*this).arrow();
    }

    template<class D = Derived, class = detail::EnableIf<!std::is_same<D, Sentinel>::value>>
    bool operator==(const Sentinel& b) const {
        return static_cast<const Derived&>(*this).eq(b);
    }

    template<class D = Derived, class = detail::EnableIf<!std::is_same<D, Sentinel>::value>>
    bool operator!=(const Sentinel& b) const {
        return !(*this == b);
    }

    bool operator==(const Derived& b) const {
        return static_cast<const Derived&>(*this).eq(b);
    }

    bool operator!=(const Derived& b) const {
        return !(*this == b);
    }
};

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class S>
detail::EnableIf<!std::is_same<S, Derived>::value, bool>
operator==(const S& a, const IterBase<Derived, Reference, Pointer, DifferenceType, IterCat, S>& b) {
    return b == a;
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class S>
detail::EnableIf<!std::is_same<S, Derived>::value, bool>
operator!=(const S& a, const IterBase<Derived, Reference, Pointer, DifferenceType, IterCat, S>& b) {
    return !(a == b);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct IterBase<Derived, Reference, Pointer, DifferenceType, std::bidirectional_iterator_tag, S>
    : public IterBase<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag, S> {
    using iterator_category = std::bidirectional_iterator_tag;
    using Sentinel = S;

    Derived& operator--() {
        static_cast<Derived&>(*this).decrement();
        return static_cast<Derived&>(*this);
    }

    Derived operator--(int) {
        Derived copy = *static_cast<Derived*>(this);
        static_cast<Derived&>(*this)->decrement();
        return copy;
    }
};

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct IterBase<Derived, Reference, Pointer, DifferenceType, std::random_access_iterator_tag, S>
    : public IterBase<Derived, Reference, Pointer, DifferenceType, std::bidirectional_iterator_tag, S> {
    using iterator_category = std::random_access_iterator_tag;
    using Sentinel = S;

    Derived& operator+=(const DifferenceType n) {
        static_cast<Derived&>(*this).plusIs(n);
        return static_cast<Derived&>(*this);
    }

    Derived operator+(const DifferenceType n) const {
        auto tmp = static_cast<const Derived&>(*this);
        tmp += n;
        return tmp;
    }

    friend DifferenceType operator-(const Derived& a, const Derived& b) {
        return a.difference(b);
    }

    Derived operator-(const DifferenceType n) const {
        Derived temp = static_cast<const Derived&>(*this);
        temp -= n;
        return temp;
    }

    Derived& operator-=(const DifferenceType n) {
        *this += (-n);
        return *static_cast<Derived*>(this);
    }

    Reference operator[](DifferenceType n) const {
        return *(*this + n);
    }

    friend bool operator<(const Derived& a, const Derived& b) {
        return b - a > 0;
    }

    friend bool operator>(const Derived& a, const Derived& b) {
        return b < a;
    }

    friend bool operator<=(const Derived& a, const Derived& b) {
        return !(b < a);
    }

    friend bool operator>=(const Derived& a, const Derived& b) {
        return !(a < b);
    }
};
} // namespace lz
#endif // LZ_ITERATOR_HPP