#pragma once

#ifndef LZ_SPLIT_ITERATOR_HPP
#    define LZ_SPLIT_ITERATOR_HPP

#    include "LzTools.hpp"

#    include <string>

namespace lz {
namespace internal {
template<class SubString, class String, class StringType>
class SplitIterator {
    std::size_t _currentPos{}, _lastPos{};
    const String* _string{ nullptr };
    StringType _delimiter{};

#    ifdef __cpp_if_constexpr
    std::size_t getDelimiterLength() const { // NOLINT
        if constexpr (std::is_same_v<char, StringType>) {
            return 1;
        }
        else {
            return _delimiter.length();
        }
    }
#    else
    template<class T = StringType>
    constexpr EnableIf<std::is_same<char, T>::value, std::size_t> getDelimiterLength() const {
        return 1;
    }

    template<class T = StringType>
    LZ_CONSTEXPR_CXX_20 EnableIf<!std::is_same<char, T>::value, std::size_t> getDelimiterLength() const {
        return _delimiter.length();
    }
#    endif
public:
    using iterator_category =
        typename std::common_type<std::bidirectional_iterator_tag, IterCat<typename String::const_iterator>>::type;
    using value_type = SubString;
    using reference = SubString;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    LZ_CONSTEXPR_CXX_20 SplitIterator(const std::size_t startingPosition, const String& string, StringType delimiter) :
        _currentPos(startingPosition),
        _string(&string),
        _delimiter(std::move(delimiter)) {
        if (startingPosition == 0) {
            _lastPos = _string->find(_delimiter);
        }
        else {
            _currentPos = startingPosition + getDelimiterLength();
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
            _currentPos = _string->length() + getDelimiterLength();
        }
        else {
            _currentPos = _lastPos + getDelimiterLength();
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
        const auto delimLen = getDelimiterLength();
        _lastPos = _currentPos - delimLen;
        _currentPos -= delimLen;
        if (_currentPos != 0) {
            _currentPos = _string->rfind(_delimiter, _currentPos - 1) + delimLen;
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