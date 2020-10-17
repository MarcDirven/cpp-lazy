#pragma once

#ifndef LZ_AFFIRM_HPP
#define LZ_AFFIRM_HPP

#include "detail/AffirmIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<class Exception, class Iterator, class Function>
    class Affirm final : public internal::BasicIteratorView<internal::AffirmIterator<Exception, Iterator, Function>> {
    public:
        using iterator = internal::AffirmIterator<Exception, Iterator, Function>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        using FunctionReturnType = internal::FunctionReturnType<Function, decltype(*std::declval<Iterator>())>;
        static_assert(std::is_same<FunctionReturnType, bool>::value, "function predicate must return bool");

    public:
        /**
         * @brief Creates an affirm view object of the iterator.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param exception The exception or value to throw when predicate returns false.
         * @param predicate The function that checks whether the iterator has met the condition(s). If false is returned, the exception
         * `exception` is thrown.
         */
        Affirm(Iterator begin, Iterator end, Exception exception, Function predicate) :
            internal::BasicIteratorView<iterator>(iterator(std::move(begin), predicate, exception),
                                                  iterator(std::move(end), predicate, exception))
        {
        }

        Affirm() = default;
    };
    /**
    * @addtogroup ItFns
    * @{
    */

    /**
     * @brief Returns an Affirm view object, that can be iterated over.
     * @details This iterator checks whether predicate returns true or false. If true is returned from the function,
     * the value is returned from the iterator. If false is returned, it throws `exception`. The best way to iterate
     * over the sequence is (if using `for (... : affirm) {}` you're not able to try-catch each individual element):
     * auto beg = affirm.begin();
     * auto end = affirm.end();
     *
     * while (beg != end) {
     *      try {
     *             std::cout << *beg << '\n';
     *      }
     *      catch (const std::logic_error& err) {
     *          std::cout << err.what() << '\n';
     *      }
     *      ++beg;
     * }
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param exception Something throwable. In C++ everything is throwable.
     * @param predicate The predicate. If true is returned from the function, the value is returned from the iterator.
     * If false is returned, it throws `exception`.
     * @return An Affirm view object, that can be iterated over
     */
    template<class Exception, class Iterator, class Function>
    Affirm<Exception, Iterator, Function>
    affirmRange(Iterator begin, Iterator end, Exception&& exception, Function predicate) {
        return Affirm<Exception, Iterator, Function>(std::move(begin), std::move(end), std::forward<Exception>(exception),
                                                     std::move(predicate));
    }

    /**
     * @brief Returns an Affirm view object, that can be iterated over.
     * @details This iterator checks whether predicate returns true or false. If true is returned from the function,
     * the value is returned from the iterator. If false is returned, it throws `exception`. The best way to iterate
     * over the sequence is (if using `for (... : affirm) {}` you're not able to try-catch each individual element):
     * ```cpp
     * auto beg = affirm.begin();
     * auto end = affirm.end();
     *
     * while (beg != end) {
     *      try {
     *             std::cout << *beg << '\n';
     *      }
     *      catch (const std::logic_error& err) {
     *          std::cout << err.what() << '\n';
     *      }
     *      ++beg;
     * }
     * ```
     * @param iterable The iterable sequence.
     * @param exception Something throwable. In C++ everything is throwable.
     * @param predicate The predicate. If true is returned from the function, the value is returned from the iterator.
     * If false is returned, it throws `exception`.
     * @return An Affirm view object, that can be iterated over
     */
    template<class Exception, class Iterable, class Function>
    Affirm<Exception, internal::IterTypeFromIterable<Iterable>, Function> affirm(Iterable&& iterable, Exception&& exception,
                                                                                 Function predicate) {
        return affirmRange(std::begin(iterable), std::end(iterable), std::forward<Exception>(exception), std::move(predicate));
    }

    // End of group
    /**
     * @}
     */
}

#endif