#pragma once


#include <array>
#include <vector>

#include <Lz/detail/SplitIterator.hpp>
#include <Lz/detail/LzTools.hpp>


namespace lz {
    template<class SubString = std::string>
    class StringSplitter {
    public:
        using iterator = detail::SplitIterator<SubString>;
        using const_iterator = iterator;

    private:
        const std::string& _string = std::string();
        std::string _delimiter{};

    public:
        using value_type = SubString;

        StringSplitter(const std::string& str, std::string&& delimiter) :
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
        return StringSplitter<SubString>(str, std::move(delimiter));
    }
}
