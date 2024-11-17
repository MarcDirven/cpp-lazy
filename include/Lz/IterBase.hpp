#pragma once

#ifndef LZ_ITERATOR_HPP
#define LZ_ITERATOR_HPP

#include <iterator>

namespace lz {

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat>
struct IterBase;

template<class Derived, class Reference, class Pointer, class DifferenceType>
struct IterBase<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag> {
private:
    static constexpr bool IsReference = std::is_reference<typename std::remove_const<Reference>::type>::value;

public:
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

    friend bool operator==(const Derived& a, const Derived& b) {
        return static_cast<const Derived&>(a).eq(static_cast<const Derived&>(b));
    }

    friend bool operator!=(const Derived& a, const Derived& b) {
        return !(a == b);
    }
};

template<class Derived, class Reference, class Pointer, class DifferenceType>
struct IterBase<Derived, Reference, Pointer, DifferenceType, std::bidirectional_iterator_tag>
    : public IterBase<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag> {

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

template<class Derived, class Reference, class Pointer, class DifferenceType>
struct IterBase<Derived, Reference, Pointer, DifferenceType, std::random_access_iterator_tag>
    : public IterBase<Derived, Reference, Pointer, DifferenceType, std::bidirectional_iterator_tag> {

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
        return static_cast<const Derived&>(a).difference(static_cast<const Derived&>(b));
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