#pragma once

#ifndef LZ_LZ_HPP
#define LZ_LZ_HPP

#include "Lz/algorithm.hpp"
#include "Lz/any_iterable.hpp"
#include "Lz/c_string.hpp"
#include "Lz/cartesian_product.hpp"
#include "Lz/chunk_if.hpp"
#include "Lz/chunks.hpp"
#include "Lz/concatenate.hpp"
#include "Lz/drop_while.hpp"
#include "Lz/drop.hpp"
#include "Lz/enumerate.hpp"
#include "Lz/except.hpp"
#include "Lz/exclude.hpp"
#include "Lz/exclusive_scan.hpp"
#include "Lz/filter.hpp"
#include "Lz/flatten.hpp"
#include "Lz/generate.hpp"
#include "Lz/generate_while.hpp"
#include "Lz/group_by.hpp"
#include "Lz/inclusive_scan.hpp"
#include "Lz/iter_tools.hpp"
#include "Lz/join.hpp"
#include "Lz/join_where.hpp"
#include "Lz/loop.hpp"
#include "Lz/map.hpp"
#include "Lz/random.hpp"
#include "Lz/range.hpp"
#include "Lz/regex_split.hpp"
#include "Lz/repeat.hpp"
#include "Lz/rotate.hpp"
#include "Lz/slice.hpp"
#include "Lz/split.hpp"
#include "Lz/take.hpp"
#include "Lz/take_every.hpp"
#include "Lz/unique.hpp"
#include "Lz/zip.hpp"
#include "Lz/zip_longest.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class chain_iterable;

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Converts an iterable into a chain_iterable, where one can chain iterators using dot operator (.filter().map().select().any())
 * @param iterable The iterable to view over.
 * @return An iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_CONSTEXPR_CXX_20 chain_iterable<iter<Iterable>, sentinel<Iterable>> chain(Iterable&& iterable) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) };
}

// End of group
/**
 * @}
 */

template<class Iterator, class S = Iterator>
class chain_iterable final : public detail::basic_iterable<Iterator, S> {
    using Base = detail::basic_iterable<Iterator, S>;
    using Traits = std::iterator_traits<Iterator>;

public:
    using detail::basic_iterable<Iterator, S>::begin;
    using detail::basic_iterable<Iterator, S>::end;

    using iterator = Iterator;
    using const_iterator = iterator;
    using difference_type = typename Traits::difference_type;

    using value_type = typename Traits::value_type;
    using reference = typename Traits::reference;

    LZ_CONSTEXPR_CXX_20 chain_iterable(Iterator begin, S end) : Base(std::move(begin), std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 chain_iterable() = default;

    //! See algorithm.hpp for documentation.
    template<class UnaryPredicate>
    void for_each_while(UnaryPredicate&& unary_predicate) const {
        lz::for_each_while(*this, std::forward<UnaryPredicate>(unary_predicate));
    }

    //! See concatenate.hpp for documentation.
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::concatenate_iterator<Iterator, iter<Iterables>...>>
    concat(Iterables&&... iterables) const {
        return chain(lz::concat(*this, std::forward<Iterables>(iterables)...));
    }

    //! See enumerate.hpp for documentation.
    template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::enumerate_iterator<Iterator, S, Arithmetic>>
    enumerate(const Arithmetic begin = 0) const {
        return chain(lz::enumerate(*this, begin));
    }

    //! See join.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::join_iterator<Iterator, S>> join(std::string delimiter) const {
        return chain(lz::join(*this, std::move(delimiter)));
    }

    //! See map.hpp for documentation
    template<class UnaryOp>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::map_iterator<Iterator, S, UnaryOp>>
    map(UnaryOp unary_op) const {
        return chain(lz::map(*this, std::move(v)));
    }

    //! See take_while.hpp for documentation.
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto take_while(UnaryPredicate unary_predicate) const
        -> decltype(chain(lz::take_while(*this, std::move(unary_predicate)))) {
        return chain(lz::take_while(*this, std::move(unary_predicate)));
    }

    //! See take.hpp for documentation. Internally uses lz::take to take the amounts
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto take(const difference_type amount) const
    -> chain_iterable<detail::take_n_iterator<Iterator>, typename detail::take_n_iterator<Iterator::sentinel> {
        return chain(lz::take(*this, amount));
    }

    //! See drop_while.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto drop(const difference_type amount) const -> chain_iterable<Iterator, S> {
        return chain(lz::drop(*this, amount));
    }

    //! See slice.hpp for documentation
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto slice(const difference_type from, const difference_type to) const
    -> chain_iterable<detail::take_n_iterator<Iterator>, typename detail::take_n_iterator<Iterator::sentinel> {
        return chain(lz::slice(*this, from, to));
    }

    //! See take_every.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::take_every_iterator<Iterator, S>>
    take_every(const difference_type offset, const Iterator start) const {
        return chain(lz::take_every(*this, offset, start));
    }

    //! See take_every.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::take_every_iterator<Iterator, S>>
    take_every(const difference_type offset) const {
        return chain(lz::take_every(*this, offset));
    }

    //! See chunks.hpp for documentation
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::chunks_iterator<Iterator, S>> 
    chunks(const std::size_t chunk_size) const {
        return chain(lz::chunks(*this, chunk_size));
    }

    //! See zip.hpp for documentation.
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::zip_iterator<Iterator, iter<Iterables>>... >
    zip(Iterables&&... iterables) const {
        return chain(lz::zip(*this, std::forward<Iterables>(iterables)...));
    }

    //! See iter_tools.hpp `zip_with` for documentation
    template<class Fn, class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto zip_with(Fn fn, Iterables&&... iterables) const
        -> chain_iterable<decltype(std::begin(lz::zip_with(std::move(fn), *this, std::forward<Iterables>(iterables)...)))> {
        return chain(lz::zip_with(std::move(fn), *this, std::forward<Iterables>(iterables)...));
    }

    //! See iter_tool.hpp `as` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::map_iterator<Iterator, S, detail::ConvertFn<T>>> as() const {
        return chain(lz::as<T>(*this));
    }

    //! See iter_tools.hpp `reverse` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<std::reverse_iterator<Iterator>> reverse() const {
        return chain(lz::reverse(*this));
    }

    //! See iter_tools.hpp `reverse` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::zip_iterator<Iterator, Iterator>> pairwise() const {
        return chain(lz::pairwise(*this));
    }

    // clang-format off

    //! See cartesian_product.hpp for documentation
    template<class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
    chain_iterable<detail::cartesian_product_iterator<
        std::tuple<Iterator, iter<Iterables>...>, std::tuple<S, sentinel<Iterables>...>>>
    cartesian_product(Iterables&&... iterables) const {
        return chain(lz::cartesian_product(*this, std::forward<Iterables>(iterables)...));
    }


    //! See flatten.hpp for documentation
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<
        detail::flatten_iterator<Iterator, S, detail::count_dims<std::iterator_traits<Iterator>>::value - 1>> 
    flatten() const {
        return chain(lz::flatten(*this));
    }

    // clang-format on

    //! See loop.hpp for documentation
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::loop_iterator<Iterator, S>> loop() const {
        return chain(lz::loop(*this));
    }

    //! See exclude.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto exclude(const difference_type from, const difference_type to) const
        -> decltype(chain(std::declval<exclude_iterable<Iterator, S>>())) {
        return chain(lz::exclude(*this, from, to));
    }

    // clang-format off
    //! See inclusive_scan.hpp for documentation.
    template<class T = value_type, class BinaryOp = MAKE_BIN_OP(std::plus, value_type<iterator>)>
    LZ_NODISCARD
    LZ_CONSTEXPR_CXX_20 chain_iterable<detail::inclusive_scan_iterator<Iterator, S, detail::decay<T>, detail::decay<BinaryOp>>>
    inclusive_scan(T&& init = {}, BinaryOp&& binary_op = {}) const {
        return chain(lz::inclusive_scan(*this, std::forward<T>(init), std::forward<BinaryOp>(binary_op)));
    }

    //! See exclusive_scan.hpp for documentation.
    template<class T = value_type, class BinaryOp = MAKE_BIN_OP(std::plus, value_type<iterator>)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
    chain_iterable<detail::exclusive_scan_iterator<Iterator, S, detail::decay<T>, detail::decay<BinaryOp>>>
    eScan(T&& init = {}, BinaryOp&& binary_op = {}) const {
        return chain(lz::eScan(*this, std::forward<T>(init), std::forward<BinaryOp>(binary_op)));
    }
    // clang-format on

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::rotate_iterator<Iterator, S>> rotate(iterator start) const {
        return chain(lz::rotate(*this, std::move(start)));
    }

    //! See algorithm.hpp `has_one` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool has_one() const {
        return lz::has_one(*this);
    }

    //! See algorithm.hpp `has_many` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool has_many() const {
        return lz::has_many(*this);
    }

    //! See algorithm.hpp `front_or` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type front_or(const T& defaultValue) const {
        return lz::front_or(*this, defaultValue);
    }

    //! See algorithm.hpp `back_or` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type back_or(const T& defaultValue) const {
        return lz::back_or(*this, defaultValue);
    }

    //! See filter.hpp for documentation
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::filter_iterator<Iterator, S, UnaryPredicate>>
    filter(UnaryPredicate unary_predicate) const {
        return chain(lz::filter(*this, std::move(unary_predicate)));
    }

    //! See except.hpp for documentation
    template<class IterableToExcept, class BinaryPredicate = MAKE_BIN_OP(std::less, value_type)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::except_iterator<Iterator, S, iter<IterableToExcept>, sentinel<IterableToExcept>, BinaryPredicate>>
    except(IterableToExcept&& toExcept, BinaryPredicate binary_predicate = {}) const {
        return chain(lz::except(*this, toExcept, std::move(binary_predicate)));
    }

    //! See unique.hpp for documentation
    template<class BinaryPredicate = MAKE_BIN_OP(std::less, value_type)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::unique_iterator<Iterator, S, BinaryPredicate>> 
    unique(BinaryPredicate binary_predicate = {}) const {
        return chain(lz::unique_iterable(*this, std::move(binary_predicate)));
    }

    //! See chunk_if.hpp for documentation
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::chunk_if_iterator<Iterator, S, UnaryPredicate>> 
    chunk_if(UnaryPredicate unary_predicate) const {
        return chain(lz::chunk_if(*this, std::move(unary_predicate)));
    }

    //! See iter_tools.hpp `filter_map` for documentation
    template<class UnaryMapOp, class UnaryFilterPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto filter_map(UnaryFilterPredicate filter_predicate, UnaryMapOp map_op) const
        -> decltype(chain(lz::filter_map(*this, std::move(filter_predicate), std::move(map_op)))) {
        return chain(lz::filter_map(*this, std::move(filter_predicate), std::move(map_op)));
    }

    //! See iter_tools.hpp `select` for documentation
    template<class SelectorIterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto select(SelectorIterable&& selectors) const
        -> decltype(chain(lz::select(*this, std::forward<SelectorIterable>(selectors)))) {
        return chain(lz::select(*this, std::forward<SelectorIterable>(selectors)));
    }

    //! See join_where.hpp for documentation
    // clang-format off
    template<class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
    chain_iterable<detail::join_where_iterator<Iterator, S, iter<IterableB>, sentinel<IterableB>, SelectorA, SelectorB, ResultSelector>>
    join_where(IterableB&& iterable_b, SelectorA selector_a, SelectorB selector_b, ResultSelector result_selector) const {
        return chain(lz::join_where(*this, iterable_b, std::move(selector_a), std::move(selector_b), std::move(result_selector)));
    }
    // clang-format on

    //! See take.hpp for documentation
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<Iterator> drop_while(UnaryPredicate unary_predicate) const {
        return chain(lz::drop_while(*this, std::move(unary_predicate)));
    }

    //! See group_by.hpp for documentation
    template<class BinaryPredicate = MAKE_BIN_OP(std::less, value_type)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chain_iterable<detail::group_by_iterator<Iterator, S, BinaryPredicate>>
    group_by(BinaryPredicate binary_predicate = {}) const {
        return chain(lz::group_by(*this, std::move(binary_predicate)));
    }

    //! See iter_tools.hpp `trim` for documentation
    template<class UnaryPredicateFirst, class UnaryPredicateLast>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto trim(UnaryPredicateFirst first, UnaryPredicateLast last) const
        -> decltype(chain(lz::trim(*this, std::move(first), std::move(last)))) {
        return chain(lz::trim(*this, std::move(first), std::move(last)));
    }

    //! See algorithm.hpp `find_or_default` for documentation
    template<class T, class U>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type find_or_default(T&& to_find, U&& default_value) const {
        return lz::find_or_default(*this, to_find, default_value);
    }

    //! See algorithm.hpp `find_or_default_if` for documentation
    template<class UnaryPredicate, class U>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type find_or_default_if(UnaryPredicate unary_predicate, U&& default_value) const {
        return lz::find_or_default_if(*this, std::move(unary_predicate), default_value);
    }

    // //! See Algorithm.hpp `findLastOrDefault` for documentation
    // template<class T, class U>
    // value_type findLastOrDefault(T&& toFind, U&& defaultValue) const {
    //     return lz::findLastOrDefault(*this, toFind, defaultValue);
    // }

    // //! See Algorithm.hpp `findLastOrDefaultIf` for documentation
    // template<class UnaryPredicate, class U>
    // value_type findLastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
    //     return lz::findLastOrDefaultIf(*this, std::move(predicate), defaultValue);
    // }

    // //! See Algorithm.hpp `indexOf` for documentation
    // template<class T>
    // std::size_t indexOf(const T& value) const {
    //     return lz::indexOf(*this, value);
    // }

    // //! See Algorithm.hpp `indexOfIf` for documentation
    // template<class UnaryPredicate>
    // std::size_t indexOfIf(UnaryPredicate predicate) const {
    //     return lz::indexOfIf(*this, std::move(predicate));
    // }

    // //! See Algorithm.hpp `contains` for documentation
    // template<class T>
    // bool contains(const T& value) const {
    //     return lz::contains(*this, value);
    // }

    // //! See Algorithm.hpp `contains_if` for documentation
    // template<class UnaryPredicate>
    // bool contains_if(UnaryPredicate predicate) const {
    //     return lz::contains_if(*this, std::move(predicate));
    // }

    /**
     * Checks if two views/iterables are equal.
     * @param other The other view/iterable to binary_predicate with
     * @param binary_predicate The comparer, default is `operator==`
     * @return
     */
    template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool equal(const Iterable& other, BinaryPredicate binary_predicate = {}) const {
        return lz::equal(*this, other, std::move(binary_predicate));
    }

    // /**
    //  * Checks if this starts with an other iterable.
    //  * @param iterable The other iterable to compare with
    //  * @param compare The comparer (operator== is default)
    //  * @param execution The execution policy.
    //  * @return True if this starts with `iterable`, false otherwise.
    //  */
    // template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type)>
    // bool startsWith(const Iterable& iterable, BinaryPredicate compare = {}) const {
    //     return lz::startsWith(*this, iterable, std::move(compare));
    // }

    // /**
    //  * Checks if this ends with an other iterable.
    //  * @param iterable The other iterable to compare with
    //  * @param compare The comparer (operator== is default)
    //  * @param execution The execution policy.
    //  * @return True if this ends with `iterable`, false otherwise.
    //  */
    // template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type)>
    // bool endsWith(const Iterable& iterable, BinaryPredicate compare = {}) const {
    //     return lz::endsWith(*this, iterable, std::move(compare));
    // }

    /**
     * Iterates over the sequence generated so far.
     * @param unary_op A function to apply over each element. Must have the following signature: `void func(value_type)`
     */
    template<class UnaryOp>
    LZ_CONSTEXPR_CXX_20 chain_iterable<Iterator>& for_each(UnaryOp unary_op) {
        lz::for_each(*this, std::move(unary_op));
        return *this;
    }

    /**
     * Performs a left fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
     * `[](value_type init, value_type next) const { return std::move(init) + value_type; }`
     * @param init The starting value
     * @param binary_op A binary function with the following signature `value_type func(value_type init, value_type element)`
     */
    template<class T, class BinaryOp>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 T foldl(T&& init, BinaryOp binary_op) const {
        return lz::accumulate(*this, std::forward<T>(init), std::move(binary_op));
    }

    /**
     * Performs a right fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
     * `[](value_type init, value_type next) const { return std::move(init) + value_type; }`
     * @param init The starting value
     * @param binary_op A binary function with the following signature `value_type func(value_type init, value_type element)`
     */
    template<class T, class BinaryOp>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 T foldr(T&& init, BinaryOp binary_op) const {
        return lz::accumulate(reverse(), std::forward<T>(init), std::move(binary_op));
    }

    /**
     * Sums the sequence generated so far.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type sum() const {
#ifdef LZ_HAS_CXX_11
        return this->foldl(value_type(), [](value_type init, const value_type& val) { return std::move(init) + val; });
#else
        return this->foldl(value_type(), std::plus<>());
#endif // LZ_HAS_CXX_11
    }

    /**
     * Gets the max value of the current iterator view.
     * @param binary_predicate The comparer. operator< is assumed by default.
     * @return The max element.
     */
    template<class BinaryPredicate = MAKE_BIN_OP(std::less, value_type)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference max(BinaryPredicate binary_predicate = {}) const {
        LZ_ASSERT(!lz::empty(*this), "sequence cannot be empty in order to get max element");
        return *lz::max_element(*this, std::move(binary_predicate));
    }

    /**
     * Gets the min value of the current iterator view.
     * @param cmp The comparer. operator< is assumed by default.
     * @return The min element.
     */
    template<class BinaryPredicate = MAKE_BIN_OP(std::less, value_type)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference min(BinaryPredicate binary_predicate = {}) const {
        LZ_ASSERT(!lz::empty(*this), "sequence cannot be empty in order to get min element");
        return *lz::min_element(*this, std::move(binary_predicate));
    }

    //! See Algorithm.hpp for documentation
    template<class BinaryPredicate = MAKE_BIN_OP(std::plus, value_type)>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 double mean(BinaryPredicate binary_predicate = {}) const {
        return lz::mean(*this, std::move(binary_predicate));
    }

    /**
     * Checks if all of the elements meet the condition `unary_predicate`. `unary_predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param unary_predicate The function that checks if an element meets a certain condition.
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool all(UnaryPredicate unary_predicate) const {
        return lz::all_of(*this, std::move(unary_predicate));
    }

    /**
     * Checks if any of the elements meet the condition `unary_predicate`. `unary_predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param unary_predicate The function that checks if an element meets a certain condition.
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool any(UnaryPredicate unary_predicate) const {
        return lz::any_of(*this, std::move(unary_predicate));
    }

    /**
     * Checks if none of the elements meet the condition `unary_predicate`. `unary_predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param unary_predicate The function that checks if an element meets a certain condition.
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool none(UnaryPredicate unary_predicate) const {
        return lz::none_of(*this, std::move(unary_predicate));
    }

    /**
     * Counts how many occurrences of `value` are in this.
     * @param value The value to count
     * @return The amount of counted elements equal to `value`.
     */
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type count(const T& value) const {
        return lz::count(*this, value);
    }

    /**
     * Counts how many occurrences times the unary predicate returns true.
     * @param predicate The function predicate that must return a bool.
     * @return The amount of counted elements.
     */
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type count_if(UnaryPredicate unary_predicate) const {
        return lz::count_if(*this, std::move(unary_predicate));
    }

    /**
     * Sorts the sequence with the default (operator<) comparer.
     * @param binary_predicate The comparer to sort the sequence with.
     * @return A reference to this.
     */
    template<class BinaryPredicate = MAKE_BIN_OP(std::less, value_type)>
    LZ_CONSTEXPR_CXX_20 chain_iterable<Iterator, S>& sort(BinaryPredicate binary_predicate = {}) {
        // Use std sort, it needs to be random access iterator anyway
        std::sort(Base::begin(), Base::end(), std::move(binary_predicate));
        return *this;
    }

    /**
     * Checks whether the sequence is sorted, using the standard (operator<) compare.
     * @param binary_predicate The comparer to check if the sequence is sorted.
     * @return True if the sequence is sorted given by the `predicate` false otherwise.
     */
    template<class BinaryPredicate = MAKE_BIN_OP(std::less, value_type)>
    bool is_sorted(BinaryCompare binary_predicate = {}) const {
        return lz::is_sorted(*this, std::move(binary_predicate));
    }
};
} // namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif // LZ_LZ_HPP