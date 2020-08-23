#pragma once


#include <Lz/detail/ConcatenateIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class... Iterators>
    class Concatenate final : public detail::BasicIteratorView<detail::ConcatenateIterator<Iterators...>> {
    public:
        using iterator = detail::ConcatenateIterator<Iterators...>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        std::tuple<Iterators...> _begin{};
        std::tuple<Iterators...> _end{};

    public:
        /**
         * @brief Concatenate constructor.
         * @param beginIterator1 The beginning of the first iterator.
         * @param endIterator1 The ending of the first iterator.
         * @param beginIterator2 The beginning of the second iterator.
         * @param endIterator2 The ending of the second iterator.
         */
        Concatenate(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) :
            _begin(begin),
            _end(end) {}

        /**
         * @brief Returns the beginning of the iterator.
         * @return The beginning of the iterator.
         */
        iterator begin() const override {
            return iterator(_begin, _begin, _end);
        }

        /**
         * @brief Returns the ending of the iterator.
         * @return The ending of the iterator.
         */
        iterator end() const override {
            return iterator(_end, _begin, _end);
        }
    };

    /**
     * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the tuple must be greater than
     * or equal to 2.
     * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
     * @tparam Iterators Is automatically deduced.
     * @param begin A tuple of iterators pointing to the beginning.
     * @param end A tuple of iterators pointing to the ending.
     * @return A concatenate view object, which contains the random access iterator, that can be used to iterate over.
     */
    template<class... Iterators>
    Concatenate<Iterators...> concatrange(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) {
        return Concatenate<Iterators...>(begin, end);
    }

    /**
     * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the parameter pack must be
     * greater than or equal to 2.
     * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
     * @tparam Iterables Is automatically deduced.
     * @param iterables A parameter pack of containers/iterables.
     * @return A concatenate view object, which contains the random access iterator, that can be used to iterate over.
     */
    template<class... Iterables>
    auto concat(Iterables&& ... iterables) -> Concatenate<decltype(std::begin(iterables))...> {
        return concatrange(std::make_tuple(std::begin(iterables)...), std::make_tuple(std::end(iterables)...));
    }
}