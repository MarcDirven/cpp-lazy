#pragma once

#ifndef LZ_SPLIT_ITERATOR_HPP
#define LZ_SPLIT_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

#include <cstring>

namespace lz {
namespace detail {
template<class CharT>
std::size_t getDelimiterLength(const CharT* delimiter) {
    if LZ_CONSTEXPR_IF (std::is_same<CharT, char>::value) {
        return std::strlen(delimiter);
    }
    else {
        return std::char_traits<CharT>::length(delimiter);
    }
}

template<class CharT, std::size_t N>
LZ_CONSTEXPR_CXX_14 std::size_t getDelimiterLength(const CharT (& /*delimiter*/)[N]) {
    LZ_ASSERT(N > 0, "delimiter must have a length of at least 1");
    return N - 1;
}

#ifdef LZ_HAS_CXX_17
template<class CharT>
constexpr std::size_t getDelimiterLength(const CharT& delimiter) {
    if constexpr (std::is_arithmetic_v<CharT>) {
        return 1;
    }
    else {
        return delimiter.length();
    }
}
#else

template<class T>
constexpr EnableIf<std::is_arithmetic<T>::value, std::size_t> getDelimiterLength(T /*delimiter*/) {
    return 1;
}

template<class T>
constexpr EnableIf<!std::is_arithmetic<T>::value, std::size_t> getDelimiterLength(const T& delimiter) {
    return delimiter.length();
}
#endif

template<class SubString, class String, class DelimiterString>
class SplitIterator
    : public IterBase<SplitIterator<SubString, String, DelimiterString>, SubString, FakePointerProxy<SubString>, std::ptrdiff_t,
                      CommonType<std::bidirectional_iterator_tag, IterCat<typename String::const_iterator>>> {

    std::size_t _currentPos{}, _lastPos{}, _delimiterLength{};
    const String* _string{ nullptr };
    DelimiterString _delimiter{};

public:
    using iterator_category = CommonType<std::bidirectional_iterator_tag, IterCat<typename String::const_iterator>>;
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

    LZ_CONSTEXPR_CXX_20 value_type dereference() const {
        if (_lastPos != String::npos) {
            return SubString(&(*_string)[_currentPos], _lastPos - _currentPos);
        }
        return SubString(&(*_string)[_currentPos]);
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_lastPos == String::npos) {
            _currentPos = _string->length() + _delimiterLength;
        }
        else {
            _currentPos = _lastPos + _delimiterLength;
            _lastPos = _string->find(_delimiter, _currentPos);
        }
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        _lastPos = _currentPos - _delimiterLength;
        _currentPos -= _delimiterLength;
        if (_currentPos != 0) {
            _currentPos = _string->rfind(_delimiter, _currentPos - 1) + _delimiterLength;
        }
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const SplitIterator& b) const noexcept {
        LZ_ASSERT(_delimiter == b._delimiter, "incompatible iterator types, found different delimiters");
        return _currentPos == b._currentPos;
    }
};
} // namespace detail
} // namespace lz

#endif