#pragma once

#include <numeric>
#include <iterator>
#include <algorithm>

#include "StringSplitter.hpp"
#include "Join.hpp"


namespace lz {
    namespace detail {
        template<class Iterator>
        using ValueType = typename std::iterator_traits<Iterator>::value_type;

        template<class Iterator>
        using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;

        template<class Iterable>
        using ValueTypeIterable = typename std::iterator_traits<decltype(std::begin(std::declval<Iterable>()))>::value_type;


        template<class Container>
        struct IterTupleCreator {

        };
    }

    /**
     * Gets the mean of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return The mean of the sequence.
     */
    template<class Iterator>
    double mean(const Iterator begin, const Iterator end) {
        const detail::DifferenceType<Iterator> distance = std::distance(begin, end);
        const detail::ValueType<Iterator> sum = std::accumulate(begin, end, detail::ValueType<Iterator>(0));
        return static_cast<double>(sum) / distance;
    }

    /**
     * Gets the mean of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @param container The container to calculate the mean of.
     * @return The mean of the container.
     */
    template<class Iterable>
    double mean(const Iterable& container) {
        return mean(std::begin(container), std::end(container));
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<class Iterator, class Compare>
    double median(const Iterator begin, const Iterator end, const Compare compare) {
        const detail::DifferenceType<Iterator> len = std::distance(begin, end);
        if (len == 0) {
            throw std::invalid_argument(fmt::format("line {}: file: {}: the length of the sequence cannot be 0",
                                                    __LINE__, __FILE__));
        }

        const detail::DifferenceType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);
        std::nth_element(begin, midIter, end, compare);

        if (detail::isEven(len)) {
            const Iterator leftHalf = std::max_element(begin, midIter);
            return (*leftHalf + *midIter) / 2.;
        }
        return *midIter;
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @param iterable The container/sequence by reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<class Iterable, class Compare>
    double median(Iterable& iterable, const Compare compare) {
        return median(std::begin(iterable), std::end(iterable), compare);
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @param iterable The container/sequence by r-value reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<class Iterable, class Compare>
    double median(Iterable&& iterable, const Compare compare) {
        return median(std::begin(iterable), std::end(iterable), compare);
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @return The median of the sequence.
     */
    template<class Iterator>
    double median(const Iterator begin, const Iterator end) {
        return median(begin, end, std::less<detail::ValueType<Iterator>>());
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The container/sequence by reference.
     * @return The median of the sequence.
     */
    template<class Iterable>
    double median(Iterable& iterable) {
        return median(std::begin(iterable), std::end(iterable), std::less<detail::ValueTypeIterable<Iterable>>());
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The container/sequence by r-value reference.
     * @return The median of the sequence.
     */
    template<class Iterable>
    double median(Iterable&& iterable) {
        return median(std::begin(iterable), std::end(iterable), std::less<detail::ValueTypeIterable<Iterable>>());
    }

    /**
     * Returns a StringSplitter iterator, that splits the string on `'\n'`.
     * @tparam SubString The string type that the `StringSplitter::value_type` must return. Must either be std::string or std::string_view.
     * @tparam String The string type. `std::string` is assumed but can be specified.
     * @param string The string to split on.
     * @return Returns a StringSplitter iterator, that splits the string on `'\n'`.
     */
    template<class SubString = std::string, class String = std::string>
    StringSplitter<SubString, String> lines(String&& string) {
        return split<SubString, String>(string, "\n");
    }

    /**
     * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
     * @tparam Strings Is automatically deduced, but must be a container of `std::string` or `std::string_view`
     * @param strings The container of `std::string` or `std::string_view`.
     * @return A Join iterator that joins the strings in the container on `'\n'`.
     */
    template<class Strings>
    auto unlines(Strings&& strings) -> Join<std::decay_t<decltype(std::begin(strings))>> {
        static_assert(std::is_same<std::string, typename std::decay_t<Strings>::value_type>::value
#ifndef CXX_LT_17
                      || std::is_same<std::string_view, typename std::decay_t<Strings>::value_type>::value
#endif
            , "the type of the container should be std::string or std::string_view");
        return join(strings, "\n");
    }

    /**
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transaccumulate(s.begin(), s.end(), 0, [](const std::string& rhs) {
     *      return rhs.size();
     * }, std::plus<size_t>()); // totalSize = 11
     * ```
     * @tparam Iterator Is automatically deduced.
     * @tparam Init Is automatically deduced.
     * @tparam SelectorFunc Is automatically deduced.
     * @tparam BinaryOp Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param init The starting value.
     * @param selectorFunc Function that specifies what to add to `init`.
     * @param binaryOp A binary operation for e.g. `std::plus<[TYPE]>()`.
     * @return The result of the transfold operation.
     */
    template<class Iterator, class Init, class SelectorFunc>
    Init transaccumulate(Iterator begin, const Iterator end, Init init, const SelectorFunc selectorFunc) {
        for (; begin != end; ++begin) {
            init = selectorFunc(init, *begin);
        }
        return init;
    }

    /**
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transaccumulate(s, 0, [](const std::string& rhs) {
     *      return rhs.size();
     * }); // totalSize = 11
     * ```
     * @tparam Iterable Is automatically deduced.
     * @tparam Init Is automatically deduced.
     * @tparam SelectorFunc Is automatically deduced.
     * @param it The container to iterate over.
     * @param init The starting value.
     * @param selectorFunc Function that specifies what to add to `init`.
     * @return The result of the transfold operation.
     */
    template<class Iterable, class Init, class SelectorFunc>
    Init transaccumulate(const Iterable& it, Init init, const SelectorFunc selectorFunc) {
        return transaccumulate(std::begin(it), std::end(it), std::move(init), selectorFunc);
    }
}