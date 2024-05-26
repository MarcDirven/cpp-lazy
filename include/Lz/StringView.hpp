#pragma once

#ifndef LZ_STRING_VIEW_HPP
#define LZ_STRING_VIEW_HPP

#include "detail/LzTools.hpp"

#ifdef LZ_HAS_STRING_VIEW
#include <string_view>
#else
#include <ostream>
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

    LZ_CONSTEXPR_CXX_17 BasicStringView(const CharT* data) noexcept : _data(data), _size(std::char_traits<CharT>::length(data)) {
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
        return std::basic_string<CharT>(_data, _size);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator==(const BasicStringView other) const noexcept {
        return _size == other._size && std::char_traits<CharT>::compare(_data, other._data, _size) == 0;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator!=(const BasicStringView other) const noexcept {
        return !(*this == other);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator<(const BasicStringView other) const noexcept {
        return std::char_traits<CharT>::compare(_data, other._data, std::min(_size, other._size)) < 0;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator>(const BasicStringView other) const noexcept {
        return other < *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator<=(const BasicStringView other) const noexcept {
        return !(*this > other);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator>=(const BasicStringView other) const noexcept {
        return !(*this < other);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator==(const std::basic_string<CharT>& str) const {
        return *this == BasicStringView(str.c_str(), str.size());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator!=(const std::basic_string<CharT>& str) const {
        return !(*this == str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator<(const std::basic_string<CharT>& str) const {
        return *this < BasicStringView(str.c_str(), str.size());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator>(const std::basic_string<CharT>& str) const {
        return *this > BasicStringView(str.c_str(), str.size());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator<=(const std::basic_string<CharT>& str) const {
        return !(*this > str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator>=(const std::basic_string<CharT>& str) const {
        return !(*this < str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator==(const CharT* str) const noexcept {
        return *this == BasicStringView(str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator!=(const CharT* str) const noexcept {
        return !(*this == str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator<(const CharT* str) const noexcept {
        return *this < BasicStringView(str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator>(const CharT* str) const noexcept {
        return *this > BasicStringView(str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator<=(const CharT* str) const noexcept {
        return !(*this > str);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool operator>=(const CharT* str) const noexcept {
        return !(*this < str);
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

using StringView = BasicStringView<char>;

template<typename CharT>
std::ostream& operator<<(std::ostream& os, const lz::BasicStringView<CharT> view) {
    return os.write(view.data(), view.size());
}
#else

template<class C>
using BasicStringView = fmt::basic_string_view<C>;

using StringView = fmt::string_view;
#endif

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz
#endif