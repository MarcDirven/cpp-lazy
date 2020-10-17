#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include "detail/RepeatIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<class T>
    class Repeat final : public internal::BasicIteratorView<internal::RepeatIterator<T>> {
    public:
        using iterator = internal::RepeatIterator<T>;
        using value_type = T;

    private:
        internal::RepeatIteratorHelper<T> _iteratorHelper{};
        std::size_t _amount{};

    public:
        /**
         * @brief Repeat object constructor.
         * @param toRepeat The value to repeat `amount` times.
         * @param amount The amount of times to repeat the loop, returning `toRepeat`.
         */
        Repeat(T toRepeat, const std::size_t amount):
            internal::BasicIteratorView<iterator>(iterator(), iterator()),
            _iteratorHelper(std::move(toRepeat), amount),
            _amount(amount)
        {
        }

        Repeat() = default;

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const override {
            return iterator(&_iteratorHelper, 0);
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const override {
            return iterator(&_iteratorHelper, _amount);
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Returns `toRepeat`, `amount` of times. If amount is left empty, it never stops and is interpreted as a
     * `while-true` loop. The repeat view object may not outlive its iterator i.e. they must have the same lifetime.
     * @param toRepeat The value to repeat `amount` times.
     * @param amount The amount of times to repeat the loop, returning `toRepeat`.
     * @return A repeat object, containing the random access iterator.
     */
    template<class T>
    Repeat<T> repeat(T toRepeat, const std::size_t amount = std::numeric_limits<std::size_t>::max()) {
        return Repeat<T>(std::move(toRepeat), amount);
    }

    //End of group
    /**
     * @}
     */
}

#endif