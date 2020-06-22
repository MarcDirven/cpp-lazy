#pragma once

#include <type_traits>
#include <algorithm>
#include <functional>


namespace lz { namespace it { namespace detail {
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
        IteratorToExcept _toExceptBegin{};
        IteratorToExcept _toExceptEnd{};

    public:
        ExceptIterator(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd):
            _iterator(begin),
            _end(end),
            _toExceptBegin(toExceptBegin),
            _toExceptEnd(toExceptEnd) {
        }

        void find() {
            IteratorToExcept it = std::find(_toExceptBegin, _toExceptEnd, *_iterator);

            while (it != _toExceptEnd) {
                ++_iterator;
                if (_iterator == _end) {
                    return;
                }
                it = std::find(_toExceptBegin, _toExceptEnd, *_iterator);
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

        bool operator!=(const ExceptIterator& other) const {
            return _iterator != other._end;
        }

        bool operator==(const ExceptIterator& other) const {
            return !(*this != other);
        }
    };
}}}