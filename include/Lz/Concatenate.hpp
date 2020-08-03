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
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Concatenate constructor.
         * @param beginIterator1 The beginning of the first iterator.
         * @param endIterator1 The ending of the first iterator.
         * @param beginIterator2 The beginning of the second iterator.
         * @param endIterator2 The ending of the second iterator.
         */
        Concatenate(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) :
            _begin(begin, begin, end),
            _end(end, begin, end) {}

        /**
         * @brief Returns the beginning of the iterator.
         * @return The beginning of the iterator.
         */
        iterator begin() const override {
            return _begin;
        }

        /**
         * @brief Returns the ending of the iterator.
         * @return The ending of the iterator.
         */
        iterator end() const override {
            return _end;
        }
    };


    template<class... Iterators>
    Concatenate<Iterators...> concatrange(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) {
        return Concatenate<Iterators...>(begin, end);
    }

    template<class... Iterables>
    auto concat(Iterables&& ... iterables) -> Concatenate<decltype(std::begin(iterables))...> {
        return concatrange(std::make_tuple(std::begin(iterables)...), std::make_tuple(std::end(iterables)...));
    }
}