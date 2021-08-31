#pragma once

#ifndef LZ_ROTATE_HPP
#    define LZ_ROTATE_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/RotateIterator.hpp"

namespace lz {
template<class Iterator, bool>
class Rotate;

namespace internal {
template<LZ_CONCEPT_ITERATOR Iterator>
internal::EnableIf<internal::IsRandomAccess<Iterator>::value, Rotate<Iterator, true>>
rotateRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> count) {
    return { begin, end, begin + count, count, end - begin };
}

template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
internal::EnableIf<internal::IsRandomAccess<Iterator>::value, Rotate<Iterator, true>>
rotate(Iterable&& iterable, const internal::DiffType<internal::IterTypeFromIterable<Iterable>> count) {
    return rotateRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), count);
}

template<LZ_CONCEPT_ITERATOR Iterator>
internal::EnableIf<!internal::IsRandomAccess<Iterator>::value, Rotate<Iterator, false>>
rotateRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> count) {
    using lz::next;
    using std::next;
    return { begin, end, next(begin, count), internal::getIterLength(begin, end) };
}

template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
internal::EnableIf<!internal::IsRandomAccess<Iterator>::value, Rotate<Iterator, false>>
rotate(Iterable&& iterable, const internal::DiffType<Iterator> count) {
    using lz::next;
    using std::next;
    const auto begin = std::begin(iterable);
    return { begin, internal::end(std::forward<Iterable>(iterable)), next(begin, count),
             static_cast<internal::DiffType<Iterator>>(iterable.size()) };
}
} // namespace internal

template<class Iterator>
class Rotate<Iterator, true> : public internal::BasicIteratorView<internal::RotateIterator<Iterator, true>> {
public:
    using iterator = internal::RotateIterator<Iterator, true>;
    using const_iterator = iterator;

    Rotate(Iterator begin, Iterator end, Iterator iter, const typename iterator::difference_type start,
           const typename iterator::difference_type distance) :
        internal::BasicIteratorView<iterator>(iterator(begin, end, iter, start, 0, distance),
                                              iterator(begin, end, iter, distance - start + 1, distance, distance)) {
    }
};

template<class Iterator>
class Rotate<Iterator, false> : public internal::BasicIteratorView<internal::RotateIterator<Iterator, false>> {
public:
    using iterator = internal::RotateIterator<Iterator, false>;
    using const_iterator = iterator;

    Rotate(Iterator begin, Iterator end, Iterator iter, const typename iterator::difference_type distance) :
        internal::BasicIteratorView<iterator>(iterator(begin, end, iter, 0), iterator(begin, end, iter, distance)) {
    }
};

template<LZ_CONCEPT_ITERATOR Iterator>
Rotate<Iterator, internal::IsRandomAccess<Iterator>::value>
rotateRange(Iterator begin, Iterator end, internal::DiffType<Iterator> start) {
    return internal::rotateRange(std::move(begin), std::move(end), start);
}

template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
Rotate<Iterator, internal::IsRandomAccess<Iterator>::value>
rotate(Iterable&& iterable, const internal::DiffType<Iterator> start) {
    return internal::rotate(iterable, start);
}

} // namespace lz

#endif // LZ_ROTATE_HPP
