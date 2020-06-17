#pragma once


#include <array>
#include <vector>
#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L) // < C++17
    #include <string>
#else
    #include <string_view>
#endif

#include <Lz/detail/SplitIterator.hpp>
#include <Lz/detail/LzTools.hpp>


namespace lz {
    template<class SubString>
    class StringSplitter {
    public:
        using const_iterator = detail::SplitIterator<SubString>;

    private:
        const std::string& _string = std::string();
        std::string _delimiter{};

    public:
        using value_type = std::string;

        /**
         * @brief Creates a String splitter object. Its `begin()` and `end()` return an forward iterator.
         * @param str The string to split.
         * @param delimiter The delimiter to split on.
         */
        StringSplitter(const std::string& str, std::string&& delimiter) :
            _string(str),
            _delimiter(std::move(delimiter)) {
        }

        /**
         * @brief Returns a forward string split iterator to the beginning.
         * @return A forward string split iterator to the beginning.
         */
        const_iterator begin() const {
            const_iterator it(0, _string, _delimiter);
            it.find();
            return it;
        }

        /**
         * @brief Returns a forward string split iterator to the ending.
         * @return A forward string split iterator to the ending.
         */
        const_iterator end() const {
            const_iterator it(_string.size(), _string, _delimiter);
            return it;
        }

        /**
         * @brief Creates a container from this split object. The container will consists of i.e.:
         * `SomeContainer<std::string>` with the substrings.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing `std::string` or `std::string_view`.
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type ContainerType<std::string[,Args...]>.
         */
        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to(Args&&... args) const {
            return Container<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        /**
        * @brief Creates a `std::vector<std::string>` with default `std::allocator` with the substrings in it.
        * @return A `std::vector<std::string>` with the with the substrings in it and a default `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<std::string>` with a specified Allocator and with the substrings in it.
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<std::string, Allocator>` with a specified Allocator and with the substrings in it.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<std::string, N>` with the substrings in it.
         * @tparam N The size of the array.
         * @return A `std::array<std::string, N>` with the substrings in it.
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container;
            detail::fillContainer(begin(), container);
            return container;
        }
    };

#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L) // < C++17
    template<class SubString = std::string>
#else
    template<class SubString = std::string_view>
#endif
    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief This is a lazy evaluated string splitter function. If not using C++17 or higher, you can use `std::move`
     * to safely move the substring, that is returned by the
     * `StringSplitter<SubString>::const_iterator::operator*`. Its `begin()` and `end()` return an forward iterator.
     * @tparam SubString The type that gets returned when the `StringSplitter<SubString>::const_iterator::operator*` is
     * called. Can be specified, but if C++17 or higher is defined, `std::string_view` is used, otherwise `std::string`.
     * @param str The string to split.
     * @param delimiter The delimiter to split on.
     * @return A stringSplitter object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::split(...))`.
     */
    auto split(const std::string& str, std::string delimiter) {
        return StringSplitter<SubString>(str, std::move(delimiter));
    }

    // End of group
    /**
     * @}
     */
}
