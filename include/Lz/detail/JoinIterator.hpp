#pragma once

#ifndef LZ_JOIN_ITERATOR_HPP
#    define LZ_JOIN_ITERATOR_HPP

#    include "LzTools.hpp"

#    if defined(LZ_STANDALONE)
#        ifdef LZ_HAS_FORMAT
#            include <format>
#        else
#            include <sstream>

#        endif // LZ_HAS_FORMAT
#    endif     // LZ_STANDALONE

#include <iostream> // TODO remove

namespace lz {
namespace internal {
#    if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
template<class T>
internal::EnableIf<!std::is_arithmetic<T>::value, std::string> toStringSpecialized(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
#    endif // defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))

template<LZ_CONCEPT_ITERATOR Iterator>
class JoinIterator {
    using IterTraits = std::iterator_traits<Iterator>;
    using ContainerType = typename IterTraits::value_type;

public:
    using value_type = std::string;
    using iterator_category = typename IterTraits::iterator_category;
    using difference_type = typename IterTraits::difference_type;
    using reference = Conditional<std::is_same<std::string, ContainerType>::value, typename IterTraits::reference, std::string>;
    using pointer = FakePointerProxy<reference>;

private:
    Iterator _iterator{};
    mutable std::string _delimiter{};
#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    std::string _fmt{};
#    endif
    mutable bool _isIteratorTurn{ true };

    template<class T = ContainerType>
    EnableIf<!std::is_same<T, std::string>::value, reference> deref() const {
        if (_isIteratorTurn) {
#    ifdef LZ_STANDALONE
#        ifdef LZ_HAS_FORMAT
            return std::vformat(_fmt.c_str(), std::make_format_args(*_iterator));
#        else
            return toStringSpecialized(*_iterator);
#        endif // LZ_HAS_FORMAT
#    else
#        if defined(LZ_HAS_CXX_20) && FMT_VERSION >= 90000
            return fmt::format(fmt::runtime(_fmt.c_str()), *_iterator);
#        else
            return fmt::format(_fmt.c_str(), *_iterator);
#        endif // LZ_HAS_CXX_20 && FMT_VERSION >= 90000
#    endif     // LZ_STANDALONE
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 EnableIf<std::is_same<T, std::string>::value, reference> deref() const {
        if (_isIteratorTurn) {
            return *_iterator;
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 EnableIf<std::is_same<T, std::string>::value, reference>
    indexOperator(const difference_type offset) const {
        // If we use *(*this + offset) when a delimiter must be returned, then we get a segfault because the operator+ returns a
        // copy of the delimiter
        if (_isIteratorTurn && isEven(offset)) {
            return *(*this + offset);
        }
        return _delimiter;
    }

    template<class T = ContainerType>
    LZ_CONSTEXPR_CXX_20 EnableIf<!std::is_same<T, std::string>::value, reference>
    indexOperator(const difference_type offset) const {
        return *(*this + offset);
    }

public:
#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    LZ_CONSTEXPR_CXX_20
    JoinIterator(Iterator iterator, std::string delimiter, std::string fmt, const bool isIteratorTurn) :
        _iterator(std::move(iterator)),
        _delimiter(std::move(delimiter)),
        _fmt(std::move(fmt)),
        _isIteratorTurn(isIteratorTurn) {
    }
#    else
    LZ_CONSTEXPR_CXX_20
    JoinIterator(Iterator iterator, std::string delimiter, const bool isIteratorTurn) :
        _iterator(std::move(iterator)),
        _delimiter(std::move(delimiter)),
        _isIteratorTurn(isIteratorTurn) {
    }
#    endif // has format

    JoinIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return deref();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 JoinIterator& operator++() {
        if (_isIteratorTurn) {
            ++_iterator;
        }
        _isIteratorTurn = !_isIteratorTurn;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 JoinIterator& operator++(int) {
        JoinIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 JoinIterator& operator--() {
        _isIteratorTurn = !_isIteratorTurn;
        if (_isIteratorTurn) {
            --_iterator;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 JoinIterator operator--(int) {
        JoinIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 JoinIterator& operator+=(const difference_type offset) {
        _iterator += offset < 0 ? roundEven<difference_type>(offset * -1, 2) * -1 : roundEven<difference_type>(offset, 2);
        if (!isEven(offset)) {
            _isIteratorTurn = !_isIteratorTurn;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 JoinIterator& operator-=(const difference_type offset) {
        return *this += -offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 JoinIterator operator+(const difference_type offset) const {
        JoinIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const JoinIterator& a, const JoinIterator& b) {
        LZ_ASSERT(a._delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        // distance * 2 for delimiter, - 1 for removing last delimiter
        return (a._iterator - b._iterator) * 2 - 1;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return indexOperator(offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 JoinIterator operator-(const difference_type offset) const {
        JoinIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const JoinIterator& a, const JoinIterator& b) noexcept {
        if (a._delimiter != b._delimiter)
#    ifdef LZ_STANDALONE
            std::cout << a._delimiter << ' ' << b._delimiter << '\n';
#    else
            fmt::print("{} {}\n", a._delimiter, b._delimiter);
#    endif
        LZ_ASSERT(a._delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        return a._iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const JoinIterator& a, const JoinIterator& b) noexcept {
        return !(a == b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const JoinIterator& a, const JoinIterator& b) {
        LZ_ASSERT(a._delimiter == b._delimiter, "incompatible iterator types: found different delimiters");
        return b - a > 0;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const JoinIterator& a, const JoinIterator& b) {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const JoinIterator& a, const JoinIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const JoinIterator& a, const JoinIterator& b) {
        return !(a < b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif