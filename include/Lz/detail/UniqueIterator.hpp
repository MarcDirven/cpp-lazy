#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include <iterator>
#include <algorithm>


namespace lz { namespace detail {
    template<LZ_CONCEPT_ITERATOR Iterator>
    class UniqueIterator {
        using IterTraits = std::iterator_traits<Iterator>;

        Iterator _iterator{};
        Iterator _end{};

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename IterTraits::value_type;
        using difference_type = typename IterTraits::difference_type;
        using pointer = typename IterTraits::pointer;
        using reference = typename IterTraits::reference;

        UniqueIterator(const Iterator begin, const Iterator end):
            _iterator(begin),
            _end(end) {
            if (begin == end) {
                return;
            }

            if (std::is_sorted(begin, end)) {
                return;
            }

            std::sort(begin, end);
        }

        UniqueIterator() = default;

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &*_iterator;
        }

        UniqueIterator& operator++() {
            _iterator = std::adjacent_find(_iterator, _end, std::less<value_type>());
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