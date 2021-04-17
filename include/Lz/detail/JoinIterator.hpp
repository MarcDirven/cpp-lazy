#pragma once

#ifndef LZ_JOIN_ITERATOR_HPP
#define LZ_JOIN_ITERATOR_HPP

#include "LzTools.hpp"
#define LZ_STANDALONE

#ifdef LZ_STANDALONE
  #ifdef LZ_HAS_FORMAT
	#include <format>
  #else
    #include <sstream>
  #endif // LZ_HAS_FORMAT
#else
  #include <fmt/ostream.h>
#endif


namespace lz { namespace internal {
#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
	template<class T, EnableIf<std::is_arithmetic<T>::value, bool> = true>
	std::string toStringSpecialized(const T& value) {
		return std::to_string(value);
	}

	template<class T, EnableIf<!std::is_arithmetic<T>::value, bool> = true>
	std::string toStringSpecialized(const T& value) {
		std::ostringstream oss;
		return (oss << value).str();
	}
#endif // defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))

	template<class T>
	std::string toStringJoinImpl(const T& value) {
#ifdef LZ_STANDALONE
  #ifdef LZ_HAS_FORMAT
		return std::format("{}", value);
  #else
		return toStringSpecialized(value);
  #endif // LZ_HAS_FORMAT
#else
		return fmt::format("{}", value);
#endif // LZ_STANDALONE
	}

    template<LZ_CONCEPT_ITERATOR Iterator>
    class JoinIterator {
        using IterTraits = std::iterator_traits<Iterator>;
        using ContainerType = typename IterTraits::value_type;
        using IsContainerTypeString = std::is_same<ContainerType, std::string>;

    public:
        using value_type = std::string;
        using iterator_category = typename IterTraits::iterator_category;
        using difference_type = typename IterTraits::difference_type;
        using reference = Conditional<
            std::is_same<std::string, ContainerType>::value, typename IterTraits::reference, std::string>;
        using pointer = FakePointerProxy<reference>;

    private:
        Iterator _iterator{};
        mutable std::string _delimiter{};
        mutable bool _isIteratorTurn{ true };
        difference_type _distance{};

        reference deref(std::false_type /* isSameContainerTypeString */) const {
            if (_isIteratorTurn) {
            	return toStringJoinImpl(*_iterator);
            }
            return _delimiter;
        }

        reference deref(std::true_type /* isSameContainerTypeString */) const {
            if (_isIteratorTurn) {
                return *_iterator;
            }
            return _delimiter;
        }

        reference indexOperator(std::true_type /* isSameContainerTypeString */, const difference_type offset) const {
            // If we use *(*this + offset) when a delimiter must be returned, then we get a segfault because the operator+ returns a copy
            // of the delimiter
            if (_isIteratorTurn && isEven(offset)) {
                return *(*this + offset);
            }
            return _delimiter;
        }

        reference indexOperator(std::false_type /* isSameContainerTypeString */, const difference_type offset) const {
            return *(*this + offset);
        }

    public:
        JoinIterator(Iterator iterator, std::string delimiter, const bool isIteratorTurn, const difference_type distance) :
            _iterator(std::move(iterator)),
            _delimiter(std::move(delimiter)),
            _isIteratorTurn(isIteratorTurn),
            _distance(distance) {}

        JoinIterator() = default;

        reference operator*() const {
            return deref(IsContainerTypeString());
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

        JoinIterator& operator++(int) {
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

        reference operator[](const difference_type offset) const {
            return indexOperator(IsContainerTypeString(), offset);
        }

        JoinIterator operator-(const difference_type offset) const {
            JoinIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        friend bool operator==(const JoinIterator& a, const JoinIterator& b) {
            return a._iterator == b._iterator;
        }

        friend bool operator!=(const JoinIterator& a, const JoinIterator& b) {
            return !(a == b); // NOLINT
        }

        friend bool operator<(const JoinIterator& a, const JoinIterator& b) {
            return b - a > 0;
        }

        friend bool operator>(const JoinIterator& a, const JoinIterator& b) {
            return b < a;
        }

        friend bool operator<=(const JoinIterator& a, const JoinIterator& b) {
            return !(b < a); // NOLINT
        }

        friend bool operator>=(const JoinIterator& a, const JoinIterator& b) {
            return !(a < b); // NOLINT
        }
    };
}} // end lz::internal

#endif