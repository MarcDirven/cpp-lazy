//
// Created by marcd on 28/10/2020.
//

#ifndef LZ_FLATTEN_ITERATOR_HPP
#define LZ_FLATTEN_ITERATOR_HPP

#include "LzTools.hpp"


namespace lz { namespace internal {
    template<class T, class U = void>
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
    struct HasDifferenceType<T, AliasWrapperT<typename T::reference>> : std::true_type {};

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
        static constexpr bool value = HasValueType<T>::value &&
                                      HasDifferenceType<T>::value &&
                                      HasPointer<T>::value &&
                                      HasReference<T>::value &&
                                      HasIterCat<T>::value;
    };

    template<class T, class = void>
    struct IterTraitsOrUnderlyingType {
        using Type = T;
    };

    template<class T>
    struct IterTraitsOrUnderlyingType<T, AliasWrapperT<typename T::iterator>> {
        using Type = std::iterator_traits<typename T::iterator>;
    };

    template<class T, class U= void>
    using IterTraitsOrUnderlyingTypeT = typename IterTraitsOrUnderlyingType<T, U>::Type;

    template<class T, bool IsCont>
    struct CountDimsHelper;

    template<class T>
    struct CountDimsHelper<T, true> {
        using Inner = IterTraitsOrUnderlyingTypeT<typename T::value_type>;
        static constexpr int value = 1 + CountDimsHelper<Inner, IsIterator<Inner>::value>::value;
    };

    template<class T>
    struct CountDimsHelper<T, false> {
        static constexpr int value = 0;
    };

    template<class T>
    struct CountDims {
        static constexpr int value = CountDimsHelper<T, IsIterator<T>::value>::value;
    };

    // Improvement of https://stackoverflow.com/a/21076724/8729023
    template<typename Iterator>
    class FlattenWrapper {
        Iterator _iterator{};
        Iterator _end{};

        using IterTraits = std::iterator_traits<Iterator>;
    public:
        using pointer = typename IterTraits::pointer;
        using reference = typename IterTraits::reference;
        using value_type = typename IterTraits::value_type;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = typename IterTraits::difference_type;

        FlattenWrapper() = default;

        FlattenWrapper(Iterator begin, Iterator end) :
            _iterator(begin),
            _end(end) {}

        bool hasSome() const {
            return _iterator != _end;
        }

        friend bool operator!=(const FlattenWrapper& a, const FlattenWrapper& b) {
            return a._iterator != b._iterator;
        }

        friend bool operator==(const FlattenWrapper& a, const FlattenWrapper& b) {
            return !(a != b);
        }

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &*_iterator;
        }

        FlattenWrapper& operator++() {
            ++_iterator;
            return *this;
        }

        FlattenWrapper operator++(int) {
            FlattenWrapper tmp(*this);
            ++* this;
            return tmp;
        }
    };

    template<typename Iterator, std::size_t N>
    class FlattenIterator {
        using Inner = FlattenIterator<decltype(std::begin(*std::declval<Iterator>())), N - 1>;

        void advance() {
            if (_iterInner.hasSome()) {
                return;
            }

            for (++_iterOuter; _iterOuter.hasSome(); ++_iterOuter) {
                _iterInner = Inner(std::begin(*_iterOuter), std::end(*_iterOuter));
                if (_iterInner.hasSome()) {
                    return;
                }
            }
            _iterInner = Inner();
        }

        FlattenWrapper<Iterator> _iterOuter{};
        Inner _iterInner{};

    public:
        using pointer = typename Inner::pointer;
        using reference = typename Inner::reference;
        using value_type = typename Inner::value_type;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;

        FlattenIterator() = default;

        FlattenIterator(Iterator begin, Iterator end) :
            _iterOuter(begin, end) {
            if (!_iterOuter.hasSome()) {
                return;
            }
            _iterInner = Inner(std::begin(*_iterOuter), std::end(*_iterOuter));
            this->advance();
        }

        bool hasSome() const {
            return _iterOuter.hasSome();
        }

        friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) {
            return a._iterOuter != b._iterOuter;
        }

        friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) {
            return !(a != b);
        }

        reference operator*() const {
            return *_iterInner;
        }

        pointer operator->() const {
            return &*_iterInner;
        }

        FlattenIterator& operator++() {
            ++_iterInner;
            this->advance();
            return *this;
        }

        FlattenIterator operator++(int) {
            FlattenIterator tmp(*this);
            ++* this;
            return tmp;
        }
    };

    template<typename Iterator>
    class FlattenIterator<Iterator, 0> {
        FlattenWrapper<Iterator> _range;

    public:
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;

        FlattenIterator() = default;

        FlattenIterator(Iterator begin, Iterator end) :
            _range(begin, end) {}

        bool hasSome() const {
            return _range.hasSome();
        }

        reference operator*() const {
            return *_range;
        }

        pointer operator->() const {
            return &*_range;
        }

        friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) {
            return a._range != b._range;
        }

        friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) {
            return !(a != b);
        }

        FlattenIterator& operator++() {
            ++_range;
            return *this;
        }

        FlattenIterator operator++(int) {
            FlattenIterator tmp(*this);
            ++* this;
            return tmp;
        }
    };

}}

#endif // LZ_FLATTEN_ITERATOR_HPP
