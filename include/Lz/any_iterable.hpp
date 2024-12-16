#pragma once

#ifndef LZ_ANY_VIEW_HPP
#define LZ_ANY_VIEW_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/any_view_helpers.hpp"
#include "Lz/detail/iterators/anyview/any_impl.hpp"
#include "Lz/detail/procs.hpp"

#include <iterator>
#include <memory>

namespace lz {
/**
 * @brief Class that can contain any type of view. For example: a container or another view. Use this when you cannot use `auto`.
 * Please be aware that this implementation uses type erasure and therefore is a little bit slower than using `auto`/specifying
 * the "actual" type. For e.g.
 * @code
 * // Preferred:
 * lz::filter_iterable<std::vector<int>::iterator, lambdafunction> f = lz::filter( // stuff...
 * // Or
 * auto f = lz::filter( // stuff
 * // Versus (slower):
 * lz::any_iterable<int> f = lz::filter( // stuff...
 * @endcode
 *
 * @tparam T The value_type of the iterator. For example: std::vector<int>::value_type = int, which requires a
 * lz::any_iterable<int>
 * @tparam Reference The reference of the iterator. In most cases T&, but with generative iterators it's oftentimes just T.
 * @tparam IterCat The iterator category. `std::forward_iterator_tag` by default.
 * @tparam DiffType The difference_type. It is used for the return type of iterator - iterator
 */
template<class T, class Reference = T&, class IterCat = std::forward_iterator_tag, class DiffType = std::ptrdiff_t>
class any_iterable final : public detail::basic_iterable<detail::iterator_wrapper<T, Reference, IterCat, DiffType>> {
private:
    using it = detail::iterator_wrapper<T, Reference, IterCat, DiffType>;
    using base = detail::basic_iterable<It>;

    template<class Iterable>
    using any_iter_impl = detail::any_iterator_impl<decltype(detail::begin(std::forward<View>(std::declval<Iterable>()))), T,
                                                    Reference, IterCat, DiffType>;

public:
    any_iterable() = default;

    /**
     * @brief Construct a new any_iterable object
     *
     * @param iterable Any iterable, like a vector, list, etc. Can also be another lz range/view
     */
    template<class Iterable>
    any_iterable(Iterable&& iterable) :
        base(it(std::make_shared<any_iter_impl<Iterable>>(detail::begin(std::forward<Iterable>(view)))),
             it(std::make_shared<any_iter_impl<Iterable>>(detail::end(std::forward<Iterable>(view))))) {
    }
};
} // namespace lz

#endif // LZ_ANY_VIEW_HPP
