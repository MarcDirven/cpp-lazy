#pragma once

#ifndef LZ_DETAIL_VARIANT_HPP
#define LZ_DETAIL_VARIANT_HPP

#if defined(__cpp_lib_variant) && defined(LZ_HAS_CXX_17)

#include <variant>

namespace lz {
namespace detail {
template<class T, class T2>
using variant = std::variant<T, T2>;

} // namespace detail
} // namespace lz

#else

namespace lz {
namespace detail {

template<class T, class T2>
class variant {
    enum class state {
        none,
        t,
        t2,
    } _state{};

    union types {
        T _t;
        T2 _t2;

        types() {
        }

        ~types() {
        }
    } _variant;

    template<class U, class V>
    void reconstruct(state s, U&& this_variant, V&& other_variant_type) {
        this->~variant();
        _state = s;
        ::new (std::addressof(this_variant)) V(std::forward<V>(other_variant_type));
    }

    template<class V, class V2>
    void construct(V&& other_variant_type, V2&& other_variant_type2) {
        switch (_state) {
        case state::t:
            ::new (std::addressof(variant._t)) T(std::forward<V>(other_variant_type));
            break;
        case state::t2:
            ::new (std::addressof(variant._t2)) T2(std::forward<V2>(other_variant_type2));
            break;
        }
    }

public:
    static constexpr std::size_t npos = -1;

    variant() : _state(state::none) {
    }

    variant(const T& t) : _state(state::t) {
        ::new (std::addressof(_variant._t)) T(t);
    }

    variant(const T2& t2) : _state(state::t2) {
        ::new (std::addressof(_variant._t2)) T2(t2);
    }

    variant(T&& t) : _state(state::t) {
        ::new (std::addressof(_variant._t)) T(std::move(t));
    }

    variant(T2&& t2) : _state(state::t2) {
        ::new (std::addressof(_variant._t2)) T2(std::move(t2));
    }

    variant(const variant& other) : _state(other.state) {
        construct(other._variant._t, other._variant._t2);
    }

    variant(variant&& other) : _state(other.state) {
        other._state = state::none;
        construct(std::move(other._variant._t), std::move(other._variant._t2));
    }

    variant& operator=(const T& t) {
        reconstruct(state::t, _variant._t, t);
        return *this;
    }

    variant& operator=(const T2& t2) {
        reconstruct(state::t2, _variant._t2, t2);
        return *this;
    }

    variant& operator=(T&& t) {
        reconstruct(state::t, _variant._t, std::move(t));
        return *this;
    }

    variant& operator=(T2&& t2) {
        reconstruct(state::t2, _variant._t2, std::move(t2));
        return *this;
    }

    variant& operator=(const variant& other) {
        this->~variant();
        _state = other._state;
        construct(other._variant._t, other._variant._t2);
        return *this;
    }

    variant& operator=(variant&& other) {
        this->~variant();
        _state = other._state;
        construct(std::move(other._variant._t), std::move(other._variant._t2));
        return *this;
    }

    template<class U>
    const U* get_if() const {
        if (_state == state::t && std::is_same<T, U>::value) {
            return reinterpret_cast<const U*>(std::addressof(_variant._t));
        }
        if (_state == state::t2 && std::is_same<T2, U>::value) {
            return reinterpret_cast<const U*>(std::addressof(_variant._t2));
        }
        return nullptr;
    }

    template<class U>
    U* get_if() {
        return const_cast<U*>(static_cast<const variant&>(*this).get_if<U>());
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
        return const_cast<U&>(static_cast<const variant&>(*this).get<U>());
    }

    ~variant() {
        switch (_state) {
        case state::t:
            variant._t.~T();
            break;
        case state::t2:
            variant._t2.~T2();
            break;
        }
        _state = state::none;
    }
};

template<class T, class U, class V>
auto get(const variant<U, V>& t) -> decltype(t.get<T>()) {
    return t.get<T>();
}

template<class T, class U, class V>
auto get(variant<U, V>& t) -> decltype(t.get<T>()) {
    return t.get<T>();
}

template<class T, class U, class V>
auto get_if(const variant<U, V>* t) -> decltype(t->get_if<T>()) {
    return t->get_if<T>();
}

template<class T, class U, class V>
auto get_if(variant<U, V>* t) -> decltype(t->get_if<T>()) {
    return t->get_if<T>();
}

#endif // !defined(__cpp_lib_variant) && !defined(LZ_HAS_CXX_17)

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_VARIANT_HPP