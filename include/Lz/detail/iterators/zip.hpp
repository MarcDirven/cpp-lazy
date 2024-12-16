#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include "Lz/iter_base.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/traits.hpp"

#include <algorithm>

namespace lz {
namespace detail {
template<class... Iterators>
class zip_iterator
    : public iter_base<zip_iterator<Iterators...>, std::tuple<ref<Iterators>...>, std::tuple<fake_ptr_proxy<ref<Iterators>>...>,
                       common_type<diff_type<Iterators>...>, common_type<iter_cat<Iterators>...>> {

public:
    using iterator_category = common_type<iter_cat<Iterators>...>;
    using value_type = std::tuple<value_type<Iterators>...>;
    using difference_type = common_type<diff_type<Iterators>...>;
    using reference = std::tuple<ref<Iterators>...>;
    using pointer = std::tuple<fake_ptr_proxy<ref<Iterators>>...>;

private:
    using make_idx_sequence_for_this = make_index_sequence<sizeof...(Iterators)>;
    std::tuple<Iterators...> _iterators{};

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence<I...>) const {
        return reference{ *std::get<I>(_iterators)... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(index_sequence<I...>) {
        decompose((++std::get<I>(_iterators), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(index_sequence<I...>) {
        decompose((--std::get<I>(_iterators), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void plus_is(index_sequence<I...>, const difference_type offset) {
        decompose(((std::get<I>(_iterators) += offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const zip_iterator& other, index_sequence<I...>) const {
        const difference_type expand[] = { ((std::get<I>(_iterators) - std::get<I>(other._iterators)))... };
        return *std::min_element(std::begin(expand), std::end(expand));
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_iterator& other, index_sequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, true) != end;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit zip_iterator(std::tuple<Iterators...> iterators) : _iterators(std::move(iterators)) {
    }

    constexpr zip_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        increment(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        decrement(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        plus_is(make_idx_sequence_for_this(), offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_iterator& other) const {
        return minus(other, make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const zip_iterator& b) const {
        return eq(b, make_idx_sequence_for_this());
    }
};
} // namespace detail
} // namespace lz

#endif