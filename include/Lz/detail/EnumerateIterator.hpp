#pragma once

#include <iterator>


namespace lz { namespace detail {
    template<class Iterator, class IntType>
    class EnumerateIterator {
    public:
        IntType _index;
        Iterator _iterator;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<IntType, typename std::iterator_traits<Iterator>::value_type>;
        using difference_type = std::ptrdiff_t;
        using reference = std::pair<IntType, typename std::iterator_traits<Iterator>::reference>;

        EnumerateIterator(IntType start, Iterator iterator) :
            _index(start),
            _iterator(iterator) {
        }

        reference operator*() const {
            return reference(_index, *_iterator);
        }

        FakePointerProxy<reference> operator->() {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        bool operator==(const EnumerateIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const EnumerateIterator& other) const {
            return _iterator != other._iterator;
        }

        EnumerateIterator& operator++() {
            ++_index;
            ++_iterator;
            return *this;
        }

        EnumerateIterator& operator--() {
            --_index;
            --_iterator;
            return *this;
        }

        EnumerateIterator& operator+=(const difference_type offset) {
            _index += offset;
            _iterator += offset;
            return *this;
        }

        EnumerateIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        EnumerateIterator& operator-=(const difference_type offset) {
            _index -= offset;
            _iterator -= offset;
            return *this;
        }

        EnumerateIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        using pointer = FakePointerProxy<decltype(std::declval<EnumerateIterator>().operator->())>;
    };
}}