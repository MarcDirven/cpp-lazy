#pragma once

#include <type_traits>
#include <algorithm>
#include <functional>


namespace lz { namespace detail {
    template<class Iterator, class IteratorToExcept>
    struct ExceptIteratorHelper {
        IteratorToExcept toExceptBegin{};
        IteratorToExcept toExceptEnd{};
    };

    template<class Iterator, class IteratorToExcept>
    class ExceptIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using reference = typename std::iterator_traits<Iterator>::reference;

    private:
        Iterator _iterator{};
        Iterator _end{};
        const ExceptIteratorHelper<Iterator, IteratorToExcept>* _iteratorHelper;

    public:
        // gcc 5.4.0 crashes with inline declaration
        ExceptIterator():
            _iteratorHelper(ExceptIteratorHelper<Iterator, IteratorToExcept>()) {
        }

        explicit ExceptIterator(const Iterator begin, const Iterator end,
                                const ExceptIteratorHelper<Iterator, IteratorToExcept>* iteratorHelper) :
            _iterator(begin),
            _end(end),
            _iteratorHelper(iteratorHelper) {
        }

        void find() {
            IteratorToExcept it = std::find(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd,
                                            *_iterator);

            while (it != _iteratorHelper->toExceptEnd) {
                ++_iterator;
                if (_iterator == _end) {
                    return;
                }
                it = std::find(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd,
                               *_iterator);
            }
        }

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return _iterator.operator->();
        }

        ExceptIterator& operator++() {
            ++_iterator;
            if (_iterator != _end) {
                find();
            }
            return *this;
        }

        ExceptIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator!=(const ExceptIterator& other) const {
            return _iterator != other._end;
        }

        bool operator==(const ExceptIterator& other) const {
            return !(*this != other);
        }
    };
}}