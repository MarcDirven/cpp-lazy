#pragma once

#ifndef LZ_FLATTEN_ITERATOR_HPP
#define LZ_FLATTEN_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

namespace lz {
namespace detail {
template<class, class U>
struct AliasWrapper {
    using Type = U;
};

template<class T, class U = void>
using AliasWrapperT = typename AliasWrapper<T, U>::Type;

template<class T, class Enable = void>
struct HasValueType : std::false_type {};

template<class T>
struct HasValueType<T, AliasWrapperT<typename T::value_type>> : std::true_type {};

template<class T, class Enable = void>
struct HasDifferenceType : std::false_type {};

template<class T>
struct HasDifferenceType<T, AliasWrapperT<typename T::difference_type>> : std::true_type {};

template<class T, class Enable = void>
struct HasPointer : std::false_type {};

template<class T>
struct HasPointer<T, AliasWrapperT<typename T::pointer>> : std::true_type {};

template<class T, class Enable = void>
struct HasIterCat : std::false_type {};

template<class T>
struct HasIterCat<T, AliasWrapperT<typename T::iterator_category>> : std::true_type {};

template<class T, class Enable = void>
struct HasReference : std::false_type {};

template<class T>
struct HasReference<T, AliasWrapperT<typename T::reference>> : std::true_type {};

template<class T>
struct IsIterator {
    static constexpr bool value = HasValueType<T>::value && HasDifferenceType<T>::value && HasPointer<T>::value &&
                                  HasReference<T>::value && HasIterCat<T>::value;
};

template<class T, class = void>
struct IterTraitsOrUnderlyingType {
    using Type = T;
};

template<class T>
struct IterTraitsOrUnderlyingType<T, AliasWrapperT<typename T::iterator>> {
    using Type = std::iterator_traits<typename T::iterator>;
};

template<class T, class U = void>
using IterTraitsOrUnderlyingTypeT = typename IterTraitsOrUnderlyingType<T, U>::Type;

template<bool B>
struct CountDimsHelper;

template<>
struct CountDimsHelper<true> {
    template<class T>
    using Inner = IterTraitsOrUnderlyingTypeT<typename T::value_type>;

    template<class T>
    using type = std::integral_constant<int, 1 + CountDimsHelper<IsIterator<Inner<T>>::value>::template type<Inner<T>>::value>;
};

template<>
struct CountDimsHelper<false> {
    template<class>
    using type = std::integral_constant<int, 0>;
};

template<class T>
using CountDims = typename CountDimsHelper<IsIterator<T>::value>::template type<T>;

// Improvement of https://stackoverflow.com/a/21076724/8729023
template<class Iterator, class S>
class FlattenWrapper
    : public IterBase<
          FlattenWrapper<Iterator, S>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>, DiffType<Iterator>,
          CommonType<std::bidirectional_iterator_tag, IterCat<Iterator>>,
          SentinelSelector<CommonType<std::bidirectional_iterator_tag, IterCat<Iterator>>, FlattenWrapper<Iterator, S>>> {

    Iterator _begin{};
    Iterator _current{};
    S _end{};

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;

    constexpr FlattenWrapper() = default;

    LZ_CONSTEXPR_CXX_20 FlattenWrapper(Iterator current, Iterator begin, S end) :
        _begin(std::move(begin)),
        _current(std::move(current)),
        _end(std::move(end)) {
    }

    void toEnd() {
        _current = _end;
    }

    LZ_CONSTEXPR_CXX_20 bool hasSome() const noexcept {
        return _current != _end;
    }

    LZ_CONSTEXPR_CXX_20 bool hasPrev() const noexcept {
        return _current != _begin;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const FlattenWrapper& b) const noexcept {
        return _current == b._current;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const noexcept {
        return _current == _end;
    }

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_current;
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_current;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_current;
    }
};

template<class, class, int>
class FlattenIterator;

template<class Iterator, class S, int N>
using Inner = FlattenIterator<decltype(std::begin(*std::declval<Iterator>())), decltype(std::begin(*std::declval<S>())), N - 1>;

template<class Iterator, class S, int N>
class FlattenIterator : public IterBase<FlattenIterator<Iterator, S, N>, RefType<Inner<Iterator, S, N>>,
                                        FakePointerProxy<RefType<Inner<Iterator, S, N>>>, DiffType<Inner<Iterator, S, N>>,
                                        CommonType<std::bidirectional_iterator_tag, IterCat<Iterator>>> {

    using ThisInner = Inner<Iterator, S, N>;

public:
    using reference = typename ThisInner::reference;
    using pointer = typename ThisInner::pointer;
    using value_type = typename ThisInner::value_type;
    using difference_type = typename ThisInner::difference_type;

private:
    LZ_CONSTEXPR_CXX_20 void advance() {
        if (_innerIter.hasSome()) {
            return;
        }
        for (++_outerIter; _outerIter.hasSome(); ++_outerIter) {
            const auto begin = std::begin(*_outerIter);
            _innerIter = { begin, begin, std::end(*_outerIter) };
            if (_innerIter.hasSome()) {
                return;
            }
        }
        _innerIter = {};
    }

    FlattenWrapper<Iterator, S> _outerIter{};
    ThisInner _innerIter{};

public:
    constexpr FlattenIterator() = default;

    LZ_CONSTEXPR_CXX_20 FlattenIterator(Iterator it, Iterator begin, S end) :
        _outerIter(std::move(it), std::move(begin), std::move(end)) {
        if (_outerIter.hasSome()) {
            const auto beg = std::begin(*_outerIter);
            _innerIter = { beg, beg, std::end(*_outerIter) };
            this->advance();
        }
    }

    LZ_CONSTEXPR_CXX_20 bool hasSome() const {
        return _outerIter.hasSome();
    }

    LZ_CONSTEXPR_CXX_20 bool hasPrev() const {
        return _innerIter.hasPrev() || _outerIter.hasPrev();
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const FlattenIterator& b) const noexcept {
        return _outerIter == b._outerIter && _innerIter == b._innerIter;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const noexcept {
        return _outerIter.hasSome() && _innerIter.hasSome();
    }

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_innerIter;
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_innerIter;
        this->advance();
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_innerIter.hasPrev()) {
            --_innerIter;
            return;
        }
        while (_outerIter.hasPrev()) {
            --_outerIter;
            const auto end = std::end(*_outerIter);
            _innerIter = { end, std::begin(*_outerIter), end };
            if (_innerIter.hasPrev()) {
                --_innerIter;
                return;
            }
        }
    }
};

template<class Iterator, class S>
class FlattenIterator<Iterator, S, 0>
    : public IterBase<FlattenIterator<Iterator, S, 0>, RefType<FlattenWrapper<Iterator, S>>,
                      FakePointerProxy<RefType<FlattenWrapper<Iterator, S>>>, DiffType<FlattenWrapper<Iterator, S>>,
                      CommonType<std::bidirectional_iterator_tag, IterCat<FlattenWrapper<Iterator, S>>>,
                      SentinelSelector<CommonType<std::bidirectional_iterator_tag, IterCat<FlattenWrapper<Iterator, S>>>,
                                       FlattenIterator<Iterator, S, 0>>> {

    FlattenWrapper<Iterator, S> _range;
    using Traits = std::iterator_traits<Iterator>;

public:
    using pointer = typename Traits::pointer;
    using reference = typename Traits::reference;
    using value_type = typename Traits::value_type;
    using difference_type = typename Traits::difference_type;

    constexpr FlattenIterator() = default;

    constexpr FlattenIterator(Iterator it, S begin, Iterator end) : _range(std::move(it), std::move(begin), std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 bool hasPrev() const noexcept {
        return _range.hasPrev();
    }

    LZ_CONSTEXPR_CXX_20 bool hasSome() const noexcept {
        return _range.hasSome();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_range;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const FlattenIterator& b) const noexcept {
        return _range == b._range;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const noexcept {
        return _range.hasSome();
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_range;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_range;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERATOR_HPP
