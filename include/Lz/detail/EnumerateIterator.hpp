#pragma once

#include <iterator>
#include <Lz/detail/LzTools.hpp>


namespace lz { namespace detail {
    template<class Iterator, class IntType>
    class EnumerateIterator {
    private:
        IntType _index;
        Iterator _iterator;

        using IterTraits = std::iterator_traits<Iterator>;
    public:
        using iterator_category = typename IterTraits::iterator_category;
        using value_type = std::pair<IntType, typename IterTraits::value_type>;
        using difference_type = typename IterTraits::difference_type;
        using reference = std::pair<IntType, typename IterTraits::reference>;
        using pointer = FakePointerProxy<reference>;

        EnumerateIterator(const IntType start, const Iterator iterator) :
            _index(start),
            _iterator(iterator) {
        }

        reference operator*() const {
            return reference(_index, *_iterator);
        }

        FakePointerProxy<reference> operator->() {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        EnumerateIterator& operator++() {
            ++_index;
            ++_iterator;
            return *this;
        }

        EnumerateIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        EnumerateIterator& operator--() {
            --_index;
            --_iterator;
            return *this;
        }

        EnumerateIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        EnumerateIterator& operator+=(const difference_type offset) {
            _index += offset;
            _iterator += offset;
            return *this;
        }

        EnumerateIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        EnumerateIterator& operator-=(const difference_type offset) {
            _index -= offset;
            _iterator -= offset;
            return *this;
        }

        EnumerateIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const EnumerateIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator==(const EnumerateIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const EnumerateIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator<(const EnumerateIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const EnumerateIterator& other) const {
            return other < *this;
        }

        bool operator<=(const EnumerateIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const EnumerateIterator& other) const {
            return !(*this < other);
        }
    };
}}
