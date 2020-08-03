#pragma once

#include <type_traits>
#include <algorithm>
#include <functional>


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class FilterIterator {
        using IterTraits = std::iterator_traits<Iterator>;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename IterTraits::value_type;
        using difference_type = typename IterTraits::difference_type;
        using pointer = typename IterTraits::pointer;
        using reference = typename IterTraits::reference;

    private:
        Iterator _iterator{};
        Iterator _end{};
        std::function<bool(value_type)> _function{};

    public:
        FilterIterator(const Iterator begin, const Iterator end, const Function& function) :
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
                _iterator = std::find_if(std::next(_iterator), _end, _function);
            }
            return *this;
        }

        FilterIterator operator++(int) {
            FilterIterator tmp(*this);
            ++*this;
            return tmp;
        }

        bool operator!=(const FilterIterator& other) const {
            return _iterator != other._end;
        }

        bool operator==(const FilterIterator& other) const {
            return !(*this != other);
        }
    };
}}