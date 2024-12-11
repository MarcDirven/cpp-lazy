#pragma once

#ifndef LZ_DETAIL_COMMON_ITERATOR_HPP
#define LZ_DETAIL_COMMON_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

#if defined(__cpp_lib_variant) && defined(LZ_HAS_CXX_17)

#include <variant>

namespace lz {
namespace detail {
template<class T, class T2>
using Variant = std::variant<T, T2>;

} // namespace detail
} // namespace lz

#endif

namespace lz {
namespace detail {

#if !defined(__cpp_lib_variant) && !defined(LZ_HAS_CXX_17)
template<class T, class T2>
class Variant {
    enum class State {
        None,
        T,
        T2,
    } state{};

    union Types {
        T t;
        T2 t2;

        Types() {
        }

        ~Types() {
        }
    } variant;

    template<class U, class V>
    void reconstruct(State s, U&& thisVariant, V&& otherVariantType) {
        this->~Variant();
        state = s;
        ::new (std::addressof(thisVariant)) V(std::forward<V>(otherVariantType));
    }

    template<class V, class V2>
    void construct(V&& otherVariantType, V2&& otherVariantType2) {
        switch (state) {
        case State::T:
            ::new (std::addressof(variant.t)) T(std::forward<V>(otherVariantType));
            break;
        case State::T2:
            ::new (std::addressof(variant.t2)) T2(std::forward<V2>(otherVariantType2));
            break;
        }
    }

public:
    static constexpr std::size_t npos{ static_cast<std::size_t>(-1) };

    Variant() : state(State::None) {
    }

    Variant(const T& t) : state(State::T) {
        ::new (std::addressof(variant.t)) T(t);
    }

    Variant(const T2& t2) : state(State::T2) {
        ::new (std::addressof(variant.t2)) T2(t2);
    }

    Variant(T&& t) : state(State::T) {
        ::new (std::addressof(variant.t)) T(std::move(t));
    }

    Variant(T2&& t2) : state(State::T2) {
        ::new (std::addressof(variant.t2)) T2(std::move(t2));
    }

    Variant(const Variant& other) : state(other.state) {
        construct(other.variant.t, other.variant.t2);
    }

    Variant(Variant&& other) : state(other.state) {
        other.state = State::None;
        construct(std::move(other.variant.t), std::move(other.variant.t2));
    }

    Variant& operator=(const T& t) {
        reconstruct(State::T, variant.t, t);
        return *this;
    }

    Variant& operator=(const T2& t2) {
        reconstruct(State::T2, variant.t2, t2);
        return *this;
    }

    Variant& operator=(T&& t) {
        reconstruct(State::T, variant.t, std::move(t));
        return *this;
    }

    Variant& operator=(T2&& t2) {
        reconstruct(State::T2, variant.t2, std::move(t2));
        return *this;
    }

    Variant& operator=(const Variant& other) {
        this->~Variant();
        state = other.state;
        construct(other.variant.t, other.variant.t2);
        return *this;
    }

    Variant& operator=(Variant&& other) {
        this->~Variant();
        state = other.state;
        construct(std::move(other.variant.t), std::move(other.variant.t2));
        return *this;
    }

    template<class U>
    const U* get_if() const {
        if (state == State::T && std::is_same<T, U>::value) {
            return reinterpret_cast<const U*>(std::addressof(variant.t));
        }
        if (state == State::T2 && std::is_same<T2, U>::value) {
            return reinterpret_cast<const U*>(std::addressof(variant.t2));
        }
        return nullptr;
    }

    template<class U>
    U* get_if() {
        return const_cast<U*>(static_cast<const Variant&>(*this).get_if<U>());
    }

    template<class U>
    const U& get() const {
        if (const auto* u = get_if<U>()) {
            return *u;
        }
        throw std::runtime_error("Bad variant access");
    }

    template<class U>
    U& get() {
        return const_cast<U&>(static_cast<const Variant&>(*this).get<U>());
    }

    ~Variant() {
        switch (state) {
        case State::T:
            variant.t.~T();
            break;
        case State::T2:
            variant.t2.~T2();
            break;
        }
        state = State::None;
    }
};

template<class T, class U, class V>
auto get(const Variant<U, V>& t) -> decltype(t.get<T>()) {
    return t.get<T>();
}

template<class T, class U, class V>
auto get(Variant<U, V>& t) -> decltype(t.get<T>()) {
    return t.get<T>();
}

template<class T, class U, class V>
auto get_if(const Variant<U, V>* t) -> decltype(t->get_if<T>()) {
    return t->get_if<T>();
}

template<class T, class U, class V>
auto get_if(Variant<U, V>* t) -> decltype(t->get_if<T>()) {
    return t->get_if<T>();
}

#endif

template<class Iterator, class S>
class CommonIterator : public IterBase<CommonIterator<Iterator, S>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                                       DiffType<Iterator>, IterCat<Iterator>, CommonIterator<Iterator, S>> {
    Variant<Iterator, S> _data;
    static_assert(!std::is_same<Iterator, S>::value, "Iterator and Sentinel must be different types");

    using Traits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename Traits::iterator_category;
    using value_type = typename Traits::value_type;
    using difference_type = typename Traits::difference_type;
    using reference = typename Traits::reference;
    using pointer = FakePointerProxy<reference>;

    CommonIterator() = default;

    CommonIterator(const Iterator& iter) : _data(iter) {
    }

    CommonIterator(const S& sent) : _data(sent) {
    }

    CommonIterator& operator=(const Iterator& iter) {
        _data = iter;
        return *this;
    }

    CommonIterator& operator=(const S& sent) {
        _data = sent;
        return *this;
    }

    reference dereference() const {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot get pointer from a sentinel");
        return **iter;
    }

    FakePointerProxy<reference> arrow() const {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot get pointer from a sentinel");
        return FakePointerProxy<decltype(**this)>(**iter);
    }

    void increment() {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot increment a sentinel");
        ++(*iter);
    }

    void decrement() {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot decrement a sentinel");
        --(*iter);
    }

    bool eq(const CommonIterator& rhs) const {
#ifdef __cpp_lib_variant
        using std::get_if;
        using std::get;
#endif

        auto&& lhsIter = get_if<Iterator>(&_data);
        auto&& rhsIter = get_if<Iterator>(&rhs._data);

        if (lhsIter && rhsIter) {
            return *lhsIter == *rhsIter;
        }
        if (lhsIter /* && !rhsIter */) {
            return *lhsIter == get<S>(rhs._data);
        }
        if (rhsIter /* && !lhsIter */) {
            return get<S>(_data) == *rhsIter;
        }
        return true;
    }

    void plusIs(const difference_type n) {
#ifdef __cpp_lib_variant
        using std::get_if;
#endif
        auto&& iter = get_if<Iterator>(&_data);
        LZ_ASSERT(iter != nullptr, "Cannot increment sentinel");
        *iter += n;
    }

    difference_type difference(const CommonIterator& rhs) const {
#ifdef __cpp_lib_variant
        using std::get_if;
        using std::get;
#endif

        auto&& lhsIter = get_if<Iterator>(&_data);
        auto&& rhsIter = get_if<Iterator>(&rhs._data);
        LZ_ASSERT(lhsIter && rhsIter, "Cannot get difference between a sentinel and an iterator");
        return *lhsIter - *rhsIter;
    }
};
} // namespace detail
} // namespace lz

#endif