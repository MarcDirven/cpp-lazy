#pragma once

#ifndef LZ_RANDOM_HPP
#define LZ_RANDOM_HPP

#include <random>

#include "detail/RandomIterator.hpp"
#include "detail/BasicIteratorView.hpp"
#include "Generate.hpp"


namespace lz { namespace internal {
		inline std::mt19937 createMtEngine() {
			std::random_device rd;
			const auto generator = lz::generate([&rd]() {
				return rd();
			}, ((std::mt19937::state_size * sizeof(std::mt19937::result_type)) - 1) / sizeof(std::random_device::result_type) + 1);
			std::seed_seq sd(generator.begin(), generator.end());
			return std::mt19937(sd);
		}
	}

	template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution, class Generator>
	class Random final : public internal::BasicIteratorView<internal::RandomIterator<Arithmetic, Distribution, Generator>> {
	public:
		using iterator = internal::RandomIterator<Arithmetic, Distribution, Generator>;
		using const_iterator = iterator;
		using value_type = typename iterator::value_type;

		/**
		 * @brief Random view object constructor, from [`min, max`].
		 * @param min The minimum value of the random number (included).
		 * @param max The maximum value of the random number (included).
		 * @param amount The amount of random numbers to generate. If `std::numeric_limits<size_t>::max()` it is
		 * interpreted as a `while-true` loop.
		 * @param isWhileTrueLoop Boolean to indicate if it's a while true loop.
		 */
		Random(const Distribution distribution, Generator& generator, const std::ptrdiff_t amount, const bool isWhileTrueLoop) :
			internal::BasicIteratorView<iterator>(iterator(distribution, generator, 0, isWhileTrueLoop),
												  iterator(distribution, generator, amount, isWhileTrueLoop)) {
		}

		Random() = default;

		/**
		 * Creates a new random number, not taking into account its size. This is for pure convenience. Example:
		 * ```cpp
		 * auto rand = lz::random(0, 5);
		 * for (int i = 0; i < 50_000; i++) {
		 *     int myRandom = rand.nextRandom();
		 * }
		 * ```
		 * @return A new random `value_type` between [min, max].
		 */
		value_type nextRandom() const {
			return *this->begin();
		}

		/**
		 * Gets the minimum random value.
		 * @return The min value
		 */
		value_type minRandom() const {
			return this->begin().min();
		}

		/**
		 * Gets the maximum random value.
		 * @return The max value
		 */
		value_type maxRandom() const {
			return this->begin().max();
		}
	};

	/**
	 * @addtogroup ItFns
	 * @{
	 */

	/**
	 * Creates a random number generator with specified generator and distribution.
	 * @param distribution A number distribution, for e.g. std::uniform_<type>_distribution<type>.
	 * @param generator A random number generator, for e.g. std::mt19937.
	 * @param amount The amount of numbers to create.
	 * @return A random view object that generates a sequence of `Generator::result_type`
	 */
	template<class Generator, class Distribution>
	Random<typename Distribution::result_type, Distribution, Generator>
	random(const Distribution& distribution, Generator& generator, const std::size_t amount = std::numeric_limits<std::size_t>::max()) {
		return Random<typename Distribution::result_type, Distribution, Generator>(
			distribution, generator, static_cast<std::ptrdiff_t>(amount), amount == std::numeric_limits<std::size_t>::max());
	}

	/**
	 * @brief Returns an output iterator view object that generates a sequence of random numbers, using a uniform distribution.
	 * @details This random access iterator view object can be used to generate a sequence of random numbers between
	 * [`min, max`]. It uses the std::mt19937 random engine and a seed of `std::random_device` as seed.
	 * @param min The minimum value , included.
	 * @param max The maximum value, included.
	 * @tparam Distribution The distribution for generating the random numbers. `std::uniform_int_distribution` by default.
	 * @tparam Generator The random number generator. `std::mt19937` by default.
	 * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<std::size_t>::max()`
	 * it is interpreted as a `while-true` loop.
	 * @return A random view object that generates a sequence of random numbers
	*/
	template<class Integral, internal::EnableIf<std::is_integral<Integral>::value, bool> = true>
	Random<Integral, std::uniform_int_distribution<Integral>, std::mt19937>
	random(const Integral min, const Integral max, const std::size_t amount = std::numeric_limits<std::size_t>::max()) {
		static std::mt19937 gen = internal::createMtEngine();
		std::uniform_int_distribution<Integral> dist(min, max);
		return random(dist, gen, amount);
	}

	/**
	 * @brief Returns an output view object that generates a sequence of floating point doubles, using a uniform
	 * distribution.
	 * @details This random access iterator view object can be used to generate a sequence of random doubles between
	 * [`min, max`]. It uses the std::mt19937 random engine and a seed of `std::random_device` as seed.
	 * @tparam Distribution The distribution for generating the random numbers. `std::uniform_real_distribution` by default.
	 * @tparam Generator The random number generator. `std::mt19937` by default.
	 * @param min The minimum value, included.
	 * @param max The maximum value, included.
	 * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<std::size_t>::max()`
	 * it is interpreted as a `while-true` loop.
	 * @return A random view object that generates a sequence of random floating point values.
	 */
	template<class Floating, internal::EnableIf<std::is_floating_point<Floating>::value, bool> = true>
	Random<Floating, std::uniform_real_distribution<Floating>, std::mt19937>
	random(const Floating min, const Floating max, const std::size_t amount = std::numeric_limits<std::size_t>::max()) {
		static std::mt19937 gen = internal::createMtEngine();
		std::uniform_real_distribution<Floating> dist(min, max);
		return random(dist, gen, amount);
	}

	// End of group
	/**
	 * @}
	 */
}

#endif