#pragma once

#include <iterator>
#include <array>
#include <Lz/it/detail/LzTools.hpp>


namespace lz { namespace it { namespace detail {
    template<size_t Repeat, class Iterator>
    class ProductIterator {
        template<std::size_t I, typename T>
        struct TupleHelperMaker {
            using Tuple = decltype(std::tuple_cat(
                std::declval<typename TupleHelperMaker<I - 1, T>::Tuple>(), std::declval<std::tuple<T>>())
            );
        };

        template<typename T>
        struct TupleHelperMaker<0, T> {
            using Tuple = std::tuple<>;
        };

        Iterator _iterator;
        Iterator _end;

    public:
        using value_type = typename TupleHelperMaker<Repeat,
                                                     typename std::iterator_traits<Iterator>::value_type>::Tuple;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using reference = typename TupleHelperMaker<Repeat,
                                                    typename std::iterator_traits<Iterator>::value_type>::Tuple;
        using pointer = FakePointerProxy<reference>;

        explicit ProductIterator(Iterator begin, Iterator end) :
            _iterator(begin),
            _end(end) {
        }

        reference operator*() {
            return reference();
        }
    };
}}}