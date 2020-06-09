#pragma once


#include <iterator>


namespace detail {
    template<class Iterator, class Function>
    class TakeWhileIterator {
    private:
        Iterator _iterator{};
        Iterator _end{};
        Function _function{};

    public:
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using pointer = typename std::iterator_traits<Iterator>::pointer;

    public:
        TakeWhileIterator(Iterator iterator, Iterator end, Function function) :
            _iterator(iterator),
            _end(end),
            _function(function) {
            _iterator = !_function(*iterator) ? end : _iterator;
        }

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &_iterator;
        }

        bool operator!=(const TakeWhileIterator& other) const {
            if (_iterator == other._end) {
                return false;
            }
            return _function(*_iterator);
        }

        bool operator==(const TakeWhileIterator& other) const {
            // Prevent recursion when: TakeWhileIterator<TakeWhileIterator<Iterator, fn>, fn>
            return &(*_iterator) == &(*other._iterator);
        }

        TakeWhileIterator& operator++() {
            ++_iterator;
            return *this;
        }

        TakeWhileIterator& operator--() {
            --_iterator;
            return *this;
        }

        TakeWhileIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        TakeWhileIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        TakeWhileIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        TakeWhileIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }
    };
}

namespace lz {
    template<class Iterator, class Function>
    class TakeWhileObject {
    public:
        using iterator = detail::TakeWhileIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type;
        using const_reference = value_type;
        using pointer = value_type;
        using const_pointer = value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        TakeWhileObject(Iterator begin, Iterator end, Function function):
            _begin(begin, end, function),
            _end(begin, end, function)
        {
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }

        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return to<std::vector>();
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container{};
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    template<class Iterator, class Function>
    auto takewhilerange(Iterator begin, Iterator end, Function predicate) {
        return TakeWhileObject<Iterator, Function>(begin, end, predicate);
    }

    template<class Container, class Function>
    auto takewhile(Container&& container, Function predicate) {
        return takewhilerange(container.begin(), container.end(), predicate);
    }

    template<class Iterator>
    auto takerange(Iterator begin, Iterator end) {
        return takewhilerange(begin, end, [](const auto&) { return true; });
    }

    template<class Container>
    auto take(Container&& container, const size_t amount, const size_t from = 0) {
        auto begin = container.begin();
        return takerange(begin + from, begin + amount);
    }
}