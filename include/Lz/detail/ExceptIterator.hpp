#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP


#include <algorithm>


namespace lz {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR, LZ_CONCEPT_ITERATOR>
#else
    template<LZ_CONCEPT_ITERATOR, LZ_CONCEPT_ITERATOR>
#endif
    class Except;

    namespace detail {
#ifdef LZ_HAS_EXECUTION
        template<class Execution, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
#else
        template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
#endif
        struct ExceptIteratorHelper {
            IteratorToExcept toExceptBegin{};
            IteratorToExcept toExceptEnd{};
            Iterator end{};
#ifdef LZ_HAS_EXECUTION
            Execution execution{};
#endif
            bool isSorted{};
        };

#ifdef LZ_HAS_EXECUTION
        template<class Execution, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
#else
        template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
#endif
        class ExceptIterator {
            using IterTraits = std::iterator_traits<Iterator>;

        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = typename IterTraits::value_type;
            using difference_type = typename IterTraits::difference_type;
            using pointer = typename IterTraits::pointer;
            using reference = typename IterTraits::reference;

        private:
            Iterator _iterator{};
#ifdef LZ_HAS_EXECUTION
            const ExceptIteratorHelper<Execution, Iterator, IteratorToExcept>* _iteratorHelper{};
            friend class Except<Execution, Iterator, IteratorToExcept>;
#else
            const ExceptIteratorHelper<Iterator, IteratorToExcept>* _iteratorHelper{};
            friend class Except<Iterator, IteratorToExcept>;
#endif

            void find() {
#ifdef LZ_HAS_EXECUTION
                if constexpr (IsSequencedPolicyV<Execution>) {
                    _iterator = std::find_if(_iterator, _iteratorHelper->end, [this](const value_type& value)
                    {
                        return !std::binary_search(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd, value);
                    });
                }
                else {
                    _iterator = std::find_if(_iteratorHelper->execution, _iterator, _iteratorHelper->end, [this](const value_type& value) {
                        return !std::binary_search(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd, value);
                    });
                }
#else
                    _iterator = std::find_if(_iterator, _iteratorHelper->end, [this](const value_type& value) {
                        return !std::binary_search(_iteratorHelper->toExceptBegin, _iteratorHelper->toExceptEnd, value);
                    });
#endif
            }

        public:
            ExceptIterator() = default;

#ifdef LZ_HAS_EXECUTION
            ExceptIterator(const Iterator begin, const Iterator end,
                                    const ExceptIteratorHelper<Execution, Iterator, IteratorToExcept>* iteratorHelper)
#else
            ExceptIterator(const Iterator begin, const Iterator end,
                                    const ExceptIteratorHelper<Iterator, IteratorToExcept>* iteratorHelper)
#endif
            :
                _iterator(begin),
                _iteratorHelper(iteratorHelper) {
                if (begin != end) {
                    if (!_iteratorHelper->isSorted) {
#ifdef LZ_HAS_EXECUTION
                        if constexpr (IsSequencedPolicyV<Execution>) {
                            std::sort(begin, end);
                        }
                        else {
                            std::sort(_iteratorHelper->execution, begin, end);
                        }
#else
                        std::sort(begin, end);
#endif
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

#endif