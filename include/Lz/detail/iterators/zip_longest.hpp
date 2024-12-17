#pragma once

#ifndef LZ_ZIP_LONGEST_ITERATOR_HPP
#define LZ_ZIP_LONGEST_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/optional.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iter_base.hpp"

namespace lz {
namespace detail {
template<bool, class...>
class zip_longest_iterator;

template<class... Iterators>
class zip_longest_iterator<false /*is random access*/, Iterators...>
    : public iter_base<zip_longest_iterator<false, Iterators...>, std::tuple<optional<val_t<Iterators>>...>,
                       fake_ptr_proxy<std::tuple<optional<val_t<Iterators>>...>>, common_type<diff_type<Iterators>...>,
                       std::forward_iterator_tag> {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::tuple<optional<val_t<Iterators>>...>;
    using difference_type = common_type<diff_type<Iterators>...>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;

private:
    using make_idx_sequence_for_this = make_index_sequence<sizeof...(Iterators)>;
    std::tuple<Iterators...> _iterators{};
    std::tuple<Iterators...> _end{};

    template<class I>
    LZ_CONSTEXPR_CXX_20 optional<val_t<I>> deref_one(const I& iterator, const I& end) const {
        return iterator == end ? optional<val_t<I>>{} : optional<val_t<I>>(*iterator);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void increment_one(I& iterator, const I& end) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrement_one(I& iterator, const I& begin) const {
        if (iterator == begin) {
            return;
        }
        --iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 value_type dereference(index_sequence<I...>) const {
        return value_type{ deref_one(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(index_sequence<I...>) {
        decompose((increment_one(std::get<I>(_iterators), std::get<I>(_end)), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, false) == end;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit zip_longest_iterator(std::tuple<Iterators...> iterators, std::tuple<Iterators...> end) :
        _iterators(std::move(iterators)),
        _end(std::move(end)) {
    }

    constexpr zip_longest_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type dereference() const {
        return dereference(make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        increment(make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& b) const {
        return eq(b, make_idx_sequence_for_this());
    }
};

template<class... Iterators>
class zip_longest_iterator<true /*is random access*/, Iterators...>
    : public iter_base<zip_longest_iterator<true, Iterators...>, std::tuple<optional<val_t<Iterators>>...>,
                       fake_ptr_proxy<std::tuple<optional<val_t<Iterators>>...>>, common_type<diff_type<Iterators>...>,
                       std::random_access_iterator_tag> {
public:
    using iterator_category = common_type<iter_cat_t<Iterators>...>;
    using value_type = std::tuple<optional<val_t<Iterators>>...>;
    using difference_type = common_type<diff_type<Iterators>...>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<value_type>;

private:
    using make_idx_sequence_for_this = make_index_sequence<sizeof...(Iterators)>;
    std::tuple<Iterators...> _begin{};
    std::tuple<Iterators...> _iterators{};
    std::tuple<Iterators...> _end{};

    template<class I>
    LZ_CONSTEXPR_CXX_20 optional<val_t<I>> deref_one(const I& iterator, const I& end) const {
        return iterator == end ? optional<val_t<I>>{} : optional<val_t<I>>(*iterator);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void increment_one(I& iterator, const I& end) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrement_one(I& iterator, const I& begin) const {
        if (iterator == begin) {
            return;
        }
        --iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 value_type dereference(index_sequence<I...>) const {
        return value_type{ deref_one(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(index_sequence<I...>) {
        decompose((increment_one(std::get<I>(_iterators), std::get<I>(_end)), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrement_one(I& iterator, const difference_type longest, const difference_type this_iter_length) {
        if (this_iter_length == longest) {
            --iterator;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(index_sequence<I...>) {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin))... };
        const auto longest = *std::max_element(std::begin(all_sizes), std::end(all_sizes));
        decompose((decrement_one(std::get<I>(_iterators), longest, all_sizes[I]), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void plus_is_one(I& iterator, const I& end, const difference_type offset) {
        const auto difference = end - iterator;
        if (offset > difference) {
            iterator = end;
        }
        else {
            iterator += offset;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void plus_is(index_sequence<I...>, const difference_type offset) {
        decompose((plus_is_one(std::get<I>(_iterators), std::get<I>(_end), offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const zip_longest_iterator& other, index_sequence<I...>) const {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                          std::get<I>(other._iterators))... };
        return *std::max_element(std::begin(all_sizes), std::end(all_sizes));
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void
    min_is_one(I& iterator, const difference_type this_iter_length, const difference_type longest, const difference_type offset) {
        if (this_iter_length == longest) {
            iterator -= offset;
            return;
        }
        const auto surplus = this_iter_length + offset;
        if (surplus > longest) {
            iterator -= (surplus - longest);
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void min_is(index_sequence<I...>, const difference_type offset) {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin))... };
        const auto longest = *std::max_element(std::begin(all_sizes), std::end(all_sizes));
        decompose((min_is_one(std::get<I>(_iterators), all_sizes[I], longest, offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, false) == end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool lt(const zip_longest_iterator& other, index_sequence<I...>) const {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                          std::get<I>(other._iterators))... };
        return *std::min_element(std::begin(all_sizes), std::end(all_sizes)) < 0;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit zip_longest_iterator(std::tuple<Iterators...> begin, std::tuple<Iterators...> iterators,
                                                      std::tuple<Iterators...> end) :
        _begin(std::move(begin)),
        _iterators(std::move(iterators)),
        _end(std::move(end)) {
    }

    constexpr zip_longest_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type dereference() const {
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
        if (offset < 0) {
            min_is(make_idx_sequence_for_this(), -offset);
        }
        else {
            plus_is(make_idx_sequence_for_this(), offset);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_longest_iterator& other) const {
        return minus(other, make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& b) const {
        return eq(b, make_idx_sequence_for_this());
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP