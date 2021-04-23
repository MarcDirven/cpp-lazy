#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include <random>

#include "LzTools.hpp"


namespace lz { namespace internal {
	template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution, class Generator>
	class RandomIterator {
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = Arithmetic;
		using difference_type = std::ptrdiff_t;
		using pointer = FakePointerProxy<Arithmetic>;
		using reference = value_type;
		using result_type = value_type;

	private:
		std::ptrdiff_t _current{};
		bool _isWhileTrueLoop{};
		Distribution _distribution{};
		Generator* _generator{nullptr};

	public:
		explicit RandomIterator(const Distribution distribution, Generator& generator, const std::ptrdiff_t current,
								const bool isWhileTrueLoop) :
			_current(current),
			_isWhileTrueLoop(isWhileTrueLoop),
			_distribution(distribution),
			_generator(&generator) {
		}

		RandomIterator() = default;

		value_type operator*() const {
			return _distribution(*_generator);
		}

		result_type (min)() {
			return _distribution->min();
		}

		result_type (max)() {
			return _distribution->max();
		}

		value_type operator()() const {
			return _distribution(*_generator);
		}

		pointer operator->() const {
			return FakePointerProxy<decltype(**this)>(**this);
		}

		RandomIterator& operator++() {
			if (!_isWhileTrueLoop) {
				++_current;
			}
			return *this;
		}

		RandomIterator operator++(int) {
			RandomIterator tmp(*this);
			++*this;
			return tmp;
		}

		friend bool operator!=(const RandomIterator& a, const RandomIterator& b) {
			return a._current != b._current;
		}

		friend bool operator==(const RandomIterator& a, const RandomIterator& b) {
			return !(a != b); // NOLINT
		}
	};
}}

#endif