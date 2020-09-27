#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP


#include <algorithm>

#include "LzTools.hpp"

namespace lz { namespace detail {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator, class Function>
#else
    template<LZ_CONCEPT_ITERATOR Iterator, class Function>
#endif
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
        Function _predicate;
#ifdef LZ_HAS_EXECUTION
        Execution _execution{};
#endif

    public:
#ifdef LZ_HAS_EXECUTION
        FilterIterator(const Iterator begin, const Iterator end, const Function& function, const Execution execution)
#else
        FilterIterator(const Iterator begin, const Iterator end, const Function& function)
#endif
    :
            _iterator(begin),
            _end(end),
            _predicate(function)
#ifdef LZ_HAS_EXECUTION
            , _execution(execution)
#endif
            {
#ifdef LZ_HAS_EXECUTION
            _iterator = std::find_if(_execution, _iterator, _end, _predicate);
#else
            _iterator = std::find_if(_iterator, _end, _predicate);
#endif
        }

        FilterIterator() = default;

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &*_iterator;
        }

        FilterIterator& operator++() {
            if (_iterator != _end) {
#ifdef LZ_HAS_EXECUTION
                _iterator = std::find_if(_execution, ++_iterator, _end, _predicate);
#else
                _iterator = std::find_if(++_iterator, _end, _predicate);
#endif
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

#endif