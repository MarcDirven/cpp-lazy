#pragma once

#include <type_traits>
#include <limits>
#include <cstddef>

#include <Lz/detail/RandomIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution>
    class Random final : public internal::BasicIteratorView<internal::RandomIterator<Arithmetic, Distribution>> {
    public:
        using iterator = internal::RandomIterator<Arithmetic, Distribution>;
        using const_iterator = iterator;

    private:
        size_t _amount{};
        detail::RandomIteratorHelper<Arithmetic, Distribution> _helper;

    public:
        /**
         * @brief Random view object constructor, from [`min, max`].
         * @param min The minimum value of the random number (included).
         * @param max The maximum value of the random number (included).
         * @param amount The amount of random numbers to generate. If `std::numeric_limits<size_t>::max()` it is
         * interpreted as a `while-true` loop.
         */
        Random(const Arithmetic min, const Arithmetic max, const size_t amount) :
            _amount(amount),
            _helper(min, max, amount) {
        }

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const override {
            return iterator(0, &_helper);
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const override {
            return iterator(_amount, &_helper);
        }
    };

    template class Random<int, std::uniform_int_distribution<int>>;
    template class Random<double, std::uniform_real_distribution<double>>;

    /**
     * @brief Returns a random view object that generates a sequence of random numbers, using a uniform distribution.
     * @details This random access iterator view object can be used to generate a sequence of random numbers between
     * [`min, max`]. It uses the std::mt19937 random engine and a seed sequence (8x) of `std::random_device` as seed.
     * @tparam Arithmetic Is automatically deduced. Must be arithmetic type.
     * @param min The minimum value , included.
     * @param max The maximum value, included.
     * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<size_t>::max()`
     * it is interpreted as a `while-true` loop.
     * @return A random view object that generates a sequence of random numbers
     */
    template<class Arithmetic>
    static auto
    random(const Arithmetic min, const Arithmetic max, const size_t amount = std::numeric_limits<size_t>::max()) {
        static_assert(std::is_arithmetic<Arithmetic>::value, "template parameter is not arithmetic");

     /**
      * @brief Returns a random view object that generates a sequence of random numbers, using a uniform distribution.
      * @details This random access iterator view object can be used to generate a sequence of random numbers between
      * [`min, max`]. It uses the std::mt19937 random engine and a seed sequence (8x) of `std::random_device` as seed.
      * @param min The minimum value , included.
      * @param max The maximum value, included.
      * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<size_t>::max()`
      * it is interpreted as a `while-true` loop.
      * @return A random view object that generates a sequence of random numbers
      */
    template<class Integral, class Distribution = std::uniform_int_distribution<Integral>>
    static internal::EnableIf<std::is_integral<Integral>::value, Random<Integral, Distribution>>
    random(const Integral min, const Integral max, const std::size_t amount = std::numeric_limits<std::size_t>::max()) {
        return Random<Integral, Distribution>(min, max, amount, amount == std::numeric_limits<std::size_t>::max());
    }

    /**
     * @brief Returns a random access view object that generates a sequence of random floats, using a uniform
     * distribution.
     * @details This random access iterator view object can be used to generate a sequence of random floats between
     * [`min, max`]. It uses the std::mt19937 random engine and a seed sequence (8x) of `std::random_device` as seed.
     * @param min The minimum value, included.
     * @param max The maximum value, included.
     * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<size_t>::max()`
     * it is interpreted as a `while-true` loop.
     * @return A random view object that generates a sequence of random floats.
     */
    template<>
    auto random(const float min, const float max, const size_t amount) {
        return Random<float, std::uniform_real_distribution<float>>(min, max, amount);
    }

    /**
     * @brief Returns a random access view object that generates a sequence of random doubles, using a uniform
     * distribution.
     * @details This random access iterator view object can be used to generate a sequence of random doubles between
     * [`min, max`]. It uses the std::mt19937 random engine and a seed sequence (8x) of `std::random_device` as seed.
     * @param min The minimum value, included.
     * @param max The maximum value, included.
     * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<size_t>::max()`
     * it is interpreted as a `while-true` loop.
     * @return A random view object that generates a sequence of random doubles.
     */
    template<class Floating, class Distribution = std::uniform_real_distribution<Floating>>
    static internal::EnableIf<std::is_floating_point<Floating>::value, Random<Floating, Distribution>>
    random(const Floating min, const Floating max, const std::size_t amount = std::numeric_limits<std::size_t>::max()) {
        return Random<Floating, Distribution>(min, max, amount, amount == std::numeric_limits<std::size_t>::max());
    }

	template<class T, class>
	static internal::EnableIf<!std::is_arithmetic<T>::value>
	random(const T, const T, const std::size_t) {
        static_assert(true, "T must be an arithmetic type");
    }

    // End of group
    /**
     * @brief Returns a random access view object that generates a sequence of random long doubles, using a uniform
     * distribution.
     * @details This random access iterator view object can be used to generate a sequence of random long doubles
     * between [`min, max`]. It uses the std::mt19937 random engine and a seed of (8x) `std::random_device`.
     * @param min The minimum value, included.
     * @param max The maximum value, included.
     * @param amount The amount of numbers to create. If left empty or equal to `std::numeric_limits<size_t>::max()`
     * it is interpreted as a `while-true` loop.
     * @return A random view object that generates a sequence of random long doubles.
     */
    template<>
    auto random(const long double min, const long double max, const size_t amount) {
        return Random<long double, std::uniform_real_distribution<long double>>(min, max, amount);
    }
}