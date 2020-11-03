#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP


#include <algorithm>


namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
#else // ^^^ has execution vvv ! has execution
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
#endif // end has execution
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
        Iterator _end{};
        IteratorToExcept _toExceptBegin{};
        IteratorToExcept _toExceptEnd{};

#ifdef LZ_HAS_EXECUTION
        Execution _execution{};
#endif // end has execution

        void find() {
#ifdef LZ_HAS_EXECUTION
            if constexpr (IsSequencedPolicyV<Execution>) {
                _iterator = std::find_if(_iterator, _end, [this](const value_type& value) {
                    return !std::binary_search(_toExceptBegin, _toExceptEnd, value);
                });
            }
            else {
                _iterator = std::find_if(_execution, _iterator, _end, [this](const value_type& value) {
                    return !std::binary_search(_toExceptBegin, _toExceptEnd, value);
               });
            }
#else // ^^^ has execution vvv ! has execution
            _iterator = std::find_if(_iterator, _end, [this](const value_type& value) {
                return !std::binary_search(_toExceptBegin, _toExceptEnd, value);
            });
#endif // end has execution
        }

    public:
        ExceptIterator() = default;

#ifdef LZ_HAS_EXECUTION
        ExceptIterator(Iterator begin, Iterator end, IteratorToExcept toExceptBegin,IteratorToExcept toExceptEnd,
                       const Execution execution) :
#else // ^^^ has execution vvv ! has execution
        ExceptIterator(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd) :
#endif // end has execution
            _iterator(std::move(begin)),
            _end(std::move(end)),
            _toExceptBegin(std::move(toExceptBegin)),
            _toExceptEnd(std::move(toExceptEnd))
#ifdef LZ_HAS_EXECUTION
            , _execution(execution)
#endif // end has execution
        {
            if (_toExceptBegin != _toExceptEnd) {
#ifdef LZ_HAS_EXECUTION
                if (!std::is_sorted(_execution, _toExceptBegin, _toExceptEnd)) {
                    std::sort(_execution, _toExceptBegin, _toExceptEnd);
                }
#else // ^^^ has execution vvv ! has execution
                if (!std::is_sorted(_toExceptBegin, _toExceptEnd)) {
                    std::sort(_toExceptBegin, _toExceptEnd);
                }
#endif // end has execution
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
            if (_iterator != _end) {
                find();
            }
            return *this;
        }

        ExceptIterator operator++(int) {
            ExceptIterator tmp(*this);
            ++* this;
            return tmp;
        }

        friend bool operator!=(const ExceptIterator& a, const ExceptIterator& b) {
            return a._iterator != b._iterator;
        }

        friend bool operator==(const ExceptIterator& a, const ExceptIterator& b) {
            return !(a != b);
        }
    };
}} // end lz::internal

#endif