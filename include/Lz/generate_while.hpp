#pragma once

#ifndef LZ_GENERATE_WHILE_HPP
#define LZ_GENERATE_WHILE_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/generate_while.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class GeneratorFunc, class... Args>
class generate_while_iterable final
    : public detail::basic_iterable<detail::GenerateWhileIterator<GeneratorFunc, Args...>, default_sentinel> {
public:
    using iterator = detail::GenerateWhileIterator<GeneratorFunc, Args...>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr generate_while_iterable(GeneratorFunc func, std::tuple<Args...> tuple) :
        detail::BasicIterable<iterator, default_sentinel>(iterator(std::move(func), std::move(tuple))) {
    }

    constexpr generate_while_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a view to a generate iterator.
 * @details This object can be used to generate any amount of values while it returns `true` (or something that is convertible to
 * `true`), generated by the `generatorFunc` function. The generator function must return a std::pair like object where
 * `object::first` must return a bool like object (while it returns `true`/something convertible to `true`, it gets executed, if
 * `false`/something that is convertible to `false`, it doesn't). The type that is returned by `object::second` can be any type.
 * Example:
 * ```cpp
 * auto vector = lz::generateWhile([](int& i) { return std::make_pair(i == 4, i++); }, 0).to_vector();
 * // vector yields: { 0, 1, 2, 3 }
 * ```
 * @param generatorFunc The function to execute any amount of times as long as it returns a pair of `{true/boolean like value,
 * <someValue>}`.
 * @param args Args to pass (they are copied) to the function @p generatorFunc. Arguments of the @p generatorFunc can be accessed
 * by (const) reference as seen in the example above
 * @return A generator iterator view object.
 */
template<class GeneratorFunc, class... Args>
LZ_NODISCARD constexpr generate_while_iterable<detail::decay<GeneratorFunc>, detail::decay<Args>...>
generateWhile(GeneratorFunc&& generatorFunc, Args&&... args) {
    using Pair = decltype(generatorFunc(args...));
    using PairFirst = decltype(std::get<0>(std::declval<Pair>()));

    static_assert(std::is_convertible<detail::Decay<PairFirst>, bool>::value,
                  "Function must return a std::pair compatible object (i.e. object::first, object::second), where object::first "
                  "returns a bool like object.");
    return { std::forward<GeneratorFunc>(generatorFunc), std::make_tuple(std::forward<Args>(args)...) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif