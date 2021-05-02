#pragma once

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
	struct HasValueType : std::false_type {
	};

	template<class T>
	struct HasValueType<T, AliasWrapperT<typename T::value_type>> : std::true_type {
	};

	template<class T, class Enable = void>
	struct HasDifferenceType : std::false_type {
	};

	template<class T>
	struct HasDifferenceType<T, AliasWrapperT<typename T::reference>> : std::true_type {
	};

	template<class T, class Enable = void>
	struct HasPointer : std::false_type {
	};

	template<class T>
	struct HasPointer<T, AliasWrapperT<typename T::pointer>> : std::true_type {
	};

	template<class T, class Enable = void>
	struct HasIterCat : std::false_type {
	};

	template<class T>
	struct HasIterCat<T, AliasWrapperT<typename T::iterator_category>> : std::true_type {
	};

	template<class T, class Enable = void>
	struct HasReference : std::false_type {
	};

	template<class T>
	struct HasReference<T, AliasWrapperT<typename T::reference>> : std::true_type {
	};

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

	template<class T, class U = void>
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
	template<class Iterator>
	class FlattenWrapper {
		Iterator _begin{};
		Iterator _current{};
		Iterator _end{};

		using IterTraits = std::iterator_traits<Iterator>;
	public:
		using pointer = typename IterTraits::pointer;
		using reference = typename IterTraits::reference;
		using value_type = typename IterTraits::value_type;
		using iterator_category = LowestIterTypeT<std::bidirectional_iterator_tag, typename IterTraits::iterator_category>;
		using difference_type = typename IterTraits::difference_type;

		constexpr FlattenWrapper() = default;

		constexpr FlattenWrapper(Iterator current, Iterator begin, Iterator end) :
			_begin(std::move(begin)),
			_current(std::move(current)),
			_end(std::move(end)) {}

		constexpr bool hasSome() const {
			return _current != _end;
		}

		constexpr bool hasPrev() const {
			return _current != _begin;
		}

		constexpr friend bool operator!=(const FlattenWrapper& a, const FlattenWrapper& b) {
			return a._current != b._current;
		}

		constexpr friend bool operator==(const FlattenWrapper& a, const FlattenWrapper& b) {
			return !(a != b); // NOLINT
		}

		constexpr reference operator*() const {
			return *_current;
		}

		constexpr pointer operator->() const {
			return &*_current;
		}

		constexpr FlattenWrapper& operator++() {
			++_current;
			return *this;
		}

		constexpr FlattenWrapper operator++(int) {
			FlattenWrapper tmp(*this);
			++*this;
			return tmp;
		}

		constexpr FlattenWrapper& operator--() {
			--_current;
			return *this;
		}

		constexpr FlattenWrapper operator--(int) {
			FlattenWrapper tmp(*this);
			++*this;
			return tmp;
		}
	};


	template<class Iterator, int N>
	class FlattenIterator {
		using Inner = FlattenIterator<decltype(std::begin(*std::declval<Iterator>())), N - 1>;

	public:
		using pointer = typename Inner::pointer;
		using reference = typename Inner::reference;
		using value_type = typename Inner::value_type;
		using iterator_category = LowestIterTypeT<std::bidirectional_iterator_tag, typename Inner::iterator_category>;
		using difference_type = typename std::iterator_traits<Iterator>::difference_type;

	private:
		constexpr void advance() {
			if (_innerIter.hasSome()) { return; }

			for (++_outerIter; _outerIter.hasSome(); ++_outerIter) {
				_innerIter = Inner(std::begin(*_outerIter), std::begin(*_outerIter), std::end(*_outerIter));
				if (_innerIter.hasSome()) { return; }
			}
			_innerIter = Inner();
		}

		FlattenWrapper<Iterator> _outerIter{};
		Inner _innerIter{};

	public:
		constexpr FlattenIterator() = default;

		constexpr FlattenIterator(Iterator it, Iterator begin, Iterator end) :
			_outerIter(std::move(it), std::move(begin), std::move(end)) {
			if (_outerIter.hasSome()) {
				_innerIter = Inner(std::begin(*_outerIter), std::begin(*_outerIter), std::end(*_outerIter));
				this->advance();
			}
		}

		constexpr bool hasSome() const {
			return _outerIter.hasSome();
		}

		constexpr bool hasPrev() const {
			return _innerIter.hasPrev() || _outerIter.hasPrev();
		}

		constexpr friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) {
			return a._outerIter != b._outerIter || a._innerIter != b._innerIter;
		}

		constexpr friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) {
			return !(a != b); // NOLINT
		}

		constexpr reference operator*() const {
			return *_innerIter;
		}

		constexpr pointer operator->() const {
			return &*_innerIter;
		}

		constexpr FlattenIterator& operator++() {
			++_innerIter;
			this->advance();
			return *this;
		}

		constexpr FlattenIterator operator++(int) {
			FlattenIterator tmp(*this);
			++*this;
			return tmp;
		}

		constexpr FlattenIterator& operator--() {
			if (_innerIter.hasPrev()) {
				--_innerIter;
				return *this;
			}

			while (_outerIter.hasPrev()) {
				--_outerIter;
				_innerIter = Inner(std::end(*_outerIter), std::begin(*_outerIter), std::end(*_outerIter));
				if (_innerIter.hasPrev()) {
					--_innerIter;
					return *this;
				}

			}
			return *this;
		}

		constexpr FlattenIterator operator--(int) {
			FlattenIterator tmp(*this);
			--*this;
			return tmp;
		}
	};


	template<class Iterator>
	class FlattenIterator<Iterator, 0> {
		FlattenWrapper<Iterator> _range;
		using Traits = std::iterator_traits<Iterator>;

	public:
		using pointer = typename Traits::pointer;
		using reference = typename Traits::reference;
		using value_type = typename Traits::value_type;
		using iterator_category = LowestIterTypeT<std::bidirectional_iterator_tag, typename Traits::iterator_category>;
		using difference_type = typename std::iterator_traits<Iterator>::difference_type;

		constexpr FlattenIterator() = default;

		constexpr FlattenIterator(Iterator it, Iterator begin, Iterator end) :
			_range(std::move(it), std::move(begin), std::move(end)) {}

		constexpr bool hasSome() const {
			return _range.hasSome();
		}

		constexpr bool hasPrev() const {
			return _range.hasPrev();
		}

		constexpr reference operator*() const {
			return *_range;
		}

		constexpr pointer operator->() const {
			return &*_range;
		}

		constexpr friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) {
			return a._range != b._range;
		}

		constexpr friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) {
			return !(a != b); // NOLINT
		}

		constexpr FlattenIterator& operator++() {
			++_range;
			return *this;
		}

		constexpr FlattenIterator operator++(int) {
			FlattenIterator tmp(*this);
			++*this;
			return tmp;
		}

		constexpr FlattenIterator& operator--() {
			--_range;
			return *this;
		}

		constexpr FlattenIterator operator--(int) {
			FlattenIterator tmp(*this);
			--*this;
			return tmp;
		}
	};
}}

#endif // LZ_FLATTEN_ITERATOR_HPP
