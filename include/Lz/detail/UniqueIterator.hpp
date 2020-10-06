#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include <iterator>
#include <algorithm>


#include "LzTools.hpp"


namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator>
#else
    template<LZ_CONCEPT_ITERATOR Iterator>
#endif
    class UniqueIterator {
        using IterTraits = std::iterator_traits<Iterator>;

        Iterator _iterator{};
        Iterator _end{};
#ifdef LZ_HAS_EXECUTION
        Execution _execution;
#endif

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename IterTraits::value_type;
        using difference_type = typename IterTraits::difference_type;
        using pointer = typename IterTraits::pointer;
        using reference = typename IterTraits::reference;

#ifdef LZ_HAS_EXECUTION
        UniqueIterator(const Iterator begin, const Iterator end, const Execution execution)
#else
        UniqueIterator(const Iterator begin, const Iterator end)
#endif
        :
            _iterator(begin),
            _end(end)
#ifdef LZ_HAS_EXECUTION
            , _execution(execution)
#endif
        {
            if (begin == end) {
                return;
            }

#ifdef LZ_HAS_EXECUTION
            if constexpr (IsSequencedPolicyV<Execution>) {
                if (std::is_sorted(begin, end)) {
                    return;
                }
            }
            else {
                if (std::is_sorted(_execution, begin, end)) {
                    return;
                }
            }
#else
            if (std::is_sorted(begin, end)) {
                return;
            }
#endif

#ifdef LZ_HAS_EXECUTION
            if constexpr (IsSequencedPolicyV<Execution>) {
                std::sort(begin, end);
            }
            else {
                std::sort(_execution, begin, end);
            }
#else
            std::sort(begin, end);
#endif
        }

        UniqueIterator() = default;

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &*_iterator;
        }

        UniqueIterator& operator++() {
#ifdef LZ_HAS_EXECUTION
            if constexpr (IsSequencedPolicyV<Execution>) {
                _iterator = std::adjacent_find(_iterator, _end, std::less<value_type>());
            }
            else {
                _iterator = std::adjacent_find(_execution, _iterator, _end, std::less<value_type>());
            }
#else
            _iterator = std::adjacent_find(_iterator, _end, std::less<value_type>());
#endif

            if (_iterator != _end) {
                ++_iterator;
            }
            return *this;
        }

        UniqueIterator operator++(int) {
            UniqueIterator tmp(*this);
            ++*this;
            return tmp;
        }

        bool operator!=(const UniqueIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const UniqueIterator& other) const {
            return !(*this != other);
        }
    };
}}

#endif