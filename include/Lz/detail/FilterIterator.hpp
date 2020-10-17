#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP


#include <algorithm>

#include "LzTools.hpp"

namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator, class Function>
#else // ^^^lz has execution vvv ! lz has execution
    template<LZ_CONCEPT_ITERATOR Iterator, class Function>
#endif // end has execution
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
#endif // end has execution

    public:
#ifdef LZ_HAS_EXECUTION
        FilterIterator(Iterator begin, Iterator end, Function function, const Execution execution)
#else // ^^^lz has execution vvv ! lz has execution
        FilterIterator(Iterator begin, Iterator end, Function function)  // NOLINT(modernize-pass-by-value)
#endif // end has execution
    :
            _iterator(begin),
            _end(end),
            _predicate(std::move(function))
#ifdef LZ_HAS_EXECUTION
            , _execution(execution)
#endif // end has execution
            {
#ifdef LZ_HAS_EXECUTION
            if constexpr (IsSequencedPolicyV<Execution>) { // prevent verbose errors when iter cat < forward
                _iterator = std::find_if(_iterator, _end, _predicate);
            }
            else {
                _iterator = std::find_if(_execution, _iterator, _end, _predicate);
            }
#else // ^^^lz has execution vvv ! lz has execution
            _iterator = std::find_if(_iterator, _end, _predicate);
#endif // end has execution
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
                if constexpr (IsSequencedPolicyV<Execution>) {
                    _iterator = std::find_if(++_iterator, _end, _predicate);
                }
                else {
                    _iterator = std::find_if(_execution, ++_iterator, _end, _predicate);
                }
#else // ^^^lz has execution vvv ! lz has execution
                _iterator = std::find_if(++_iterator, _end, _predicate);
#endif // end has execution
            }
            return *this;
        }

        FilterIterator operator++(int) {
            FilterIterator tmp(*this);
            ++*this;
            return tmp;
        }

        bool operator!=(const FilterIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const FilterIterator& other) const {
            return !(*this != other);
        }
    };
}}

#endif