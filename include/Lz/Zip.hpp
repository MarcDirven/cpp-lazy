#pragma once

#ifndef LZ_ZIP_HPP
#define LZ_ZIP_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/ZipIterator.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR... Iterators>
    class Zip final : public detail::BasicIteratorView<detail::ZipIterator<Iterators...>> {
    public:
        using iterator = detail::ZipIterator<Iterators...>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin;
        iterator _end{};

    public:
        /**
         * @brief This object can be used to iterate over multiple containers. It stops at its smallest container.
         * Its `begin()` function returns a random access iterator. The operators `<, <=, >, >=` will return true
         * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/
         * `operator>=`.
         * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
         * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
         * unless the iterator is const, making it a const reference.
         * to alter the values in the iterator (and therefore also the container/iterable), unless the iterator is const,
         * making it a const reference.
         * @param containers
         */
        explicit Zip(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) :
            _begin(begin),
            _end(end) {
        }

        Zip() = default;

        /**
         * @brief Returns the beginning of the zip iterator.
         * @return The beginning of the zip iterator.
         */
        iterator begin() const override {
            return _begin;
        }

        /**
         * @brief Returns the ending of the zip iterator.
         * @return The ending of the zip iterator.
         */
        iterator end() const override {
            return _end;
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    template<LZ_CONCEPT_ITERATOR... Iterators>
    Zip<Iterators...> zipRange(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) {
        return Zip<Iterators...>(begin, end);
    }

    /**
     * @brief This function can be used to iterate over multiple containers. It stops at its smallest container.
     * Its `begin()` function returns a random access iterator. The operators `<, <=, >, >=` will return true
     * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/`operator>=`.
     * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
     * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
     * unless the iterator is const, making it a const reference.
     * @tparam Iterables Is automatically deduced.
     * @param iterables The iterables to iterate simultaneously over.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto tuple :  lz::zip(...))`.
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    Zip<detail::IterType<Iterables>...> zip(Iterables&& ... iterables) {
        return zipRange(std::make_tuple(std::begin(iterables)...), std::make_tuple(std::end(iterables)...));
    }

    // End of group
    /**
     * @}
     */
}

#endif