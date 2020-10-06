#pragma once

#ifndef LZ_JOIN_ITERATOR_HPP
#define LZ_JOIN_ITERATOR_HPP

#include <iterator>
#include "LzTools.hpp"

#include "fmt/format.h"

namespace lz { namespace internal {
	// ReSharper disable once CppUnnamedNamespaceInHeaderFile
	namespace {
        template<class T>
        struct IsFmtIntCompatible : std::integral_constant<bool,
            std::is_integral<T>::value &&
            !std::is_same<T, bool>::value &&
            !std::is_same<T, char>::value &&
            !std::is_same<T, unsigned char>::value &&
            !std::is_same<T, wchar_t>::value&&
#ifdef LZ_HAS_CXX_20
            !std::is_same<T, char8_t>::value&&
#endif
            !std::is_same<T, char16_t>::value &&
            !std::is_same<T, char32_t>::value> {
        };
    } // end anonymous


    template<LZ_CONCEPT_ITERATOR Iterator>
    class JoinIterator {
        using IterTraits = std::iterator_traits<Iterator>;
        using ContainerType = typename IterTraits::value_type;

    public:
        using value_type = std::string;
        using iterator_category = typename IterTraits::iterator_category;
        using difference_type = typename IterTraits::difference_type;
        using reference = Conditional<
            std::is_same<std::string, ContainerType>::value, typename IterTraits::reference, std::string>;
        using pointer = FakePointerProxy<reference>;

        template<class Val = ContainerType>
        EnableIf<IsFmtIntCompatible<Val>::value, std::string> getFormatted() const {
            return fmt::format_int(*_iterator).str();
        }

        template<class Val = ContainerType>
        EnableIf<!IsFmtIntCompatible<Val>::value, std::string> getFormatted() const {
            return fmt::format("{}", *_iterator);
        }

    private:
        Iterator _iterator{};
        mutable std::string _delimiter{};
        mutable bool _isIteratorTurn{ true };
        difference_type _distance{};

    public:
        JoinIterator(const Iterator iterator, std::string delimiter, const bool isIteratorTurn, const difference_type distance) :
            _iterator(iterator),
            _delimiter(std::move(delimiter)),
            _isIteratorTurn(isIteratorTurn),
            _distance(distance) {}

        JoinIterator() = default;

        template<class Val = ContainerType>
        EnableIf<!std::is_same<std::string, Val>::value, reference> operator*() const {
            if (_isIteratorTurn) {
                return getFormatted();
            }
            return _delimiter;
        }

        template<class Val = ContainerType>
        EnableIf<std::is_same<std::string, Val>::value, reference> operator*() const {
            if (_isIteratorTurn) {
                return *_iterator;
            }
            return _delimiter;
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        JoinIterator& operator++() {
            if (_isIteratorTurn) {
                ++_iterator;
            }
            _isIteratorTurn = !_isIteratorTurn;
            return *this;
        }

        JoinIterator& operator++() const {
            JoinIterator tmp(*this);
            ++* this;
            return tmp;
        }

        JoinIterator& operator--() {
            _isIteratorTurn = !_isIteratorTurn;
            if (_isIteratorTurn) {
                --_iterator;
            }
            return *this;
        }

        JoinIterator operator--(int) {
            JoinIterator tmp(*this);
            --* this;
            return tmp;
        }

        JoinIterator& operator+=(const difference_type offset) {
            _iterator += offset == 1 ? 1 : offset == _distance ? (offset >> 1) + 1 : offset >> 1;

            if (!isEven(offset)) {
                _isIteratorTurn = !_isIteratorTurn;
            }

            return *this;
        }

        JoinIterator& operator-=(const difference_type offset) {
            if (offset == 1) {
                _iterator -= 1;
            }
            else {
                _iterator -= (offset >> 1);
            }

            if (!isEven(offset)) {
                _isIteratorTurn = !_isIteratorTurn;
            }

            return *this;
        }

        JoinIterator operator+(const difference_type offset) const {
            JoinIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        difference_type operator-(const JoinIterator& other) const {
            // distance * 2 for delimiter, - 1 for removing last delimiter
            return (_iterator - other._iterator) * 2 - 1;
        }

        template<class Val = ContainerType, class = EnableIf<!std::is_same<std::string, Val>::value>>
        std::string operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        template<class Val = ContainerType, class = EnableIf<std::is_same<std::string, Val>::value>>
        std::string& operator[](const difference_type offset) const {
            // If we use *(*this + offset) when a delimiter must be returned, then we get a segfault because the operator+ returns a copy
            // of the delimiter
            if (_isIteratorTurn && isEven(offset)) {
                return *(*this + offset);
            }
            return _delimiter;
        }

        JoinIterator operator-(const difference_type offset) const {
            JoinIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        bool operator==(const JoinIterator& other) const {
            return _iterator == other._iterator;
        }

        bool operator!=(const JoinIterator& other) const {
            return !(*this == other);
        }

        bool operator<(const JoinIterator& other) const {
            return other - *this > 0;
        }

        bool operator>(const JoinIterator& other) const {
            return other < *this;
        }

        bool operator<=(const JoinIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const JoinIterator& other) const {
            return !(*this < other);
        }
    };
}} // end lz::internal

#endif