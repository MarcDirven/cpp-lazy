#pragma once


#include <Lz/detail/ConcatenateIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<LZ_CONCEPT_ITERATOR... Iterators>
    class Concatenate final : public internal::BasicIteratorView<internal::ConcatenateIterator<Iterators...>> {
    public:
        using iterator = internal::ConcatenateIterator<Iterators...>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        Concatenate(const Iterator1 beginIterator1, const Iterator1 endIterator1, const Iterator2 beginIterator2,
                    const Iterator2 endIterator2) :
            _begin(beginIterator1, endIterator1, beginIterator2, endIterator2),
            _end(endIterator1, endIterator1, endIterator2, endIterator2) {
        }

        iterator begin() const override {
            return _begin;
        }

        iterator end() const override {
            return _end;
        }
    };

    template<class Iterator1, class Iterator2>
    auto concatrange(Iterator1 begin1, Iterator1 end1, Iterator2 begin2, Iterator2 end2) {
        using ValueType1 = typename std::iterator_traits<Iterator1>::value_type;
        using ValueType2 = typename std::iterator_traits<Iterator2>::value_type;
        using Pointer1 = typename std::iterator_traits<Iterator1>::pointer;
        using Pointer2 = typename std::iterator_traits<Iterator2>::pointer;
        using Reference1 = typename std::iterator_traits<Iterator1>::reference;
        using Reference2 = typename std::iterator_traits<Iterator2>::reference;

    /**
     * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the tuple must be greater than
     * or equal to 2.
     * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
     * @param begin A tuple of iterators pointing to the beginning.
     * @param end A tuple of iterators pointing to the ending.
     * @return A concatenate view object, which contains the random access iterator, that can be used to iterate over.
     */
    template<LZ_CONCEPT_ITERATOR... Iterators>
    Concatenate<Iterators...> concatRange(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end) {
        static_assert(sizeof...(Iterators) >= 2, "amount of iterators/containers cannot be less than or equal to 1");
        static_assert(internal::IsAllSame<typename std::iterator_traits<Iterators>::value_type...>::value,
                      "value types of iterators do not match");
        static_assert(internal::IsAllSame<typename std::iterator_traits<Iterators>::pointer...>::value,
                      "pointer types of iterators do not match");
        static_assert(internal::IsAllSame<typename std::iterator_traits<Iterators>::reference...>::value,
                      "reference types of iterators do not match");

        return Concatenate<Iterator1, Iterator2>(begin1, end1, begin2, end2);
    }

    /**
     * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the parameter pack must be
     * greater than or equal to 2.
     * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
     * @param iterables A parameter pack of containers/iterables.
     * @return A concatenate view object, which contains the random access iterator, that can be used to iterate over.
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    Concatenate<internal::IterType<Iterables>...> concat(Iterables&& ... iterables) {
        return concatRange(std::make_tuple(std::begin(iterables)...), std::make_tuple(std::end(iterables)...));
    }

    // End of group
    /**
     * @}
     */
} // end lz

#endif