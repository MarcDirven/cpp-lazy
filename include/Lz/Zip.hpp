#pragma once

#include <vector>
#include <array>

#include <Lz/detail/BasicIteratorView.hpp>
#include <Lz/detail/ZipIterator.hpp>


namespace lz {
    template<LZ_CONCEPT_ITERATOR... Iterators>
    class Zip final : public internal::BasicIteratorView<internal::ZipIterator<Iterators...>> {
    public:
        using iterator = internal::ZipIterator<Iterators...>;
        using const_iterator = iterator;

    private:
        iterator _begin;
        iterator _end;

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
        explicit Zip(Containers&& ... containers) :
            _begin(std::make_tuple(std::begin(containers)...)),
            _end(std::make_tuple(std::end(containers)...))
        {
        }

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

    /**
     * @brief This function can be used to iterate over multiple containers. It stops at its smallest container.
     * Its `begin()` function returns a random access iterator. The operators `<, <=, >, >=` will return true
     * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/`operator>=`.
     * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
     * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
     * unless the iterator is const, making it a const reference.
     * @param iterables The iterables to iterate simultaneously over.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto tuple :  lz::zip(...))`.
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    Zip<internal::IterType<Iterables>...> zip(Iterables&& ... iterables) {
        return zipRange(std::make_tuple(std::begin(iterables)...), std::make_tuple(std::end(iterables)...));
    }

    // End of group
    /**
     * @}
     */
}
