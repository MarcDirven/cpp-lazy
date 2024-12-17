#pragma once

#ifndef LZ_C_STRING_HPP
#define LZ_C_STRING_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/c_string.hpp"
#include "detail/traits.hpp"


namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class C, class Tag>
class c_string_iterable final
    : public detail::basic_iterable<detail::c_string_iterator<C, Tag>, typename detail::c_string_iterator<C, Tag>::sentinel> {
    using s = typename detail::c_string_iterator<C, Tag>::sentinel;

public:
    using iterator = detail::c_string_iterator<C, Tag>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr c_string_iterable(const C* begin) noexcept : detail::basic_iterable<iterator, default_sentinel>(iterator(begin)) {
    }

    constexpr c_string_iterable(const C* begin, const C* end) noexcept :
        detail::basic_iterable<iterator, iterator>(iterator(begin), iterator(end)) {
    }

    constexpr c_string_iterable() = default;

    template<class S = s>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 detail::enable_if<!std::is_same<iterator, S>::value, std::size_t> size() const noexcept {
        std::size_t s = 0;
        for (auto i = this->begin(); i != this->end(); ++i, ++s) {
        }
        return s;
    }

    template<class S = s>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 detail::enable_if<std::is_same<iterator, S>::value, std::size_t> size() const noexcept {
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
LZ_NODISCARD constexpr c_string_iterable<C, std::random_access_iterator_tag> c_string(const C* begin, const C* end) noexcept {
    return { begin, end };
}

/**
 * Creates a c-string iterator without knowing its length
 *
 * @param s The c string
 * @return CString object containing a forward iterator
 */
template<class C>
LZ_NODISCARD constexpr c_string_iterable<C, std::forward_iterator_tag> c_string(const C* s) noexcept {
    return { s };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif