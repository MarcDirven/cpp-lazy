#pragma once

#ifndef LZ_C_STRING_HPP
#define LZ_C_STRING_HPP

#include "detail/BasicIterable.hpp"
#include "detail/Traits.hpp"
#include "detail/iterators/CStringIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class C, class Tag>
class CString final
    : public detail::BasicIterable<detail::CStringIterator<C, Tag>, typename detail::CStringIterator<C, Tag>::Sentinel> {
    using Sentinel = typename detail::CStringIterator<C, Tag>::Sentinel;

public:
    using iterator = detail::CStringIterator<C, Tag>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr CString(const C* begin) noexcept : detail::BasicIterable<iterator, Sentinel>(iterator(begin)) {
    }

    constexpr CString(const C* begin, const C* end) noexcept :
        detail::BasicIterable<iterator, iterator>(iterator(begin), iterator(end)) {
    }

    constexpr CString() = default;

    template<class S = Sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 detail::EnableIf<!std::is_same<detail::CStringIterator<C, S>, S>::value, std::size_t>
    size() const noexcept {
        std::size_t s = 0;
        for (auto i = this->begin(); i != this->end(); ++i, ++s) {
        }
        return s;
    }

    template<class S = Sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 detail::EnableIf<std::is_same<detail::CStringIterator<C, S>, S>::value, std::size_t>
    size() const noexcept {
        return static_cast<std::size_t>(this->_end - this->_begin);
    }

    LZ_NODISCARD constexpr explicit operator bool() const noexcept {
        return static_cast<bool>(this->begin());
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Creates a c-string iterator with knowing its length
 * 
 * @param begin Beginning of the c string
 * @param end Ending of the c string
 * @return CString object containing a random access iterator
 */
template<class C>
LZ_NODISCARD constexpr CString<C, std::random_access_iterator_tag> cString(const C* begin, const C* end) noexcept {
    return { begin, end };
}

/**
 * Creates a c-string iterator without knowing its length
 * 
 * @param s The c string
 * @return CString object containing a forward iterator
 */
template<class C>
LZ_NODISCARD constexpr CString<C, std::forward_iterator_tag> cString(const C* s) noexcept {
    return { s };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif