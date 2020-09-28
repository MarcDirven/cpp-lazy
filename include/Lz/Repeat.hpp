#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include "detail/RepeatIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<class T>
    class Repeat final : public detail::BasicIteratorView<detail::RepeatIterator<T>> {
    public:
        using iterator = detail::RepeatIterator<T>;
        using value_type = T;

    private:
        detail::RepeatIteratorHelper<T> _iteratorHelper{};
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Repeat object constructor.
         * @param toRepeat The value to repeat `amount` times.
         * @param amount The amount of times to repeat the loop, returning `toRepeat`.
         */
        Repeat(T toRepeat, const size_t amount):
            _iteratorHelper{std::move(toRepeat), amount == std::numeric_limits<size_t>::max()},
            _begin(&_iteratorHelper, 0),
            _end(&_iteratorHelper, amount)
            {
        }

        Repeat() = default;

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const override {
            return _begin;
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const override {
            return _end;
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Returns `toRepeat`, `amount` of times. If amount is left empty, it never stops and is interpreted as a
     * `while-true` loop.
     * @tparam T Is automatically deduced by the compiler.
     * @param toRepeat The value to repeat `amount` times.
     * @param amount The amount of times to repeat the loop, returning `toRepeat`.
     * @return A repeat object, containing the random access iterator.
     */
    template<class T>
    Repeat<T> repeat(T toRepeat, const size_t amount = std::numeric_limits<size_t>::max()) {
        return Repeat<T>(std::move(toRepeat), amount);
    }

    //End of group
    /**
     * @}
     */
}

#endif