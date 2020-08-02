#pragma once

#include <iterator>
#include <functional>


namespace lz { namespace detail {
    template<class Iterator, class Function>
    class TakeIterator {
        using IterTraits = std::iterator_traits<Iterator>;

    public:
        using value_type = typename IterTraits::value_type;
        using iterator_category = typename IterTraits::iterator_category;
        using difference_type = typename IterTraits::difference_type;
        using reference = typename IterTraits::reference;
        using pointer = typename IterTraits::pointer;

    private:
        Iterator _iterator{};
        std::function<value_type(value_type)> _function{};

    public:
        TakeIterator(const Iterator iterator, const Iterator end, const Function& function) :
            _iterator(iterator),
            _function(function) {
            if (iterator != end) {
                _iterator = !_function(*iterator) ? end : _iterator;
            }
        }

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &_iterator;
        }

        TakeIterator& operator++() {
            ++_iterator;
            return *this;
        }

        TakeIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        TakeIterator& operator--() {
            --_iterator;
            return *this;
        }

        TakeIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        TakeIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        TakeIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        TakeIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        TakeIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const TakeIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator!=(const TakeIterator& other) const {
            if (_iterator == other._iterator) {
                return false;
            }
            return _function(*_iterator);
        }

        bool operator==(const TakeIterator& other) const {
            return _iterator == other._iterator;
        }

        bool operator<(const TakeIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const TakeIterator& other) const {
            return other < *this;
        }

        bool operator<=(const TakeIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const TakeIterator& other) const {
            return !(*this < other);
        }
    };
}}