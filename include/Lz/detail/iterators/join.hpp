#pragma once

#ifndef LZ_JOIN_ITERATOR_HPP
#define LZ_JOIN_ITERATOR_HPP

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iter_Base.hpp"

#if defined(LZ_STANDALONE)
#ifdef LZ_HAS_FORMAT
#include <format>
#else
#include <sstream>

#endif // LZ_HAS_FORMAT
#endif // LZ_STANDALONE

namespace lz {
namespace detail {
#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
template<class T>
enable_if<!std::is_arithmetic<T>::value, std::string> to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<class T>
enable_if<std::is_arithmetic<T>::value, std::string> to_string(const T value) {
    char buff[safe_buffer_size<T>::value]{};
    to_string_from_buff(value, buff);
    return buff;
}

inline std::string to_string(const bool value) {
    char buff[safe_buffer_size<bool>::value]{};
    to_string_from_buff(value, buff);
    return buff;
}
#endif // defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))

template<class Iterator, class S>
class join_iterator
    : public iter_base<
          join_iterator<Iterator, S>,
          conditional<std::is_same<std::string, value_type<Iterator>>::value, ref<Iterator>, std::string>,
          fake_ptr_proxy<conditional<std::is_same<std::string, value_type<Iterator>>::value, ref<Iterator>, std::string>>,
          diff_type<Iterator>, iter_cat<Iterator>, sentinel_selector<iter_cat<Iterator>, join_iterator<Iterator, S>, S>> {

    using iter < raits = std::iterator_traits<Iterator>;
    using ContainerType = typename iter < raits::value_type;

public:
    using value_type = std::string;
    using iterator_category = typename iter < raits::iterator_category;
    using difference_type = typename iter < raits::difference_type;
    using reference = conditional < std::is_same<std::string, ContainerType>::value, typename iter<raits::reference, std::string>;
    using pointer = fake_ptr_proxy<reference>;

private:
    // TODO: make join compatible with generic iterators?
    mutable std::string _delimiter{};
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    std::string _fmt{};
#endif
    Iterator _iterator{};
    mutable bool _is_iterator_turn{ true };

private:
    template<class T = ContainerType>
    enable_if<!std::is_same<T, std::string>::value, reference> deref() const {
        if (_is_iterator_turn) {
#ifdef LZ_STANDALONE
#ifdef LZ_HAS_FORMAT
            return std::vformat(_fmt.c_str(), std::make_format_args(*_iterator));
#else
            return to_string(*_iterator);
#endif // LZ_HAS_FORMAT
#else
#if defined(LZ_HAS_CXX_20) && FMT_VERSION >= 90000
            return fmt::format(fmt::runtime(_fmt.c_str()), *_iterator);
#else
            return fmt::format(_fmt.c_str(), *_iterator);
#endif // LZ_HAS_CXX_20 && FMT_VERSION >= 90000
#endif // LZ_STANDALONE
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 enable_if<std::is_same<T, std::string>::value, reference> deref() const {
        if (_is_iterator_turn) {
            return *_iterator;
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 enable_if<std::is_same<T, std::string>::value, reference>
    index_operator(const difference_type offset) const {
        // If we use *(*this + offset) when a delimiter must be returned, then we get a segfault because the operator+ returns a
        // copy of the delimiter
        if (_is_iterator_turn && is_even(offset)) {
            return *(*this + offset);
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 enable_if<!std::is_same<T, std::string>::value, reference>
    index_operator(const difference_type offset) const {
        return *(*this + offset);
    }

public:
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    LZ_CONSTEXPR_CXX_20
    join_iterator(Iterator iterator, std::string delimiter, std::string fmt, const bool is_iter < urn) :
        _delimiter(std::move(delimiter)),
        _fmt(std::move(fmt)),
        _iterator(std::move(iterator)),
        _is_iterator_turn(is_iter < urn) {
    }
#else
    LZ_CONSTEXPR_CXX_20
    join_iterator(Iterator iterator, std::string delimiter, const bool is_iter < urn) :
        _delimiter(std::move(delimiter)),
        _iterator(std::move(iterator)),
        _is_iterator_turn(is_iter < urn) {
    }
#endif // has format

    join_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return deref();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_is_iterator_turn) {
            ++_iterator;
        }
        _is_iterator_turn = !_is_iterator_turn;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        _is_iterator_turn = !_is_iterator_turn;
        if (_is_iterator_turn) {
            --_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        _iterator += offset < 0 ? round_even<difference_type>(offset * -1, static_cast<difference_type>(2)) * -1
                                : round_even<difference_type>(offset, static_cast<difference_type>(2));
        if (!is_even(offset)) {
            _is_iterator_turn = !_is_iterator_turn;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const join_iterator& b) const {
        LZ_ASSERT(_delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        // distance * 2 for delimiter, - 1 for removing last delimiter
        return (_iterator - b._iterator) * 2 - 1;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const join_iterator& b) const noexcept {
        LZ_ASSERT(_delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const S& other) const noexcept {
        return _iterator == other;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return index_operator(offset);
    }
};
} // namespace detail
} // namespace lz

#endif