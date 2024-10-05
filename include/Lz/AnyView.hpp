#pragma once

#ifndef LZ_ANY_VIEW_HPP
#define LZ_ANY_VIEW_HPP

#include "Lz/detail/BasicIteratorView.hpp"
#include "Lz/detail/Procs.hpp"
#include "Lz/detail/iterators/AnyViewHelpers.hpp"

#include <iterator>
#include <memory>

namespace lz {
template<class T, class IterCat = std::forward_iterator_tag, class Reference = T&, class Pointer = T*,
         class DiffType = std::ptrdiff_t>
class AnyView final : public detail::BasicIteratorView<detail::IteratorWrapper<T, IterCat, Reference, Pointer, DiffType>> {
private:
    using It = detail::IteratorWrapper<T, IterCat, Reference, Pointer, DiffType>;
    using Base = detail::BasicIteratorView<It>;

    template<typename View>
    using AnyIteratorImplType = detail::AnyIteratorImpl<decltype(detail::begin(std::forward<View>(std::declval<View>()))), T,
                                                        IterCat, Reference, Pointer, DiffType>;

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
