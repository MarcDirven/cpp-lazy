#pragma once

#ifndef LZ_STRING_SPLITTER_HPP
#define LZ_STRING_SPLITTER_HPP

#include "CString.hpp"
#include "detail/iterators/SplitIterator.hpp"

namespace lz {

namespace detail {

template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringView toStringView(const View<Iterator, S>& view, std::random_access_iterator_tag) {
    return StringView(&*(view.begin()), static_cast<std::size_t>(view.distance()));
}

template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringView toStringView(const View<Iterator, S>& view, std::forward_iterator_tag) {
    return StringView(&*(view.begin()));
}

} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, class Iterator2, class S2>
class Splitter final : public detail::BasicIterable<detail::SplitIterator<Iterator, S, Iterator2, S2>, DefaultSentinel> {
public:
    using const_iterator = detail::SplitIterator<Iterator, S, Iterator2, S2>;
    using iterator = const_iterator;

public:
    using value_type = detail::BasicIterable<iterator>;

    LZ_CONSTEXPR_CXX_20 Splitter(Iterator begin, S end, Iterator2 begin2, S2 end2) :
        detail::BasicIterable<iterator, DefaultSentinel>(iterator(begin, end, begin2, end2)) {
    }

    Splitter() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

template<class Iterable, class CharT>
using StringSplitter =
    Splitter<IterT<Iterable>, SentinelT<Iterable>, detail::CStringIterator<CharT, std::forward_iterator_tag>, DefaultSentinel>;

template<class Iterable, class Iterable2>
using RegularSplitter = Splitter<IterT<Iterable>, SentinelT<Iterable>, IterT<Iterable2>, SentinelT<Iterable2>>;

template<class Iterable, class Iterable2>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 RegularSplitter<IterT<Iterable>, IterT<Iterable2>>
split(Iterable&& iterable, Iterable2&& delimiter) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             detail::begin(std::forward<Iterable>(delimiter)), detail::end(std::forward<Iterable>(delimiter)) };
}

template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringSplitter<Iterable, char> split(Iterable&& iterable, const char* delimiter) {
    auto string = lz::cString(delimiter);
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             detail::begin(std::move(string)), detail::end(std::move(string)) };
}

template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 StringView toStringView(const View<Iterator, S>& view) {
    return detail::toStringView(view, IterCat<Iterator>{});
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_STRING_SPLITTER_HPP