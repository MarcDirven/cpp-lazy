#pragma once

#ifndef LZ_AFFIRM_ITERATOR_HPP
#define LZ_AFFIRM_ITERATOR_HPP

#include <iterator>
#include <functional>


namespace lz { namespace detail {
    template<class Exception, class ValueType>
    struct AffirmIteratorHelper {
        std::function<bool(ValueType)> predicate{};
        Exception exception{};
    };

    template<class Exception, class Iterator, class Function>
    class AffirmIterator {
        using IteratorTraits = std::iterator_traits<Iterator>;

    public:
        using iterator_category = typename IteratorTraits::iterator_category;
        using value_type = typename IteratorTraits::value_type;
        using difference_type = typename IteratorTraits::difference_type;
        using reference = typename IteratorTraits::reference;
        using pointer = typename IteratorTraits::pointer;

    private:
        Iterator _iterator{};
        const AffirmIteratorHelper<Exception, value_type>* _helper{};

    public:
        AffirmIterator(const Iterator iterator, const AffirmIteratorHelper<Exception, value_type>* helper) :
            _iterator(iterator),
            _helper(helper) {
        }

        AffirmIterator() = default;

        reference operator*() const {
            if (!_helper->predicate(*_iterator)) {
                throw _helper->exception;
            }
            return *_iterator;
        }

        pointer operator->() const {
            if (!_helper->predicate(*_iterator)) {
                throw _helper->exception;
            }
            return &*_iterator;
        }

        AffirmIterator& operator++() {
            ++_iterator;
            return *this;
        }

        AffirmIterator operator++(int) {
            AffirmIterator tmp(*this);
            ++*this;
            return tmp;
        }

        AffirmIterator& operator--() {
            --_iterator;
            return *this;
        }

        AffirmIterator operator--(int) {
            AffirmIterator tmp = *this;
            --*this;
            return tmp;
        }

        AffirmIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        AffirmIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        AffirmIterator operator+(const difference_type offset) const {
            AffirmIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        AffirmIterator operator-(const difference_type offset) const {
            AffirmIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const AffirmIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator!=(const AffirmIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const AffirmIterator& other) const {
            return !(*this != other);
        }

        bool operator<(const AffirmIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const AffirmIterator& other) const {
            return other < *this;
        }

        bool operator<=(const AffirmIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const AffirmIterator& other) const {
            return !(*this < other);
        }
    };
}}

#endif