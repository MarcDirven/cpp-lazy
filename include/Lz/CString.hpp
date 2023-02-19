#pragma once

#ifndef LZ_C_STRING_HPP
#    define LZ_C_STRING_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/CStringIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class C, bool IsRandomAccess>
class CString final : public internal::BasicIteratorView<internal::CStringIterator<C, IsRandomAccess>> {
public:
    using iterator = internal::CStringIterator<C, IsRandomAccess>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr CString(const C* begin, const C* end) noexcept :
        internal::BasicIteratorView<iterator>(iterator(begin), iterator(end)) {
    }

    constexpr CString() = default;

    template<bool RA = IsRandomAccess>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 internal::EnableIf<!RA, std::size_t> size() const noexcept {
        std::size_t size = 0;
        for (auto i = this->begin(); i != this->end(); ++i, ++size) {}
        return size;
    }

    template<bool RA = IsRandomAccess>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 internal::EnableIf<RA, std::size_t> size() const noexcept {
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
LZ_NODISCARD constexpr CString<C, true> cString(const C* begin, const C* end) noexcept {
    return { begin, end };
}

/**
 * Creates a c-string iterator without knowing its length
 * 
 * @param s The c string
 * @return CString object containing a forward iterator
 */
template<class C>
LZ_NODISCARD constexpr CString<C, false> cString(const C* s) noexcept {
    return { s, nullptr };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif