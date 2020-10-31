#ifndef LZ_FLATTEN_HPP
#define LZ_FLATTEN_HPP

#include "detail/FlattenIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<class Iterator, int Dims>
    class Flatten : public internal::BasicIteratorView<internal::FlattenIterator<Iterator, Dims>> {
    public:
        using iterator = internal::FlattenIterator<Iterator, Dims>;
        using const_iterator = iterator;
        using value_type = typename internal::FlattenIterator<Iterator, 0>::value_type;

    private:
        using Base = internal::BasicIteratorView<iterator>;

    public:
        Flatten() = default;

        Flatten(Iterator begin, Iterator end) :
            Base(iterator(begin, end), iterator(end, end))
        {}
    };

    template<LZ_CONCEPT_ITERATOR Iterator, int Dims = internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
    Flatten<Iterator, Dims> flatten(Iterator begin, Iterator end) {
        return Flatten<Iterator, Dims>(begin, end);
    }

    template<LZ_CONCEPT_ITERABLE Iterable,
        class Iterator = internal::IterTypeFromIterable<Iterable>,
        int Dims = internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
    Flatten<Iterator, Dims> flatten(Iterable&& iterable) {
        return flatten<Iterator, Dims>(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
    }
}

#endif // LZ_FLATTEN_HPP
