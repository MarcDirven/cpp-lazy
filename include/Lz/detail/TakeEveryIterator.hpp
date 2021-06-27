#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include "LzTools.hpp"

#include <cmath>

namespace lz {
namespace internal {
template<class Iterator>
class TakeEveryIterator {
    Iterator _iterator{};
    Iterator _end{};
    std::size_t _offset{};
    std::size_t _current{};
    std::size_t _distance{};

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator(Iterator iterator, Iterator end, const std::size_t offset, const std::size_t distance) :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _offset(offset),
        _current(iterator == end ? distance : 0),
        _distance(distance) {
    }

    constexpr TakeEveryIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator++() {
        using lz::next;
        using std::next;
        if (_current + _offset >= _distance) {
            _iterator = _end;
        }
        else {
            _iterator = next(std::move(_iterator), static_cast<difference_type>(_offset));
            _current += _offset;
        }
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
        using lz::distance;
        using std::distance;
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        const auto dist = distance(b._iterator, a._iterator) / static_cast<float>(a._offset);
        return static_cast<difference_type>(std::ceil(dist));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator+(const difference_type offset) const {
        using lz::distance;
        using lz::next;
        using std::distance;
        using std::next;
        const auto dist = distance(_iterator, _end);
        const auto diffOffset = static_cast<difference_type>(_offset) * offset;
        if (diffOffset >= dist) {
            return { _end, _end, _offset, 0 };
        }
        return { next(_iterator, diffOffset), _end, _offset, static_cast<std::size_t>(dist) };
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