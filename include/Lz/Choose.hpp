#pragma once

#include <Lz/detail/ChooseIterator.hpp>
#include <Lz/detail/BasicIteratorView.hpp>


namespace lz {
    template<class Iterator, class Function>
    class Choose final
        : public detail::BasicIteratorView<detail::ChooseIterator<Iterator, Function>> {
    public:
        using IteratorType = typename std::iterator_traits<Iterator>::value_type;
        using iterator = detail::ChooseIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        Iterator _begin;
        Iterator _end;
        detail::ChooseIteratorHelper<Iterator, Function> _helper;

    public:
        /**
         * @brief Choose view iterator object constructor.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param function A function that returns a std::pair<bool, T> and takes a value type of its corresponding
         * container as parameter.
         */
        Choose(const Iterator begin, const Iterator end, const Function function):
            _begin(begin),
            _end(end),
            _helper{function}
        {}

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const override {
            return iterator(_begin, _end, &_helper);
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const override {
            return iterator(_end, _end, &_helper);
        }
    };

    /**
     * @brief Returns a forward iterator view object.
     * @details This forward iterator view object applies a function over every item in the iterator. The function
     * must return a std::pair<bool, T>, where T may be anything. If the bool is `true` the value T in the pair
     * is returned, otherwise it skips that element. Example:
     * ```cpp
     * std::string values = "1234eee56";
     * auto func = [](const char c) {
     *      // if pair.first is true, pair.second is returned, which makes an int from a char.
     *      return std::make_pair(std::isdigit(c), static_cast<int>(c - '0'));
     * }
     * auto chooser = lz::choose(values.begin(), values.end(), func);
     * for (int i : chooser) {
     *      std::cout << i << '\n';
     * }
     * // output:
     * // 1
     * // 2
     * // 3
     * // 4
     * // 5
     * // 6
     * ```
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced/
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param function A function that returns a std::pair<bool, T> and takes a value type of its corresponding
     * container as parameter.
     * @return A choose view object.
     */
    template<class Iterator, class Function>
    auto chooserange(const Iterator begin, const Iterator end, const Function function) {
        using First = decltype(function(*begin).first);
        static_assert(std::is_same<First, bool>::value, "the function must return a std::pair<bool, T>");

        return Choose<Iterator, Function>(begin, end, function);
    }

    /**
     * @brief Returns a forward iterator view object.
     * @details This forward iterator view object applies a function over every item in the iterator. The function
     * must return a std::pair<bool, T>, where T may be anything. If the bool is `true` the value T in the pair
     * is returned, otherwise it skips that element. Example:
     * ```cpp
     * std::string values = "1234eee56";
     * auto func = [](const char c) {
     *      // if pair.first is true, pair.second is returned, which makes an int from a char.
     *      return std::make_pair(std::isdigit(c), static_cast<int>(c - '0'));
     * }
     * auto chooser = lz::choose(values.begin(), values.end(), func);
     * for (int i : chooser) {
     *      std::cout << i << '\n';
     * }
     * // output:
     * // 1
     * // 2
     * // 3
     * // 4
     * // 5
     * // 6
     * ```
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced/
     * @param iterable A container or sequence.
     * @param function A function that returns a std::pair<bool, T> and takes a value type of its corresponding
     * container as parameter.
     * @return A choose view object.
     */
    template<class Iterable, class Function>
    auto choose(Iterable&& iterable, const Function function) {
        return chooserange(std::begin(iterable), std::end(iterable), function);
    }
}