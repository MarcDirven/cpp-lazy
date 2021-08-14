#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#    define LZ_TAKE_EVERY_ITERATOR_HPP

#    include "LzTools.hpp"

namespace lz {
namespace internal {
template<class Iterator>
class TakeEveryIterator {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    Iterator _iterator{};
    Iterator _end{};
    difference_type _offset{};
    difference_type _currentDistance{};

    void next() {
        if (_offset >= _currentDistance) {
            _iterator = _end;
            _currentDistance = 0;
        }
        else {
            _iterator = std::next(std::move(_iterator), _offset);
            _currentDistance -= _offset;
        }
    }

public:
    LZ_CONSTEXPR_CXX_20
    TakeEveryIterator(Iterator iterator, Iterator end, const difference_type offset, const difference_type distance) :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _offset(offset),
        _currentDistance(distance) {
    }

    constexpr TakeEveryIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator++() {
        this->next();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator++(int) {
        TakeEveryIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        const auto dist = getIterLength(b._iterator, a._iterator);
        if (isEven(dist) && isEven(a._offset)) {
            return static_cast<difference_type>(dist / a._offset);
        }
        return static_cast<difference_type>(dist / a._offset) + 1;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator+(const difference_type offset) const {
        using lz::next;
        using std::next;
        const auto dist = getIterLength(_iterator, _end);
        const auto diffOffset = _offset * offset;
        if (diffOffset >= dist) {
            return { _end, _end, _offset, dist };
        }
        return { next(_iterator, diffOffset), _end, _offset, dist };
    }
};
} // namespace internal

/**
 * Gets the distance between begin and end. If the underlying iterator type is random access, distance is O(1).
 * @param begin Beginning of the sequence.
 * @param end Ending of the sequence.
 * @return The distance between begin and end.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::TakeEveryIterator<Iterator>::difference_type
distance(const internal::TakeEveryIterator<Iterator>& begin, const internal::TakeEveryIterator<Iterator>& end) {
    return end - begin;
}

/**
 * Gets the nth value from iter. If the underlying iterator type is random access, next is O(1).
 * @param iter A take every iterator instance.
 * @param value The amount to add.
 * @return A take every iterator with offset iter + value.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::TakeEveryIterator<Iterator>
next(const internal::TakeEveryIterator<Iterator>& t, const internal::DiffType<internal::TakeEveryIterator<Iterator>> value) {
    LZ_ASSERT(value >= 0, "offset must be greater than 0 since this is not a bidirectional/random access iterator");
    return t + value;
}
} // namespace lz

#endif