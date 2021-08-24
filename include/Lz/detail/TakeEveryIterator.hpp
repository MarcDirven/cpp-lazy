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

    template<class I = Iterator>
    EnableIf<IsRandomAccess<I>::value> advance() noexcept {
        const auto distance = _end - _iterator;
        if (_offset >= distance) {
            _iterator = _end;
        }
        else {
            _iterator += _offset;
        }
    }

    template<class I = Iterator>
        EnableIf<!IsRandomAccess<I>::value> advance() noexcept {
        difference_type count = 0;
        while (_iterator != _end || count != _offset) {
            ++_iterator;
            ++count;
        }
    }

public:
    LZ_CONSTEXPR_CXX_20
    TakeEveryIterator(Iterator iterator, Iterator end, const difference_type offset) noexcept :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _offset(offset) {
    }

    constexpr TakeEveryIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator++() noexcept {
        advance();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator++(int) {
        TakeEveryIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const TakeEveryIterator& a, const TakeEveryIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const TakeEveryIterator& a, const TakeEveryIterator& b) noexcept {
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        return a._iterator != b._iterator;
    }
};
} // namespace internal
} // namespace lz

#endif