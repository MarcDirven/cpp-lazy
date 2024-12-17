#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#define LZ_BASIC_ITERATOR_VIEW_HPP

#include "Lz/algorithm.hpp"
#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/concepts.hpp"
#include "Lz/detail/procs.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/string_view.hpp"

#include <algorithm>
#include <array>
#include <map>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(LZ_STANDALONE)
#ifdef LZ_HAS_FORMAT
#include <format>
#else
#include <sstream>
#endif // LZ_HAS_FORMAT
#else
#include <fmt/ostream.h>
#endif

namespace lz {
namespace detail {

#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
template<class Iterable, class OutputIterator>
void stringify(const Iterable& iterable, OutputIterator out, std::string delimiter, std::string fmt) {
#else
template<class Iterable>
void stringify(const Iterable& iterable, std::ostream& out, std::string delimiter) {
#endif // has format

    const auto end = std::end(iterable);
    auto it = std::begin(iterable);
    if (it == end) {
        return;
    }

#ifndef LZ_STANDALONE

#if FMT_VERSION >= 90000
    fmt::format_to(out, fmt::runtime(fmt.c_str()), *it);
#else
    fmt::format_to(out, fmt.c_str(), *it);
#endif // FMT_VERSION >= 90000

    ++it;
    delimiter += fmt;
    // We do this purely to have the name appear as `fmt` in the lambda, instead of `delimiter`
    fmt = std::move(delimiter);

#if FMT_VERSION >= 90000
    detail::for_each(std::move(it), std::move(end),
                     [&out, &fmt](const auto& v) { fmt::format_to(out, fmt::runtime(fmt.c_str()), v); });
#else
    detail::for_each(std::move(it), std::move(end), [&out, &fmt](const auto& v) { fmt::format_to(out, fmt.c_str(), v); });
#endif // FMT_VERSION >= 90000

#elif defined(LZ_HAS_FORMAT) && defined(LZ_STANDALONE)
    std::format_to(out, fmt.c_str(), *it);
    ++it;
    delimiter.append(fmt.data(), fmt.size());
    fmt = std::move(delimiter);
    detail::for_each(std::move(it), std::move(end), [&out, &fmt](const auto& v) { std::format_to(out, fmt.c_str(), v); });
#else
// clang-format off
    out << *it;
    ++it;
    detail::for_each(std::move(it), std::move(end), [&out, &delimiter](const auto& v) {
        out << delimiter;
        out << v;
    });
// clang-format on
#endif
}

template<class T>
struct is_array : std::is_array<T> {};

template<class T, std::size_t N>
struct is_array<std::array<T, N>> : std::true_type {};

template<class T, class = int>
struct has_reserve : std::false_type {};

template<class T>
struct has_reserve<T, decltype((void)std::declval<T&>().reserve(1), 0)> : std::true_type {};

template<class T, class = int>
struct has_resize : std::false_type {};

template<class T>
struct has_resize<T, decltype((void)std::declval<T&>().resize(1), 0)> : std::true_type {};

// TODO: make a new base class that contains a .size(), make inserter_for return begin() if it contains trivial objects and is
// random access

template<class It, class S = It>
class basic_iterable {
protected:
    It _begin{};
    S _end{};

public:
    using value_type = val_t<It>;
    using iterator = It;
    using reference = ref_t<It>;
    using const_reference = typename std::add_const<reference>::type;
    using const_iterator = iterator;

private:
    template<class PairFunc>
    using key_type = decay<decltype(std::get<0>(std::declval<func_ret_type<PairFunc, reference>>()))>;

    template<class PairFunc>
    using value_type_map = decay<decltype(std::get<1>(std::declval<func_ret_type<PairFunc, reference>>()))>;

#ifdef __cpp_if_constexpr
    template<class T>
    auto inserter_for(T&& container) const {
        using C = decay<T>;

        if constexpr (is_array<C>::value) {
            return std::begin(container);
        }
        else {
            return std::inserter(container, container.begin());
        }
    }
#else
    template<class T>
    auto inserter_for(T&& container)
        -> enable_if<!is_array<decay<T>>::value, decltype(std::inserter(container, container.begin()))> {
        return std::inserter(container, container.begin());
    }

    template<class T>
    auto inserter_for(T&& container) -> enable_if<is_array<decay<T>>::value, decltype(std::begin(container))> {
        return std::begin(container);
    }
#endif // __cpp_if_constexpr
public:
#ifdef __cpp_explicit_this_parameter
    template<class Self>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto&& begin(this Self&& self) {
        return std::forward<Self>(self)._begin;
    }

    template<class Self>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto&& end(this Self&& self) {
        return std::forward<Self>(self)._end;
    }
#else
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It begin() LZ_CONST_REF_QUALIFIER noexcept {
        return _begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 S end() LZ_CONST_REF_QUALIFIER noexcept {
        return _end;
    }
#endif

#if defined(LZ_HAS_REF_QUALIFIER) && !defined(__cpp_explicit_this_parameter)
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It begin() && noexcept {
        return std::move(_begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 S end() && noexcept {
        return std::move(_end);
    }
#endif // LZ_HAS_REF_QUALIFIER

    constexpr basic_iterable() = default;

    constexpr basic_iterable(It&& begin, S&& end) noexcept : _begin(std::move(begin)), _end(std::move(end)) {
    }

    constexpr basic_iterable(It&& begin, const S& end) noexcept : _begin(std::move(begin)), _end(end) {
    }

    constexpr basic_iterable(const It& begin, S&& end) noexcept : _begin(begin), _end(std::move(end)) {
    }

    constexpr basic_iterable(const It& begin, const S& end) noexcept : _begin(begin), _end(end) {
    }

    constexpr basic_iterable(const It& begin) noexcept : _begin(begin) {
    }

    constexpr basic_iterable(It&& begin) noexcept : _begin(std::move(begin)) {
    }

    /**
     * @brief This function can be used to create a new container from the current view. The template parameter `Container`
     * must be specified along with its value type: `view.to<std::vector<int>>()`. One could also use `view.to<std::vector>()`.
     * See the other `to` function overload for documentation.
     * @tparam Container The container along with its value type.
     * @example `iterable.to<std::vector<int>>(100); // This will create a vec of size 100 with containing the contents of
     * iterable`
     * @param args Additional container args. Must be compatible with the constructor of `Container`
     * @return The container.
     */
    template<class Container, class... Args>
    Container to(Args&&... args) const {
        Container cont(std::forward<Args>(args)...);
        copy_to(inserter_for(cont));
        return cont;
    }

    /**
     * Fills destination output iterator `output_iter` with current contents of [`begin()`, `end()`)
     * @param output_iter The output to fill into. Essentially the same as:
     * `lz::copy(iterable, container.begin());`
     */
    template<class OutputIterator>
    void copy_to(OutputIterator output_iter) const {
        lz::copy(*this, output_iter);
    }

    /**
     * Fills destination output iterator `output_iter` with current contents of [`begin()`, `end()`), using `transform_func`.
     * @param output_iter The output to fill into.
     * @param transform_func The transform function. Must be a callable object that has a parameter of the current value type.
     * Essentially the same as: `std::transform(iterable, container.begin(), [](T value) { ... });`
     */
    template<class OutputIterator, class TransformFunc>
    void transform_to(OutputIterator output_iter, TransformFunc&& transform_func) const {
        lz::transform(*this, output_iter, std::forward<TransformFunc>(transform_func));
    }

    /**
     * @brief Transforms the current iterator to a container of type `Container`. The container is filled with the result of the
     * function `f`. i.e. `view.transform_as<std::vector<int>>([](int i) { return i * 2; });` will return a vector with all
     * elements doubled.
     * @tparam Container The container to transform into
     * @param f The transform function
     * @param args args passed to the container
     * @return The container
     */
    template<class Container, class TransformFunc, class... Args>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Container transform_as(TransformFunc&& f, Args&&... args) const {
        Container cont(std::forward<Args>(args)...);
        transform_to(inserter_for(cont), std::forward<TransformFunc>(f));
        return cont;
    }

    /**
     * @brief Creates a new `std::vector<value_type, Allocator>`.
     * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
     * by this function.
     * @param alloc The allocator
     * @return A new `std::vector<value_type, Allocator>`.
     */
    template<class Allocator = std::allocator<value_type>>
    std::vector<value_type, Allocator> to_vector(const Allocator& alloc = Allocator()) const {
        return to<std::vector<value_type, Allocator>>(alloc);
    }

    /**
     * @brief Creates a new map from the current sequence. The map is filled with the result of the function `pair_func`.
     * @p pair_func must return a `std::pair` like object where the first element is the key and the second element is the value.
     *
     * @param pair_func The function to transform the sequence to a map.
     * @param allocator The allocator to use for the map.
     * @param cmp The compare function to use for the map.
     * @return A new `std::map` with the sequence.
     */
    template<class PairFunc, class BinaryPredicate = std::less<key_type<PairFunc>>,
             class Allocator = std::allocator<std::pair<const key_type<PairFunc>, value_type_map<PairFunc>>>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::map<key_type<PairFunc>, value_type_map<PairFunc>, BinaryPredicate, Allocator>
    to_map(PairFunc pair_func, const Allocator& allocator = {}, const BinaryPredicate& cmp = {}) const {
        using map = std::map<key_type<PairFunc>, value_type_map<PairFunc>, BinaryPredicate, Allocator>;
        return transform_as<map>(std::move(pair_func), cmp, allocator);
    }

    /**
     * @brief Creates a new unordered map from the current sequence. The map is filled with the result of the function
     * `pair_func`.
     * @p pair_func must return a `std::pair` like object where the first element is the key and the second element is the value.
     *
     * @param pair_func The function to transform the sequence to a unordered_map.
     * @param allocator The allocator to use for the unordered_map.
     * @param cmp The compare function to use for the unordered_map.
     * @return A new `std::unordered_map` with the sequence.
     */
    // clang-format off
    template<class PairFunc, class Hasher = std::hash<key_type<PairFunc>>, class KeyEquality = std::equal_to<key_type<PairFunc>>,
             class Allocator = std::allocator<std::pair<const key_type<PairFunc>, value_type_map<PairFunc>>>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
    std::unordered_map<key_type<PairFunc>, value_type_map<PairFunc>, Hasher, KeyEquality, Allocator>
    to_unordered_map(PairFunc pair_func, const Allocator& alloc = {}, const KeyEquality& cmp = {}, const Hasher& h = {}) const {
        using unordered_map = std::unordered_map<key_type<PairFunc>, value_type_map<PairFunc>, Hasher, KeyEquality, Allocator>;
        return transform_as<unordered_map>(
                std::move(pair_func), static_cast<std::size_t>(size_hint(_begin, _end)), h, cmp, alloc);
    }
    // clang-format on

    /**
     * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).to_string() yields 0123, while
     * lz::range(4).to_string(" ") yields 0 1 2 3 and lz::range(4).to_string(", ") yields 0, 1, 2, 3.
     * @param delimiter The delimiter between the previous value and the next.
     * @param fmt The format args. (`{}` is default, not applicable if std::format isn't available or LZ_STANDALONE is defined)
     * @return The converted iterator in string format.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::string
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    to_string(std::string delimiter = "", std::string fmt = "{}") const {
        std::string result;
        stringify(*this, std::back_inserter(result), std::move(delimiter), std::move(fmt));
        return result;
#else
    to_string(std::string delimiter = "") const {
        std::stringstream result;
        stringify(*this, result, std::move(delimiter));
        return result.str();
#endif
    }

    /**
     * Function to stream the iterator to an output stream e.g. `std::cout`.
     * @param o The stream object.
     * @param it The iterator to print.
     * @return The stream object by reference.
     */
    friend std::ostream& operator<<(std::ostream& o, const basic_iterable<It, S>& it) {
#if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
        stringify(it, std::ostream_iterator<value_type>(o), ", ", "{}");
#else
        stringify(it, o, ", ");
#endif
        return o;
    }

    /**
     * Returns the length of the view.
     * @note Please note that this traverses the whole sequence.
     * @return The length of the view.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 diff_type<It> distance() const {
        return lz::distance(_begin, _end);
    }

    /**
     * Gets the nth position of the iterator from this sequence.
     * @param n The offset.
     * @return The element referred to by `begin() + n`
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It next(const diff_type<It> n = 1) const {
        return std::next(_begin, n);
    }

    /**
     * Checks is the sequence is empty.
     * @return True if it is empty, false otherwise.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty() const {
        return lz::empty(*this);
    }

    //! See `lz::front(Iterable)` for details
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference front() const {
        return lz::front(*this);
    }

    /**
     * @brief See `lz::back` for details
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference back() const {
        return lz::back(*this);
    }
};
} // namespace detail
} // namespace lz

#if defined(LZ_HAS_FORMAT) && defined(LZ_STANDALONE)
template<class Iterable>
struct std::formatter<Iterable,
                      lz::detail::enable_if< // Enable if Iterable is base of BasicIterable
                          std::is_base_of<lz::detail::BasicIterable<lz::iter_t<Iterable>>, Iterable>::value, char>>
    : std::formatter<std::string> {
    using InnerIter = lz::detail::BasicIterable<lz::iter_t<Iterable>>;

    template<class FormatCtx>
    auto format(const InnerIter& it, FormatCtx& ctx) const -> decltype(ctx.out()) {
        return std::formatter<std::string>::format(it.to_string(), ctx);
    }
};
#elif !defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
template<class Iterable>
struct fmt::formatter<
    Iterable, lz::detail::enable_if<std::is_base_of<lz::detail::basic_iterable<lz::iter_t<Iterable>>, Iterable>::value, char>>
    : fmt::ostream_formatter {};
#endif

LZ_MODULE_EXPORT_SCOPE_END
// End of group
/**
 * @}
 */

#endif // LZ_BASIC_ITERATOR_VIEW_HPP