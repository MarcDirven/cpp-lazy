#pragma once

#ifndef LZ_JOIN_ITERATOR_HPP
#define LZ_JOIN_ITERATOR_HPP

#include "LzTools.hpp"

#ifdef LZ_STANDALONE
#ifdef LZ_HAS_FORMAT
#include <format>
#else
#include <sstream>
#endif // LZ_HAS_FORMAT
#else
#include <fmt/ostream.h>
#endif // LZ_STANDALONE

namespace lz {
namespace internal {
#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
template<class T>
std::string toStringSpecialized(std::true_type /* isArithmetic */, const T value) {
    using std::to_string;
    using lz::internal::to_string;
    return to_string(value);
}

template<class T>
std::string toStringSpecialized(std::false_type /* isArithmetic */, const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
#endif // defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))

template<LZ_CONCEPT_ITERATOR Iterator>
class JoinIterator {
    using IterTraits = std::iterator_traits<Iterator>;
    using ContainerType = typename IterTraits::value_type;
    using IsContainerTypeString = std::is_same<ContainerType, std::string>;

  public:
    using value_type = std::string;
    using iterator_category = typename IterTraits::iterator_category;
    using difference_type = typename IterTraits::difference_type;
    using reference = Conditional<std::is_same<std::string, ContainerType>::value, typename IterTraits::reference, std::string>;
    using pointer = FakePointerProxy<reference>;

  private:
    Iterator _iterator{};
    mutable std::string _delimiter{};
    mutable bool _isIteratorTurn{ true };
    difference_type _distance{};

    reference deref(std::false_type /* isSameContainerTypeString */) const {
        if (_isIteratorTurn) {
#ifdef LZ_STANDALONE
#ifdef LZ_HAS_FORMAT
            return std::format("{}", *_iterator);
#else
            return toStringSpecialized(std::is_arithmetic<ContainerType>(), *_iterator);
#endif // LZ_HAS_FORMAT
#else
            return fmt::format(FMT_STRING("{}"), *_iterator);
#endif // LZ_STANDALONE
        }
        return _delimiter;
    }

    reference deref(std::true_type /* isSameContainerTypeString */) const {
        if (_isIteratorTurn) {
            return *_iterator;
        }
        return _delimiter;
    }

    reference indexOperator(std::true_type /* isSameContainerTypeString */, const difference_type offset) const {
        // If we use *(*this + offset) when a delimiter must be returned, then we get a segfault because the operator+ returns a
        // copy of the delimiter
        if (_isIteratorTurn && isEven(offset)) {
            return *(*this + offset);
        }
        return _delimiter;
    }

    reference indexOperator(std::false_type /* isSameContainerTypeString */, const difference_type offset) const {
        return *(*this + offset);
    }

  public:
    JoinIterator(Iterator iterator, std::string delimiter, const bool isIteratorTurn, const difference_type distance) :
        _iterator(std::move(iterator)),
        _delimiter(std::move(delimiter)),
        _isIteratorTurn(isIteratorTurn),
        _distance(distance) {
    }

    JoinIterator() = default;

    LZ_NODISCARD reference operator*() const {
        return deref(IsContainerTypeString());
    }

    LZ_NODISCARD pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    JoinIterator& operator++() {
        if (_isIteratorTurn) {
            ++_iterator;
        }
        _isIteratorTurn = !_isIteratorTurn;
        return *this;
    }

    JoinIterator& operator++(int) {
        JoinIterator tmp(*this);
        ++*this;
        return tmp;
    }

    JoinIterator& operator--() {
        _isIteratorTurn = !_isIteratorTurn;
        if (_isIteratorTurn) {
            --_iterator;
        }
        return *this;
    }

    JoinIterator operator--(int) {
        JoinIterator tmp(*this);
        --*this;
        return tmp;
    }

    JoinIterator& operator+=(const difference_type offset) {
        _iterator += offset == 1 ? 1 : offset == _distance ? (offset >> 1) + 1 : offset >> 1;
        if (!isEven(offset)) {
            _isIteratorTurn = !_isIteratorTurn;
        }
        return *this;
    }

    JoinIterator& operator-=(const difference_type offset) {
        if (offset == 1) {
            _iterator -= 1;
        }
        else {
            _iterator -= (offset >> 1);
        }
        if (!isEven(offset)) {
            _isIteratorTurn = !_isIteratorTurn;
        }
        return *this;
    }

    LZ_NODISCARD JoinIterator operator+(const difference_type offset) const {
        JoinIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD friend difference_type operator-(const JoinIterator& a, const JoinIterator& b) {
        LZ_ASSERT(a._delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        // distance * 2 for delimiter, - 1 for removing last delimiter
        return (a._iterator - b._iterator) * 2 - 1;
    }

    LZ_NODISCARD reference operator[](const difference_type offset) const {
        return indexOperator(IsContainerTypeString(), offset);
    }

    LZ_NODISCARD JoinIterator operator-(const difference_type offset) const {
        JoinIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD friend bool operator==(const JoinIterator& a, const JoinIterator& b) {
        LZ_ASSERT(a._delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        return a._iterator == b._iterator;
    }

    LZ_NODISCARD friend bool operator!=(const JoinIterator& a, const JoinIterator& b) {
        return !(a == b); // NOLINT
    }

    LZ_NODISCARD friend bool operator<(const JoinIterator& a, const JoinIterator& b) {
        LZ_ASSERT(a._delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        return b - a > 0;
    }

    LZ_NODISCARD friend bool operator>(const JoinIterator& a, const JoinIterator& b) {
        return b < a;
    }

    LZ_NODISCARD friend bool operator<=(const JoinIterator& a, const JoinIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD friend bool operator>=(const JoinIterator& a, const JoinIterator& b) {
        return !(a < b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif