#pragma once

#ifndef LZ_REGEX_SPLIT_ITERATOR_HPP
#define LZ_REGEX_SPLIT_ITERATOR_HPP

#include "LzTools.hpp"

namespace lz {
namespace internal {

template<class RegexTokenIter>
class RegexSplitIterator {
    using Traits = std::iterator_traits<RegexTokenIter>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = StringView;
    using difference_type = typename Traits::difference_type;
    using pointer = FakePointerProxy<value_type>;
    using reference = value_type;

private:
    RegexTokenIter _current{};

public:
    constexpr RegexSplitIterator() = default;

    LZ_CONSTEXPR_CXX_14 RegexSplitIterator(RegexTokenIter first, RegexTokenIter last) : _current(first) {
        while (_current != last && _current->length() == 0) {
            ++_current;
        }
    }

    LZ_CONSTEXPR_CXX_14 RegexSplitIterator& operator++() {
        ++_current;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 RegexSplitIterator operator++(int) {
        RegexSplitIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 value_type operator*() const {
        return value_type(&*_current->first, _current->length());
    }

    LZ_CONSTEXPR_CXX_14 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 bool operator==(const RegexSplitIterator& other) const {
        return _current == other._current;
    }

    LZ_CONSTEXPR_CXX_14 bool operator!=(const RegexSplitIterator& other) const {
        return !(*this == other);
    }
};

} // namespace internal
} // namespace lz

#endif