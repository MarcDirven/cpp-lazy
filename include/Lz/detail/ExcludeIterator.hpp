#pragma once

#ifndef LZ_EXCLUDE_ITERATOR_HPP
#define LZ_EXCLUDE_ITERATOR_HPP

namespace lz {
namespace internal {
template<class Iterator>
class ExcludeIterator {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

private:
    Iterator _iterator{};
    Iterator _begin{};
    Iterator _end{};
    difference_type _index{};
    difference_type _from{};
    difference_type _to{};

public:
    LZ_CONSTEXPR_CXX_20
    ExcludeIterator(Iterator it, Iterator begin, Iterator end, const difference_type from, const difference_type to) :
        _iterator(std::move(it)),
        _begin(std::move(begin)),
        _end(std::move(end)),
        _from(from),
        _to(to) {
        using lz::next;
        using std::next;
        if (_iterator == _begin && _from == 0) {
            _iterator = next(std::move(_iterator), _to);
            _index = _to;
        }
    }

    constexpr ExcludeIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 ExcludeIterator& operator++() {
        using lz::next;
        using std::next;
        ++_iterator;
        ++_index;
        if (_index == _from) {
            _iterator = next(std::move(_iterator), _to - _from);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ExcludeIterator operator++(int) {
        ExcludeIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ExcludeIterator& a, const ExcludeIterator& b) {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ExcludeIterator& a, const ExcludeIterator& b) {
        LZ_ASSERT(a._to == b._to && a._from == b._from, "incompatible iterator types: from and to must be equal");
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const ExcludeIterator& a, const ExcludeIterator& b) {
        LZ_ASSERT(a._to == b._to && a._from == b._from, "incompatible iterator types: from and to must be equal");
        using lz::distance;
        using std::distance;
        if (b._index >= a._to || a._from == 0) { // after range
            return distance(b._iterator, a._iterator);
        }
        return distance(b._iterator, a._iterator) - (a._to - b._from);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ExcludeIterator operator+(difference_type offset) const {
        using lz::distance;
        using lz::next;
        using std::distance;
        using std::next;
        ExcludeIterator tmp(*this);
        const auto indexToAchieve = offset - tmp._index;
        if (indexToAchieve >= tmp._to) { // after range
            tmp._index += offset;
            offset += tmp._to - tmp._from;
        }
        else if (indexToAchieve >= tmp._from && tmp._from != 0) { // within range
            tmp._index = tmp._to;
            offset += tmp._to - tmp._from;
        }
        else { // before range
            tmp._index += offset;
        }
        tmp._iterator = next(std::move(tmp._iterator), offset);
        return tmp;
    }
};
} // namespace internal

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::ExcludeIterator<Iterator>::difference_type
distance(const internal::ExcludeIterator<Iterator>& begin, const internal::ExcludeIterator<Iterator>& end) {
    return end - begin;
}

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ExcludeIterator<Iterator>
next(const internal::ExcludeIterator<Iterator>& iter, internal::DiffType<internal::ExcludeIterator<Iterator>> value) {
    LZ_ASSERT(value >= 0, "offset must be greater than 0 since this is not a bidirectional/random access iterator");
    return iter + value;
}
} // namespace lz

#endif // LZ_EXCLUDE_ITERATOR_HPP
