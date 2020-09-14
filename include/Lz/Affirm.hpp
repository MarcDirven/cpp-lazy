#pragma once

#include "detail/AffirmIterator.hpp"
#include "detail/BasicIteratorView.hpp"
#include "detail/LzTools.hpp"


namespace lz {
    template<class Exception, class Iterator, class Function>
    class Affirm final : public detail::BasicIteratorView<detail::AffirmIterator<Exception, Iterator, Function>> {
    public:
        using iterator = detail::AffirmIterator<Exception, Iterator, Function>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        detail::AffirmIteratorHelper<Iterator, Exception, value_type> _helper{};
        Iterator _begin{};
        Iterator _end{};

        using FunctionReturnType = detail::FunctionReturnType<Function, decltype(*std::declval<Iterator>())>;
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
        Affirm(const Iterator begin, const Iterator end, Exception&& exception, const Function& predicate) :
            _helper{predicate, std::forward<Exception>(exception)},
            _begin(begin),
            _end(end) {}

        Affirm() = default;

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const {
            return iterator(_begin, &_helper);
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const {
            return iterator(_end, &_helper);
        }
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
     * @tparam Exception Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param exception Something throwable. In C++ everything is throwable.
     * @param predicate The predicate. If true is returned from the function, the value is returned from the iterator.
     * If false is returned, it throws `exception`.
     * @return An Affirm view object, that can be iterated over
     */
    template<class Exception, class Iterator, class Function>
    Affirm<Exception, Iterator, Function>
    affirmrange(const Iterator begin, const Iterator end, Exception&& exception, const Function& predicate) {
        return Affirm<Exception, Iterator, Function>(begin, end, std::forward<Exception>(exception), predicate);
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
     * @tparam Exception Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable The iterable sequence.
     * @param exception Something throwable. In C++ everything is throwable.
     * @param predicate The predicate. If true is returned from the function, the value is returned from the iterator.
     * If false is returned, it throws `exception`.
     * @return An Affirm view object, that can be iterated over
     */
    template<class Exception, class Iterable, class Function>
    auto affirm(Iterable&& iterable, Exception&& exception, const Function& predicate) ->
    Affirm<Exception, decltype(std::begin(iterable)), Function> {
        return affirmrange(std::begin(iterable), std::end(iterable), std::forward<Exception>(exception), predicate);
    }

    // End of group
    /**
     * @}
     */
}