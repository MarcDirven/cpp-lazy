#pragma once

#include <algorithm>
#include <string>
#include <array>
#include <vector>


namespace detail {
    template<class SubString>
    class SplitIterator {
        std::string _delimiter{};
        const std::string& _string = std::string();
        size_t _iterIndex{};
        size_t _current{};
        SubString _substring{};

    public:
        using reference = typename std::conditional<__cplusplus < 201703L, const SubString&, SubString>::type;
        using pointer = const SubString*;
        using iterator_category = std::input_iterator_tag;
        using value_type = SubString;
        using difference_type = std::string::const_iterator::difference_type;

        SplitIterator(size_t iterIndex, const std::string& string, std::string delimiter) :
            _delimiter(std::move(delimiter)),
            _string(string),
            _iterIndex(iterIndex) {
        }

        void find() {
            _current = _string.find(_delimiter, _iterIndex);

            if (_current == std::string::npos) {
                _substring = SubString(&_string[_iterIndex]);
            }
            else {
                _substring = SubString(&_string[_iterIndex], _current - _iterIndex);
                _iterIndex = _current + _delimiter.length();
            }
        }

        reference operator*() const {
            return _substring;
        }

        pointer operator->() const {
            return &_substring;
        }

        bool operator!=(const SplitIterator& other) const {
            return _iterIndex != other._iterIndex;
        }

        bool operator==(const SplitIterator& other) const {
            return !(*this != other);
        }

        SplitIterator& operator++() {
            _iterIndex = _current == std::string::npos ? _string.size() : _iterIndex;
            find();
            return *this;
        }

        SplitIterator& operator+=(const difference_type offset) {
            for (difference_type i = 0; i < offset; i++, ++*this) {}
            return *this;
        }

        SplitIterator operator+(const difference_type offset) const {
            auto tmp = *this;
            return tmp += offset;
        }
    };
}

namespace lz {
    template<class SubString = std::string>
    class SplitObject {
    public:
        using iterator = detail::SplitIterator<SubString>;
        using const_iterator = iterator;

    private:
        const std::string& _string = std::string();
        std::string _delimiter{};

    public:
        using value_type = std::string;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = std::string*;
        using const_pointer = const std::string*;
        using reference = std::string&;
        using const_reference = const std::string&;

        SplitObject(const std::string& str, std::string&& delimiter) :
            _string(str),
            _delimiter(std::move(delimiter)) {
        }

        const_iterator begin() const {
            const_iterator it(0, _string, _delimiter);
            it.find();
            return it;
        }

        const_iterator end() const {
            const_iterator it(_string.size(), _string, _delimiter);
            return it;
        }

        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to() const {
            return Container<value_type, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return to<std::vector>();
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container;
            detail::fillContainer(begin(), container);
            return container;
        }
    };

#if __cplusplus < 201703L // < C++17

    template<class SubString = std::string>
#else
    template<class SubString = std::string_view>
#endif
    auto split(const std::string& str, std::string delimiter) {
        return SplitObject<SubString>(str, std::move(delimiter));
    }
}
