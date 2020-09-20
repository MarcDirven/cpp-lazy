#pragma once

#include <type_traits>
#include <algorithm>
#include <functional>


namespace lz {
    template<class, class>
    class Except;

    namespace detail {
        template<class Iterator, class IteratorToExcept>
        struct ExceptIteratorHelper {
            IteratorToExcept toExceptBegin{};
            IteratorToExcept toExceptEnd{};
            Iterator end{};
            bool isSorted{};
        };

        template<class Iterator, class IteratorToExcept>
        class ExceptIterator {
            using IterTraits = std::iterator_traits<Iterator>;
            using ValueTypeToExcept = typename std::iterator_traits<IteratorToExcept>::value_type;
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = typename IterTraits::value_type;
            using difference_type = typename IterTraits::difference_type;
            using pointer = typename IterTraits::pointer;
            using reference = typename IterTraits::reference;

        private:
            Iterator _iterator{};
            const ExceptIteratorHelper<Iterator, IteratorToExcept>* _iteratorHelper{};

            friend class Except<Iterator, IteratorToExcept>;

            void find() {
                _iterator = std::find_if(_iterator, _iteratorHelper->end, [this](const value_type& value) {
                    return !std::binary_search(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd, value);
                });
            }

        public:
            ExceptIterator() = default;

            explicit ExceptIterator(const Iterator begin, const Iterator end,
                                    const ExceptIteratorHelper<Iterator, IteratorToExcept>* iteratorHelper) :
                _iterator(begin),
                _iteratorHelper(iteratorHelper) {
                if (begin != end) {
                    if (!_iteratorHelper->isSorted) {
                        std::sort(begin, end);
                    }
                    find();
                }
            }

            reference operator*() const {
                return *_iterator;
            }

            pointer operator->() const {
                return &*_iterator;
            }

            ExceptIterator& operator++() {
                ++_iterator;
                if (_iterator != _iteratorHelper->end) {
                    find();
                }
                return *this;
            }

            ExceptIterator operator++(int) {
                ExceptIterator tmp(*this);
                ++*this;
                return tmp;
            }

            bool operator!=(const ExceptIterator& other) const {
                return _iterator != other._iteratorHelper->end;
            }

            bool operator==(const ExceptIterator& other) const {
                return !(*this != other);
            }
        };
    }
}