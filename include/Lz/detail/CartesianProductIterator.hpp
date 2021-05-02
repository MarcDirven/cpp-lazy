#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include "Lz/detail/LzTools.hpp"


namespace lz { namespace internal {
	// Edited version of https://github.com/mirandaconrado/product-iterator
	template<class... Iterators>
	class CartesianProductIterator {
	public:
		using value_type = std::tuple<typename std::iterator_traits<Iterators>::value_type...>;
		using reference = std::tuple<typename std::iterator_traits<Iterators>::reference...>;
		using pointer = std::tuple<typename std::iterator_traits<Iterators>::pointer...>;
		using iterator_category = LowestIterTypeT<
			std::forward_iterator_tag, typename std::iterator_traits<Iterators>::iterator_category...>;
		using difference_type = std::ptrdiff_t;

	private:
		std::tuple<Iterators...> _begin{};
		std::tuple<Iterators...> _iterator{};
		std::tuple<Iterators...> _end{};

		template<std::size_t I>
		constexpr EnableIf<I == 0> next() const {}

#ifdef LZ_MSVC
  #pragma warning(push)
  #pragma warning(disable:4127)
#endif

		template<std::size_t I>
		LZ_CONSTEXPR_CXX_14 EnableIf<(I > 0)> next() {
			auto& prev = std::get<I - 1>(_iterator);
			++prev;

			if (prev == std::get<I - 1>(_end)) {
				if LZ_CONSTEXPR_IF (I != 1) {
					prev = std::get<I - 1>(_begin);
					next<I - 1>();
				}
			}
		}

#ifdef LZ_MSVC
  #pragma warning(pop)
#endif

		template<std::size_t... Is>
		LZ_CONSTEXPR_CXX_14 reference dereference(IndexSequence<Is...>) {
			return {*std::get<Is>(_iterator)...};
		}

	public:
		constexpr CartesianProductIterator() = default;

		LZ_CONSTEXPR_CXX_14 CartesianProductIterator(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
			_begin(begin),
			_iterator(begin),
			_end(std::move(end)) {}

		LZ_CONSTEXPR_CXX_14 reference operator*() {
			return dereference(MakeIndexSequence<sizeof...(Iterators)>());
		}

		LZ_CONSTEXPR_CXX_14 CartesianProductIterator& operator++() {
			if (std::get<0>(_iterator) == std::get<0>(_end)) {
				return *this;
			}
			next<sizeof...(Iterators)>();
			return *this;
		}

		LZ_CONSTEXPR_CXX_14 CartesianProductIterator operator++(int) {
			CartesianProductIterator tmp(*this);
			++*this;
			return tmp;
		}

		LZ_CONSTEXPR_CXX_14 friend bool operator==(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) {
			return std::get<0>(lhs._iterator) == std::get<0>(rhs._iterator);
		}

		LZ_CONSTEXPR_CXX_14 friend bool operator!=(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) {
			return !(lhs == rhs); // NOLINT
		}
	};
}}
#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
