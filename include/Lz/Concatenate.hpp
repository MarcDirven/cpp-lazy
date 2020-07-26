#pragma once


#include <Lz/detail/ConcatenateIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class Iterator1, class Iterator2>
    class Concatenate : public detail::BasicIteratorView<detail::ConcatenateIterator<Iterator1, Iterator2>> {
    public:
        using iterator = detail::ConcatenateIterator<Iterator1, Iterator2>;
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

        static_assert(std::is_same<ValueType1, ValueType2>::value, "value types of the iterators do not match");
        static_assert(std::is_same<Pointer1, Pointer2>::value, "pointer types of the iterators do not match");
        static_assert(std::is_same<Reference1, Reference2>::value, "reference types of the iterators do not match");

        return Concatenate<Iterator1, Iterator2>(begin1, end1, begin2, end2);
    }

    template<class Iterable1, class Iterable2>
    auto concat(Iterable1&& a, Iterable2&& b) {
        return concatrange(std::begin(a), std::end(a), std::begin(b), std::end(b));
    }
}