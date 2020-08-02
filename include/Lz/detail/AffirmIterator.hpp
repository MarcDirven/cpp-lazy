#pragma once

#include <iterator>
#include <functional>


namespace lz { namespace detail {
    template<class Iterator, class Exception, class ValueType>
    struct AffirmIteratorHelper {
        std::function<bool(ValueType)> _predicate{};
        Exception _exception{};
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
        const AffirmIteratorHelper<Iterator, Exception, value_type>* _helper{};

    public:
        AffirmIterator(const Iterator iterator, const AffirmIteratorHelper<Iterator, Exception, value_type>* helper) :
            _iterator(iterator),
            _helper(helper) {
        }

        reference operator*() const {
            if (!_helper->_predicate(*_iterator)) {
                throw _helper->_exception;
            }
            return *_iterator;
        }

        pointer operator->() const {
            if (!_helper->_predicate(*_iterator)) {
                throw _helper->_exception;
            }
            return _iterator.operator->();
        }

        AffirmIterator& operator++() {
            ++_iterator;
            return *this;
        }

        AffirmIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        AffirmIterator& operator--() {
            --_iterator;
            return *this;
        }

        AffirmIterator operator--(int) {
            auto tmp = *this;
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
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        AffirmIterator operator-(const difference_type offset) const {
            auto tmp(*this);
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