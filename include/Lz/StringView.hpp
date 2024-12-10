#pragma once

#ifndef LZ_STRING_VIEW_HPP
#define LZ_STRING_VIEW_HPP

#include "detail/CompilerChecks.hpp"

#ifdef LZ_HAS_STRING_VIEW
#include <string_view>
#else
#include <cstddef>
#include <ostream>
#endif

#if !defined(LZ_HAS_STRING_VIEW) && !defined(LZ_STANDALONE)
#include <fmt/core.h>
#endif

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#if defined(LZ_HAS_STRING_VIEW)
template<class C>
using BasicStringView = std::basic_string_view<C>;

using StringView = std::string_view;
#elif defined(LZ_STANDALONE)
template<class CharT>
class BasicStringView {
public:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    BasicStringView() = default;

    constexpr BasicStringView(const CharT* data, std::size_t size) noexcept : _data(data), _size(size) {
    }

    LZ_CONSTEXPR_CXX_17 BasicStringView(const CharT* data) noexcept :
        _data(data),
        _size(std::char_traits<CharT>::length(data)) {
    }

    constexpr BasicStringView(const CharT* begin, const CharT* end) noexcept :
        _data(begin),
        _size(static_cast<std::size_t>(end - begin)) {
    }

    constexpr const CharT* data() const noexcept {
        return _data;
    }

    constexpr std::size_t size() const noexcept {
        return _size;
    }

    constexpr const CharT* begin() const noexcept {
        return _data;
    }

    constexpr const CharT* end() const noexcept {
        return _data + _size;
    }

    constexpr std::size_t length() const noexcept {
        return _size;
    }

    constexpr const CharT& operator[](const std::size_t index) const {
        return _data[index];
    }

    constexpr BasicStringView substr(const std::size_t pos, const std::size_t count) const noexcept {
        return { _data + pos, count };
    }

    constexpr BasicStringView substr(const std::size_t pos) const noexcept {
        return { _data + pos, _size - pos };
    }

    constexpr bool empty() const noexcept {
        return _size == 0;
    }

    constexpr const CharT& front() const {
        return _data[0];
    }

    constexpr const CharT& back() const {
        return _data[_size - 1];
    }

    LZ_CONSTEXPR_CXX_14 void remove_prefix(const std::size_t n) noexcept {
        _data += n;
        _size -= n;
    }

    LZ_CONSTEXPR_CXX_14 void remove_suffix(const std::size_t n) noexcept {
        _size -= n;
    }

    LZ_CONSTEXPR_CXX_17 bool contains(const BasicStringView str) const noexcept {
        return find(str) != npos;
    }

    void swap(BasicStringView& other) noexcept {
        std::swap(_data, other._data);
        std::swap(_size, other._size);
    }

    std::basic_string<CharT> toStdString() const {
        return { _data, _size };
    }

    explicit operator std::basic_string<CharT>() const {
        return toStdString();
    }

    LZ_CONSTEXPR_CXX_17 std::size_t find(const BasicStringView str, std::size_t pos = 0) const noexcept {
        if (pos > _size) {
            return npos;
        }
        const auto result = std::char_traits<CharT>::find(_data + pos, _size - pos, str.front());
        if (result == nullptr) {
            return npos;
        }
        return static_cast<std::size_t>(result - _data);
    }

    LZ_CONSTEXPR_CXX_17 std::size_t find(const char c, std::size_t pos = 0) const noexcept {
        return std::char_traits<CharT>::find(_data + pos, _size - pos, c) - _data;
    }

private:
    const CharT* _data{ nullptr };
    std::size_t _size{};
};

// Equality operator
template<class CharT>
inline bool operator==(const BasicStringView<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return lhs.size() == rhs.size() && std::char_traits<CharT>::compare(lhs.data(), rhs.data(), lhs.size()) == 0;
}

template<class CharT>
inline bool operator==(const BasicStringView<CharT>& lhs, const std::basic_string<CharT>& rhs) {
    return lhs == BasicStringView<CharT>(rhs.c_str(), rhs.size());
}

template<class CharT>
inline bool operator==(const std::basic_string<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return BasicStringView<CharT>(lhs.c_str(), lhs.size()) == rhs;
}

template<class CharT>
inline bool operator==(const BasicStringView<CharT>& lhs, const CharT* rhs) {
    return lhs == BasicStringView<CharT>(rhs);
}

template<class CharT>
inline bool operator==(const CharT* lhs, const BasicStringView<CharT>& rhs) {
    return BasicStringView<CharT>(lhs) == rhs;
}

// Inequality operator
template<class CharT>
inline bool operator!=(const BasicStringView<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return !(lhs == rhs);
}

template<class CharT>
inline bool operator!=(const BasicStringView<CharT>& lhs, const std::basic_string<CharT>& rhs) {
    return !(lhs == rhs);
}

template<class CharT>
inline bool operator!=(const std::basic_string<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return !(lhs == rhs);
}

template<class CharT>
inline bool operator!=(const BasicStringView<CharT>& lhs, const CharT* rhs) {
    return !(lhs == rhs);
}

template<class CharT>
inline bool operator!=(const CharT* lhs, const BasicStringView<CharT>& rhs) {
    return !(lhs == rhs);
}

// Less than operator
template<class CharT>
inline bool operator<(const BasicStringView<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    const auto cmp = std::char_traits<CharT>::compare(lhs.data(), rhs.data(), std::min(lhs.size(), rhs.size()));
    return cmp < 0 || (cmp == 0 && lhs.size() < rhs.size());
}

template<class CharT>
inline bool operator<(const BasicStringView<CharT>& lhs, const std::basic_string<CharT>& rhs) {
    return lhs < BasicStringView<CharT>(rhs.c_str(), rhs.size());
}

template<class CharT>
inline bool operator<(const std::basic_string<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return BasicStringView<CharT>(lhs.c_str(), lhs.size()) < rhs;
}

template<class CharT>
inline bool operator<(const BasicStringView<CharT>& lhs, const CharT* rhs) {
    return lhs < BasicStringView<CharT>(rhs);
}

template<class CharT>
inline bool operator<(const CharT* lhs, const BasicStringView<CharT>& rhs) {
    return BasicStringView<CharT>(lhs) < rhs;
}

// Greater than operator
template<class CharT>
inline bool operator>(const BasicStringView<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return rhs < lhs;
}

template<class CharT>
inline bool operator>(const BasicStringView<CharT>& lhs, const std::basic_string<CharT>& rhs) {
    return rhs < lhs;
}

template<class CharT>
inline bool operator>(const std::basic_string<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return rhs < lhs;
}

template<class CharT>
inline bool operator>(const BasicStringView<CharT>& lhs, const CharT* rhs) {
    return BasicStringView<CharT>(rhs) < lhs;
}

template<class CharT>
inline bool operator>(const CharT* lhs, const BasicStringView<CharT>& rhs) {
    return rhs < lhs;
}

// Less than or equal operator
template<class CharT>
inline bool operator<=(const BasicStringView<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return !(rhs < lhs);
}

template<class CharT>
inline bool operator<=(const BasicStringView<CharT>& lhs, const std::basic_string<CharT>& rhs) {
    return !(rhs < lhs);
}

template<class CharT>
inline bool operator<=(const std::basic_string<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return !(rhs < lhs);
}

template<class CharT>
inline bool operator<=(const BasicStringView<CharT>& lhs, const CharT* rhs) {
    return !(BasicStringView<CharT>(rhs) < lhs);
}

template<class CharT>
inline bool operator<=(const CharT* lhs, const BasicStringView<CharT>& rhs) {
    return !(rhs < lhs);
}

// Greater than or equal operator
template<class CharT>
inline bool operator>=(const BasicStringView<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return !(lhs < rhs);
}

template<class CharT>
inline bool operator>=(const BasicStringView<CharT>& lhs, const std::basic_string<CharT>& rhs) {
    return !(lhs < rhs);
}

template<class CharT>
inline bool operator>=(const std::basic_string<CharT>& lhs, const BasicStringView<CharT>& rhs) {
    return !(lhs < rhs);
}

template<class CharT>
inline bool operator>=(const BasicStringView<CharT>& lhs, const CharT* rhs) {
    return !(lhs < BasicStringView<CharT>(rhs));
}

template<class CharT>
inline bool operator>=(const CharT* lhs, const BasicStringView<CharT>& rhs) {
    return !(lhs < rhs);
}

template<class CharT>
constexpr std::size_t BasicStringView<CharT>::npos;

using StringView = BasicStringView<char>;

template<typename CharT>
std::ostream& operator<<(std::ostream& os, const lz::BasicStringView<CharT> view) {
    return os.write(view.data(), static_cast<std::streamsize>(view.size()));
}
#else

template<class C>
using BasicStringView = fmt::basic_string_view<C>;

using StringView = fmt::string_view;
#endif

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz
#endif