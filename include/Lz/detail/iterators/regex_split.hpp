#pragma once

#ifndef LZ_REGEX_SPLIT_ITERATOR_HPP
#define LZ_REGEX_SPLIT_ITERATOR_HPP

#include "Lz/iter_base.hpp"
#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"

namespace lz {
namespace detail {
template<class RegexTokenIter>
using regex_split_value_type = basic_string_view<typename RegexTokenIter::regex_type::value_type>;

template<class RegexTokenIter>
using regex_split_val = typename val_t<RegexTokenIter>::string_type::value_type;

template<class RegexTokenIter>
class regex_split_iterator
    : public iter_base<regex_split_iterator<RegexTokenIter>, basic_string_view<regex_split_val<RegexTokenIter>>,
                       fake_ptr_proxy<basic_string_view<regex_split_val<RegexTokenIter>>>, diff_type<RegexTokenIter>,
                       std::forward_iterator_tag, default_sentinel> {
public:
    using value_type = basic_string_view<regex_split_val<RegexTokenIter>>;
    using difference_type = typename RegexTokenIter::difference_type;
    using pointer = fake_ptr_proxy<value_type>;
    using reference = value_type;

private:
    RegexTokenIter _current{};

public:
    constexpr regex_split_iterator() = default;

    LZ_CONSTEXPR_CXX_14 regex_split_iterator(RegexTokenIter first, RegexTokenIter last) : _current(first) {
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
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const regex_split_iterator& other) const {
        return _current == other._current;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return _current == RegexTokenIter{};
    }
};

} // namespace detail
} // namespace lz

#endif