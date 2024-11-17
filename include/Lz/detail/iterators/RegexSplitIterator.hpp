#pragma once

#ifndef LZ_REGEX_SPLIT_ITERATOR_HPP
#define LZ_REGEX_SPLIT_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

namespace lz {
namespace detail {
template<class RegexTokenIter>
using RegexSplitIterValueType = BasicStringView<typename RegexTokenIter::regex_type::value_type>;

template<class RegexTokenIter>
class RegexSplitIterator : public IterBase<RegexSplitIterator<RegexTokenIter>, RegexSplitIterValueType<RegexTokenIter>,
                                           FakePointerProxy<RegexSplitIterValueType<RegexTokenIter>>,
                                           typename RegexTokenIter::difference_type, std::forward_iterator_tag> {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = RegexSplitIterValueType<RegexTokenIter>;
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

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_current;
    }

    LZ_CONSTEXPR_CXX_14 value_type dereference() const {
        return value_type(&*_current->first, static_cast<std::size_t>(_current->length()));
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const RegexSplitIterator& other) const {
        return _current == other._current;
    }
};

} // namespace detail
} // namespace lz

#endif