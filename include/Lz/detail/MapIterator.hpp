#pragma once

#ifndef LZ_MAP_ITERATOR_HPP
#define LZ_MAP_ITERATOR_HPP


#include "LzTools.hpp"
#include "FunctionContainer.hpp"


namespace lz { namespace internal {
	template<LZ_CONCEPT_ITERATOR Iterator, class Function>
	class MapIterator {
		Iterator _iterator{};
		mutable FunctionContainer<Function> _function{};

		using IterTraits = std::iterator_traits<Iterator>;

	public:
		using reference = decltype(_function(*_iterator));
		using value_type = Decay<reference>;
		using iterator_category = typename IterTraits::iterator_category;
		using difference_type = typename IterTraits::difference_type;
		using pointer = FakePointerProxy<reference>;

		constexpr MapIterator(Iterator iterator, Function function) :
			_iterator(std::move(iterator)),
			_function(std::move(function)) {
		}

		constexpr MapIterator() = default;

		constexpr reference operator*() const {
			return _function(*_iterator);
		}

		constexpr pointer operator->() const {
			return FakePointerProxy<decltype(**this)>(**this);
		}

		constexpr MapIterator& operator++() {
			++_iterator;
			return *this;
		}

		constexpr MapIterator operator++(int) {
			MapIterator tmp(*this);
			++*this;
			return tmp;
		}

		constexpr MapIterator& operator--() {
			--_iterator;
			return *this;
		}

		constexpr MapIterator operator--(int) {
			MapIterator tmp(*this);
			--*this;
			return tmp;
		}

		constexpr MapIterator& operator+=(const difference_type offset) {
			_iterator += offset;
			return *this;
		}

		constexpr MapIterator& operator-=(const difference_type offset) {
			_iterator -= offset;
			return *this;
		}

		constexpr MapIterator operator+(const difference_type offset) const {
			MapIterator tmp(*this);
			tmp += offset;
			return tmp;
		}

		constexpr MapIterator operator-(const difference_type offset) const {
			MapIterator tmp(*this);
			tmp -= offset;
			return tmp;
		}

		constexpr difference_type operator-(const MapIterator& other) const {
			return _iterator - other._iterator;
		}

		constexpr reference operator[](const difference_type offset) const {
			return *(*this + offset);
		}

		constexpr friend bool operator==(const MapIterator& a, const MapIterator& b) {
			return !(a != b); // NOLINT
		}

		constexpr friend bool operator!=(const MapIterator& a, const MapIterator& b) {
			return a._iterator != b._iterator;
		}

		constexpr friend bool operator<(const MapIterator& a, const MapIterator& b) {
			return a._iterator < b._iterator;
		}

		constexpr friend bool operator>(const MapIterator& a, const MapIterator& b) {
			return b < a;
		}

		constexpr friend bool operator<=(const MapIterator& a, const MapIterator& b) {
			return !(b < a); // NOLINT
		}

		constexpr friend bool operator>=(const MapIterator& a, const MapIterator& b) {
			return !(a < b); // NOLINT
		}
	};
}} // end lz::internal

#endif