#pragma once

#ifndef LZ_SPLIT_ITERATOR_HPP
#define LZ_SPLIT_ITERATOR_HPP

#include "LzTools.hpp"

#include <string>

namespace lz {
namespace internal {
template<class CharT>
std::size_t getDelimiterLength(const CharT* delimiter) {
    return std::strlen(delimiter);
}

template<class CharT, std::size_t N>
std::size_t getDelimiterLength(const CharT (& /*delimiter*/)[N]) {
    LZ_ASSERT(N > 0, "delimiter must have a length of at least 1");
    return N - 1;
}

#ifdef LZ_HAS_CXX_17
template<class CharT>
std::size_t getDelimiterLength(const CharT delimiter) {
    if constexpr (std::is_arithmetic_v<CharT>) {
        return 1;
    }
    else {
        return delimiter.length();
    }
}
#else

template<class T>
EnableIf<std::is_arithmetic<T>::value, std::size_t> getDelimiterLength(T /*delimiter*/) {
    return 1;
}

template<class T>
EnableIf<!std::is_arithmetic<T>::value, std::size_t> getDelimiterLength(const T& delimiter) {
    return delimiter.length();
}
#endif

template<class SubString, class String, class DelimiterString>
class SplitIterator {
    std::size_t _currentPos{}, _lastPos{}, _delimiterLength{};
    const String* _string{ nullptr };
    DelimiterString _delimiter{};

public:
    using iterator_category =
        typename std::common_type<std::bidirectional_iterator_tag, IterCat<typename String::const_iterator>>::type;
    using value_type = SubString;
    using reference = SubString;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    LZ_CONSTEXPR_CXX_20 SplitIterator(const std::size_t startingPosition, const String& string, DelimiterString delimiter,
                                      std::size_t delimiterLength) :
        _currentPos(startingPosition),
        _delimiterLength(delimiterLength),
        _string(&string),
        _delimiter(std::move(delimiter)) {
        if (startingPosition == 0) {
            _lastPos = _string->find(_delimiter);
        }
        else {
            _currentPos = startingPosition + _delimiterLength;
        }
    }

    SplitIterator() = default;

    LZ_CONSTEXPR_CXX_20 value_type operator*() const {
        if (_lastPos != std::string::npos) {
            return SubString(&(*_string)[_currentPos], _lastPos - _currentPos);
        }
        else {
            return SubString(&(*_string)[_currentPos]);
        }
    }

    LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 friend bool operator!=(const SplitIterator& a, const SplitIterator& b) noexcept {
        LZ_ASSERT(a._delimiter == b._delimiter, "incompatible iterator types, found different delimiters");
        return a._currentPos != b._currentPos;
    }

    LZ_CONSTEXPR_CXX_14 friend bool operator==(const SplitIterator& a, const SplitIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_CONSTEXPR_CXX_20 SplitIterator& operator++() {
        if (_lastPos == std::string::npos) {
            _currentPos = _string->length() + _delimiterLength;
        }
        else {
            _currentPos = _lastPos + _delimiterLength;
            _lastPos = _string->find(_delimiter, _currentPos);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 SplitIterator operator++(int) {
        SplitIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 SplitIterator& operator--() {
        _lastPos = _currentPos - _delimiterLength;
        _currentPos -= _delimiterLength;
        if (_currentPos != 0) {
            _currentPos = _string->rfind(_delimiter, _currentPos - 1) + _delimiterLength;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 SplitIterator& operator--(int) {
        SplitIterator tmp(*this);
        --*this;
        return tmp;
    }
};
} // namespace internal
} // namespace lz

#endif