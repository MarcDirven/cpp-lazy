#pragma once

#include <type_traits>
#include <algorithm>


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class FilterIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using reference = typename std::iterator_traits<Iterator>::reference;

    private:
        Iterator _iterator{};
        Iterator _end{};
        Function _function{};

    public:
        FilterIterator(Iterator begin, Iterator end, Function function) :
            _iterator(begin),
            _end(end),
            _function(function) {
            _iterator = std::find_if(_iterator, _end, _function);
        }

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return _iterator.operator->();
        }

        FilterIterator& operator++() {
            if (_iterator != _end) {
                _iterator = std::find_if(_iterator + 1, _end, _function);
            }
            return *this;
        }

        bool operator!=(const FilterIterator& other) const {
            return _iterator != other._end;
        }

        bool operator==(const FilterIterator& other) const {
            return !(*this != other);
        }
    };
}}