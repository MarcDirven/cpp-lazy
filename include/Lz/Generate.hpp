#pragma once

#ifndef LZ_GENERATE_HPP
#define LZ_GENERATE_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/generate.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class GeneratorFunc, class... Args>
class generate_iterable final
    : public detail::basic_iterable<detail::generate_iterator<GeneratorFunc, Args...>, default_sentinel> {
public:
    using iterator = detail::generate_iterator<GeneratorFunc, Args...>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr generate_iterable(GeneratorFunc func, const std::size_t amount, const bool is_inf_loop, std::tuple<Args...> tuple) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(amount, std::move(func), is_inf_loop, std::move(tuple))) {
    }

    constexpr generate_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a view to a generate iterator.
 * @details This object can be used to generate `amount` of values, generated by the `generatorFunc` function.
 * Example:
 * ```cpp
 * int a = 0;
 * size_t amount = 4;
 * auto vector = lz::generate([](int& i) { auto tmp{a++}; return tmp; }, amount, 0).to_vector();
 * // vector yields: { 0, 1, 2, 3 }
 * @param generatorFunc The function to execute `amount` of times. The return value of the function is the type
 * that is generated.
 * ```
 * @param amount The amount of times to execute `generatorFunc`.
 * @param args Args to pass to the function @p generatorFunc
 * @return A generator iterator view object.
 */
template<class GeneratorFunc, class... Args>
LZ_NODISCARD constexpr generate_iterable<detail::decay<GeneratorFunc>, detail::decay<Args>...>
generate(GeneratorFunc&& generatorFunc, const std::size_t amount = (std::numeric_limits<std::size_t>::max)(), Args&&... args) {
    return { std::forward<GeneratorFunc>(generatorFunc), amount, 
             amount == (std::numeric_limits<std::size_t>::max)(), std::make_tuple(std::forward<Args>(args)...) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif