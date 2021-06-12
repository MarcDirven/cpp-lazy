#pragma once

#ifndef LZ_SPLIT_ITERATOR_HPP
#define LZ_SPLIT_ITERATOR_HPP

#include "LzTools.hpp"

#include <string>

namespace lz {
namespace internal {
template<class SubString, class String, class StringType>
class SplitIterator {
    std::size_t _currentPos{}, _last{};
    const String* _string{ nullptr };
    StringType _delimiter{};

    constexpr std::size_t getLength(std::true_type /* isChar */) const {
        return 1;
    }

    LZ_CONSTEXPR_CXX_20 std::size_t getLength(std::false_type /* isChar */) const {
        return _delimiter.length();
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = SubString;
    using reference = SubString;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    LZ_CONSTEXPR_CXX_20 SplitIterator(const std::size_t startingPosition, const String& string, StringType delimiter) :
        _currentPos(startingPosition),
        _string(&string),
        _delimiter(std::move(delimiter)) {
        // Micro optimization, check if object is created from begin(), only then we want to search
        if (startingPosition == 0) {
            _last = _string->find(_delimiter, _currentPos);
        }
    }

    SplitIterator() = default;

    LZ_CONSTEXPR_CXX_20 value_type operator*() const {
        if (_last != std::string::npos) {
            return SubString(&(*_string)[_currentPos], _last - _currentPos);
        }
        else {
            return SubString(&(*_string)[_currentPos]);
        }
    }

    LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    constexpr friend bool operator!=(const SplitIterator& a, const SplitIterator& b) noexcept {
        return a._currentPos != b._currentPos;
    }

    constexpr friend bool operator==(const SplitIterator& a, const SplitIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_CONSTEXPR_CXX_20 SplitIterator& operator++() noexcept {
        const std::size_t delimLen = getLength(std::is_same<StringType, char>());
        const std::size_t stringLen = _string->length();
        if (_last == std::string::npos) {
            _currentPos = stringLen;
        }
        else if (_last == stringLen - delimLen) {
            // Check if ends with delimiter
            _last = std::string::npos;
            _currentPos = _string->length();
        }
        else {
            _currentPos = _last + delimLen;
            _last = _string->find(_delimiter, _currentPos);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 SplitIterator operator++(int) noexcept {
        SplitIterator tmp(*this);
        ++*this;
        return tmp;
    }
};
} // namespace internal
} // namespace lz

#endif