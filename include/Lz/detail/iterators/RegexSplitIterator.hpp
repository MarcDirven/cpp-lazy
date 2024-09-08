#pragma once

#ifndef LZ_REGEX_SPLIT_ITERATOR_HPP
#define LZ_REGEX_SPLIT_ITERATOR_HPP

#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

namespace lz {
namespace detail {

template<class RegexTokenIter>
class RegexSplitIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = BasicStringView<typename RegexTokenIter::regex_type::value_type>;
    using difference_type = typename RegexTokenIter::difference_type;
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
        return value_type(&*_current->first, static_cast<std::size_t>(_current->length()));
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

} // namespace detail
} // namespace lz

#endif