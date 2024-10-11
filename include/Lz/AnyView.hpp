#pragma once

#ifndef LZ_ANY_VIEW_HPP
#define LZ_ANY_VIEW_HPP

#include "Lz/detail/BasicIteratorView.hpp"
#include "Lz/detail/Procs.hpp"
#include "Lz/detail/iterators/AnyViewHelpers.hpp"
#include "Lz/detail/iterators/anyview/AnyIteratorImpl.hpp"

#include <iterator>
#include <memory>

namespace lz {
/**
 * @brief Class that can contain any type of view. For example: a container or another view. Use this when you cannot use `auto`.
 * Please be aware that this implementation uses type erasure and therefore is a little bit slower than using `auto`/specifying
 * the "actual" type. For e.g.
 * @code
 * // Preferred:
 * lz::Filter<std::vector<int>::iterator, lambdafunction> f = lz::filter( // stuff...
 * // Or
 * auto f = lz::filter( // stuff
 * // Versus (slower):
 * lz::AnyRange<int> f = lz::filter( // stuff...
 * @endcode
 *
 * @tparam T The value_type of the iterator. For example: std::vector<int>::value_type = int, which requires a lz::AnyView<int>
 * @tparam Reference The reference of the iterator. In most cases T&, but with generative iterators it's oftentimes just T.
 * @tparam IterCat The iterator category. `std::forward_iterator_tag` by default.
 * @tparam DiffType The difference_type. It is used for the return type of iterator - iterator
 */
template<class T, class Reference = T&, class IterCat = std::forward_iterator_tag, class DiffType = std::ptrdiff_t>
class AnyView final : public detail::BasicIteratorView<detail::IteratorWrapper<T, Reference, IterCat, DiffType>> {
private:
    using It = detail::IteratorWrapper<T, Reference, IterCat, DiffType>;
    using Base = detail::BasicIteratorView<It>;

    template<typename View>
    using AnyIteratorImplType = detail::AnyIteratorImpl<decltype(detail::begin(std::forward<View>(std::declval<View>()))), T,
                                                        Reference, IterCat, DiffType>;

public:
    AnyView() = default;
    
    /**
     * @brief Construct a new Any View object
     * 
     * @param view Any view, like a vector, list, etc. Can also be another lz range/view
     */
    template<class View>
    AnyView(View&& view) :
        Base(It(std::make_shared<AnyIteratorImplType<View>>(detail::begin(std::forward<View>(view)))),
             It(std::make_shared<AnyIteratorImplType<View>>(detail::end(std::forward<View>(view))))) {
    }
};
} // namespace lz
#endif // LZ_ANY_VIEW_HPP
