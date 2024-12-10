#pragma once

#ifndef LZ_DETAIL_VARIANT_HPP
#define LZ_DETAIL_VARIANT_HPP

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
    static constexpr std::size_t npos = -1;

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
auto get(Variant<U, V>&& t) -> decltype(std::move(t.get<T>())) {
    return t.get<T>();
}

template<class T, class U, class V>
auto get_if(const Variant<U, V>* t) -> decltype(t->get_if<T>()) {
    return t->get_if<T>();
}

#endif // !defined(__cpp_lib_variant) && !defined(LZ_HAS_CXX_17)

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_VARIANT_HPP