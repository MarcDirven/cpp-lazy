#pragma once

#ifndef LZ_ROTATE_HPP
#    define LZ_ROTATE_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/RotateIterator.hpp"

namespace lz {
template<class Iterator, bool>
class Rotate;

#    ifndef __cpp_if_constexpr
namespace internal {
template<LZ_CONCEPT_ITERATOR Iterator>
EnableIf<IsRandomAccess<Iterator>::value, Rotate<Iterator, true>>
rotateRange(Iterator begin, Iterator end, const DiffType<Iterator> count) {
    return { begin, end, begin + count, count, end - begin };
}

template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = IterTypeFromIterable<Iterable>>
EnableIf<IsRandomAccess<Iterator>::value, Rotate<Iterator, true>>
rotate(Iterable&& iterable, const DiffType<IterTypeFromIterable<Iterable>> count) {
    return rotateRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), count);
}

template<LZ_CONCEPT_ITERATOR Iterator>
EnableIf<!IsRandomAccess<Iterator>::value, Rotate<Iterator, false>>
rotateRange(Iterator begin, Iterator end, const DiffType<Iterator> count) {
    using lz::next;
    using std::next;
    return { begin, end, next(begin, count), getIterLength(begin, end) };
}

template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = IterTypeFromIterable<Iterable>>
EnableIf<!IsRandomAccess<Iterator>::value, Rotate<Iterator, false>> rotate(Iterable&& iterable, const DiffType<Iterator> count) {
    using lz::next;
    using std::next;
    const auto begin = std::begin(iterable);
    return { begin, internal::end(std::forward<Iterable>(iterable)), next(begin, count),
             static_cast<DiffType<Iterator>>(iterable.size()) };
}
} // namespace internal
#    endif // __cpp_if_constexpr
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
    using value_type = typename iterator::value_type;

    Rotate(Iterator begin, Iterator end, Iterator iter, const typename iterator::difference_type distance) :
        internal::BasicIteratorView<iterator>(iterator(begin, end, iter, 0), iterator(begin, end, iter, distance)) {
    }
};

#    ifndef __cpp_if_constexpr
/**
 * Rotates a sequence by `start` amount. Example: if the sequence = `{1, 2, 3}` and `start` = 1, then its result = `{2, 3, 1}`.
 * @param begin Beginning of the sequence
 * @param end Ending of the sequence
 * @param start Offset that rotates the sequence.
 * @return A rotate iterator view object.
 */
template<class Iterator>
Rotate<Iterator, internal::IsRandomAccess<Iterator>::value>
rotateRange(Iterator begin, Iterator end, internal::DiffType<Iterator> start) {
    return internal::rotateRange(std::move(begin), std::move(end), start);
}

/**
 * Rotates a sequence by `start` amount. Example: if the sequence = `{1, 2, 3}` and `start` = 1, then its result = `{2, 3, 1}`.
 * @param iterable The iterable sequence
 * @param start Offset that rotates the sequence.
 * @return A rotate iterator view object.
 */
template<class Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
Rotate<Iterator, internal::IsRandomAccess<Iterator>::value>
rotate(Iterable&& iterable, const internal::DiffType<Iterator> start) {
    return internal::rotate(iterable, start);
}

#    else
/**
 * Rotates a sequence by `start` amount. Example: if the sequence = `{1, 2, 3}` and `start` = 1, then its result = `{2, 3, 1}`.
 * @param begin Beginning of the sequence
 * @param end Ending of the sequence
 * @param start Offset that rotates the sequence.
 * @return A rotate iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
Rotate<Iterator, internal::IsRandomAccess<Iterator>::value>
rotateRange(Iterator begin, Iterator end, internal::DiffType<Iterator> start) {
    if constexpr (internal::IsRandomAccess<Iterator>::value) {
        return { begin, end, begin + start, start, end - begin };
    }
    else {
        using lz::next;
        using std::next;
        return { begin, end, next(begin, start), getIterLength(begin, end) };
    }
}
/**
 * Rotates a sequence by `start` amount. Example: if the sequence = `{1, 2, 3}` and `start` = 1, then its result = `{2, 3, 1}`.
 * @param iterable The iterable sequence
 * @param start Offset that rotates the sequence.
 * @return A rotate iterator view object.
 */
template<class Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
Rotate<Iterator, internal::IsRandomAccess<Iterator>::value>
rotate(Iterable&& iterable, const internal::DiffType<Iterator> start) {
    if constexpr (internal::IsRandomAccess<Iterator>::value) {
        return rotateRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                           start);
    }
    else {
        using lz::next;
        using std::next;
        const auto begin = std::begin(iterable);
        return { begin, internal::end(std::forward<Iterable>(iterable)), next(begin, start),
                 static_cast<internal::DiffType<Iterator>>(iterable.size()) };
    }
}

#    endif

} // namespace lz

#endif // LZ_ROTATE_HPP
