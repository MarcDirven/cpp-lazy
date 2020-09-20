#pragma once

#include <vector>
#include <array>

#include "detail/BasicIteratorView.hpp"
#include "detail/ZipIterator.hpp"


namespace lz {
    template<class... Iterators>
    class Zip final : public detail::BasicIteratorView<detail::ZipIterator<Iterators...>> {
    public:
        using iterator = detail::ZipIterator<Iterators...>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        std::tuple<Iterators...> _begin{};
        std::tuple<Iterators...> _end{};

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
            _end(end)
        {
        }

        Zip() = default;

        /**
         * @brief Returns the beginning of the zip iterator.
         * @return The beginning of the zip iterator.
         */
        iterator begin() const override {
            return iterator(_begin);
        }

        /**
         * @brief Returns the ending of the zip iterator.
         * @return The ending of the zip iterator.
         */
        iterator end() const override {
            return iterator(_end);
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    template<class... Iterators>
    Zip<Iterators...> ziprange(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) {
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
    template<class... Iterables>
    auto zip(Iterables&& ... iterables) -> Zip<decltype(std::begin(iterables))...> {
        return ziprange(std::make_tuple(std::begin(iterables)...), std::make_tuple(std::end(iterables)...));
    }

    // End of group
    /**
     * @}
     */
}
