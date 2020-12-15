#pragma once

#ifndef LZ_RANDOM_HPP
#define LZ_RANDOM_HPP

#include "detail/RandomIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution>
    class Random final : public internal::BasicIteratorView<internal::RandomIterator<Arithmetic, Distribution>> {
    public:
        using iterator = internal::RandomIterator<Arithmetic, Distribution>;
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
        Random(const Arithmetic min, const Arithmetic max, const std::ptrdiff_t amount, const bool isWhileTrueLoop) :
            internal::BasicIteratorView<iterator>(iterator(min, max, 0, isWhileTrueLoop), iterator(min, max, amount, isWhileTrueLoop))
        {
        }

        Random() = default;
    };
	
    /**
     * @addtogroup ItFns
     * @{
     */

     /**
      * @brief Returns a random view object that generates a sequence of random numbers, using a uniform distribution.
      * @details This random access iterator view object can be used to generate a sequence of random numbers between
      * [`min, max`]. It uses the std::mt19937 random engine and a seed sequence (8x) of `std::random_device` as seed.
      * @param min The minimum value , included.
      * @param max The maximum value, included.
      * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<std::ptrdiff_t>::max()`
      * it is interpreted as a `while-true` loop.
      * @return A random view object that generates a sequence of random numbers
      */
    template<class Integral, class Distribution = std::uniform_int_distribution<Integral>>
    static internal::EnableIf<std::is_integral<Integral>::value, Random<Integral, Distribution>>
    random(const Integral min, const Integral max, const std::ptrdiff_t amount = std::numeric_limits<std::ptrdiff_t >::max()) {
        return Random<Integral, Distribution>(min, max, amount, amount == std::numeric_limits<std::ptrdiff_t >::max());
    }

    /**
     * @brief Returns a random access view object that generates a sequence of floating point doubles, using a uniform
     * distribution.
     * @details This random access iterator view object can be used to generate a sequence of random doubles between
     * [`min, max`]. It uses the std::mt19937 random engine and a seed sequence (8x) of `std::random_device` as seed.
     * @param min The minimum value, included.
     * @param max The maximum value, included.
     * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<std::ptrdiff_t>::max()`
     * it is interpreted as a `while-true` loop.
     * @return A random view object that generates a sequence of random doubles.
     */
    template<class Floating, class Distribution = std::uniform_real_distribution<Floating>>
    static internal::EnableIf<std::is_floating_point<Floating>::value, Random<Floating, Distribution>>
    random(const Floating min, const Floating max, const std::ptrdiff_t amount = std::numeric_limits<std::ptrdiff_t>::max()) {
        return Random<Floating, Distribution>(min, max, amount, amount == std::numeric_limits<std::ptrdiff_t>::max());
    }

    // End of group
    /**
     * @}
     */
}

#endif