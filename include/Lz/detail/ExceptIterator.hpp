#pragma once

#include <type_traits>
#include <algorithm>
#include <functional>


namespace lz { namespace detail {
    template<class Iterator, class IteratorToExcept>
    struct ExceptIteratorHelper {
        mutable Iterator iterator{};
        Iterator end{};
        IteratorToExcept toExceptBegin{};
        IteratorToExcept toExceptEnd{};
    };

    template<class Iterator, class IteratorToExcept>
    class ExceptIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using reference = typename std::iterator_traits<Iterator>::reference;

    private:
        const ExceptIteratorHelper<Iterator, IteratorToExcept>* _iteratorHelper;

    public:
        // gcc 5.4.0 crashes with inline declaration
        ExceptIterator():
            _iteratorHelper(ExceptIteratorHelper<Iterator, IteratorToExcept>()) {
        }

        explicit ExceptIterator(const ExceptIteratorHelper<Iterator, IteratorToExcept>* iteratorHelper) :
            _iteratorHelper(iteratorHelper) {
        }

        void find() {
            IteratorToExcept it = std::find(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd,
                                            *_iteratorHelper->iterator);

            while (it != _iteratorHelper->toExceptEnd) {
                ++_iteratorHelper->iterator;
                if (_iteratorHelper->iterator == _iteratorHelper->end) {
                    return;
                }
                it = std::find(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd,
                               *_iteratorHelper->iterator);
            }
        }

        reference operator*() const {
            return *_iteratorHelper->iterator;
        }

        pointer operator->() const {
            return _iteratorHelper->iterator.operator->();
        }

        ExceptIterator& operator++() {
            ++_iteratorHelper->iterator;
            if (_iteratorHelper->iterator != _iteratorHelper->end) {
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
            return _iteratorHelper->iterator != other._iteratorHelper->end;
        }

        bool operator==(const ExceptIterator& other) const {
            return !(*this != other);
        }
    };
}}