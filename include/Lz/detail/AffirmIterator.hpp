#pragma once

#ifndef LZ_AFFIRM_ITERATOR_HPP
#define LZ_AFFIRM_ITERATOR_HPP

#include <iterator>
#include "LzTools.hpp"


namespace lz { namespace detail {
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
        Exception _exception{};
        Function _function{};

    public:
        AffirmIterator(const Iterator iterator, const Function& function, Exception exception) :
            _iterator(iterator),
            _exception(std::move(exception)),
    		_function(function)
    	{
        }

        AffirmIterator() = default;

        reference operator*() const {
            if (!_function(*_iterator)) {
                throw _exception;
            }
            return *_iterator;
        }

        pointer operator->() const {
            if (!_function(*_iterator)) {
                throw _exception;
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