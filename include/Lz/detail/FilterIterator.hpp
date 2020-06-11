#pragma once

#include <type_traits>
#include <algorithm>


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class FilterIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using reference = typename std::iterator_traits<Iterator>::reference;

    private:
        Iterator _begin{};
        Iterator _end{};
        Function _function{};

    public:
        FilterIterator(Iterator begin, Iterator end, Function function) :
            _begin(begin),
            _end(end),
            _function(function) {
            _begin = std::find_if(_begin, _end, _function);
        }

        reference operator*() const {
            return *_begin;
        }

        pointer operator->() const {
            return _begin.operator->();
        }

        bool operator!=(const FilterIterator& other) const {
            return _begin != other._end;
        }

        bool operator==(const FilterIterator& other) const {
            return !(*this != other);
        }

        FilterIterator& operator++() {
            if (_begin != _end) {
                _begin = std::find_if(_begin + 1, _end, _function);
            }
            return *this;
        }

        FilterIterator& operator+=(const difference_type offset) {
            _begin += offset;
            return *this;
        }

        FilterIterator operator+(const difference_type other) const {
            auto tmp = *this;
            tmp += other;
            return tmp;
        }

        FilterIterator& operator-=(const difference_type offset) {
            _begin -= offset;
            return *this;
        }

        FilterIterator operator-(const difference_type other) const {
            auto tmp = *this;
            tmp -= other;
            return tmp;
        }
    };
}}