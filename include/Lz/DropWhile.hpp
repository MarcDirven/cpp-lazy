#pragma once

#include "detail/DropWhileIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<class Iterator, class Function>
    class DropWhile final : public detail::BasicIteratorView<detail::DropWhileIterator<Iterator, Function>> {
    public:
        using iterator = detail::DropWhileIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Creates a DropWhile iterator view object.
         * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns
         * false, no more values are being skipped.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
         */
        DropWhile(const Iterator begin, const Iterator end, const Function& predicate) :
            _begin(begin, end, predicate),
            _end(end, end, predicate) {
        }

        DropWhile() = default;

        /**
         * @brief Returns the beginning of the enumerate iterator object.
         * @return A random access DropWhile.
         */
        iterator begin() const {
            return _begin;
        }

        /**
         * @brief Returns the ending of the enumerate object.
         * @return A random access DropWhile.
         */
        iterator end() const {
            return _end;
        }
    };

    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Creates a DropWhile iterator view object.
     * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns false,
     * no more values are being skipped.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced. Must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @return A DropWhile iterator view object.
     */
    template<class Iterator, class Function>
    DropWhile<Iterator, Function> dropwhilerange(const Iterator begin, const Iterator end, const Function& predicate) {
        static_assert(std::is_same<detail::FunctionReturnType<Function, typename std::iterator_traits<Iterator>::value_type>, bool>::value,
                      "the function predicate must return a bool");
        return DropWhile<Iterator, Function>(begin, end, predicate);
    }

    /**
     * @brief Creates a DropWhile iterator view object.
     * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns false,
     * no more values are being skipped.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced. Must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @param iterable The sequence with the values that can be iterated over.
     * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
     * @return A DropWhile iterator view object.
     */
    template<class Iterable, class Function>
    auto dropwhile(Iterable&& iterable, const Function& predicate) -> DropWhile<decltype(std::begin(iterable)), Function> {
        return dropwhilerange(std::begin(iterable), std::end(iterable), predicate);
    }

    // End of group
    /**
     * @}
     */
}