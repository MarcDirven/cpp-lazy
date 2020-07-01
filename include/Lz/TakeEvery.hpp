#pragma once

#include <iterator>
#include <Lz/detail/TakeEveryIterator.hpp>


namespace lz {
    template<class Iterator>
    class TakeEvery {
    public:
        using iterator = detail::TakeEveryIterator<Iterator>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin;
        iterator _end;

    public:
        TakeEvery(Iterator begin, Iterator end, const size_t offset):
            _begin(begin, offset),
            _end(end, offset){
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }
    };

    template<class Iterator>
    TakeEvery<Iterator> takeeveryrange(Iterator begin, Iterator end, const size_t offset, const size_t start = 0) {
        return TakeEvery<Iterator>(begin + start, end, offset);
    }

    template<class Iterable>
    auto takeevery(Iterable&& iterable, const size_t offset, const size_t start = 0) {
#ifdef _MSC_VER
        // If MSVC Compiler is the defined, the operator + of an arbitrary STL container contains a
        // _Verify_Offset(size_t) method which causes the program to crash if the amount added to the iterator is
        // past-the-end and also causing the operator>= never to be used.
        return takeeveryrange(&(*iterable.begin()), &(*(iterable.end() - 1)) + 1, offset, start);
#else
        return takeeveryrange(iterable.begin(), iterable.end(), offset, start);
#endif
    }
}