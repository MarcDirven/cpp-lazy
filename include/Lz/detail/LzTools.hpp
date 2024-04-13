#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include <iterator>
#include <tuple>

#if defined(__has_include)
#define LZ_HAS_INCLUDE(FILE) __has_include(FILE)
#else
#define LZ_HAS_INCLUDE(FILE) 0
#endif // __has_include

#if defined(__has_cpp_attribute)
#define LZ_HAS_ATTRIBUTE(ATTR) __has_cpp_attribute(ATTR)
#else
#define LZ_HAS_ATTRIBUTE(ATTR) 0
#endif // __has_cpp_attribute

#if (defined(__GNUC__)) && !(defined(__clang__))
#define LZ_GCC_VERSION __GNUC__
#endif // GNU/clang

#if defined(_MSVC_LANG)
#define LZ_MSVC _MSVC_LANG
#endif // _MSVC_LANG

#if (defined(LZ_MSVC) && (LZ_MSVC >= 201103L) && (LZ_MSVC < 201402L)) || ((__cplusplus >= 201103L) && (__cplusplus < 201402L))
#define LZ_HAS_CXX_11
#endif // end has cxx 11

#if (__cplusplus >= 201300) || ((defined(LZ_MSVC)) && (LZ_MSVC >= 201300))
#define LZ_CONSTEXPR_CXX_14 constexpr
#else
#define LZ_CONSTEXPR_CXX_14 inline
#endif // has cxx 14

#if (__cplusplus >= 201703L) || ((defined(LZ_MSVC)) && (LZ_MSVC >= 201703L))
#define LZ_HAS_CXX_17
#define LZ_CONSTEXPR_CXX_17 constexpr
#else
#define LZ_CONSTEXPR_CXX_17 inline
#endif // Has cxx 17

#if (__cplusplus > 201703L) || ((defined(LZ_MSVC) && (LZ_MSVC > 201703L)))
#define LZ_HAS_CXX_20
#if defined(__cpp_constexpr_dynamic_alloc) && defined(__cpp_lib_constexpr_dynamic_alloc) &&                              \
            defined(__cpp_lib_constexpr_string) && defined(__cpp_lib_constexpr_vector) &&                                        \
            defined(__cpp_lib_constexpr_algorithms)
#define LZ_CONSTEXPR_CXX_20 constexpr
#else
#define LZ_CONSTEXPR_CXX_20 inline
#endif // cpp constexpr new/algo
#else
#define LZ_CONSTEXPR_CXX_20 inline
#endif // Has cxx 20

#if LZ_HAS_ATTRIBUTE(nodiscard) && defined(LZ_HAS_CXX_17)
#define LZ_NODISCARD [[nodiscard]]
#else
#define LZ_NODISCARD
#endif // LZ_HAS_ATTRIBUTE(nodiscard)

#ifdef __cpp_ref_qualifiers
#define LZ_HAS_REF_QUALIFIER
#define LZ_CONST_REF_QUALIFIER const&
#else
#define LZ_CONST_REF_QUALIFIER
#endif // __cpp_ref_qualifiers

#if LZ_HAS_INCLUDE(<execution>) && (defined(LZ_HAS_CXX_17) && (defined(__cpp_lib_execution)))
#define LZ_HAS_EXECUTION
#include <execution>
#endif // has execution

#if LZ_HAS_INCLUDE(<string_view>) && (defined(LZ_HAS_CXX_17) && (defined(__cpp_lib_string_view)))
#define LZ_HAS_STRING_VIEW
#endif // has string view

#if LZ_HAS_INCLUDE(<concepts>) && (defined(LZ_HAS_CXX_20)) && (defined(__cpp_lib_concepts))
#define LZ_HAS_CONCEPTS
#include <concepts>
#endif // Have concepts

#ifdef __cpp_if_constexpr
#define LZ_CONSTEXPR_IF constexpr
#else
#define LZ_CONSTEXPR_IF
#endif // __cpp_if_constexpr

#if defined(LZ_STANDALONE) && defined(__cpp_lib_to_chars) && LZ_HAS_INCLUDE(<charconv>)
#include <charconv>
#endif

#if !defined(LZ_STANDALONE) || !defined(LZ_MODULE_EXPORT)
#include <fmt/format.h>
#include <fmt/ranges.h>
#endif

#ifndef LZ_MODULE_EXPORT
#define LZ_MODULE_EXPORT_SCOPE_BEGIN
#define LZ_MODULE_EXPORT_SCOPE_END
#endif

#if defined(__cpp_lib_format) && (LZ_HAS_INCLUDE(<format>)) && defined(LZ_HAS_CXX_20)
#define LZ_HAS_FORMAT
#endif // format

#if LZ_HAS_ATTRIBUTE(no_unique_address)
#define LZ_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
#define LZ_NO_UNIQUE_ADDRESS
#endif // LZ_HAS_ATTRIBUTE(no_unique_address)

#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
#include <string>
#endif

#if !defined(LZ_HAS_STRING_VIEW) && defined(LZ_STANDALONE)
#include <ostream> // for operator << for lz::BasicStringView
#endif

#if defined(LZ_HAS_STRING_VIEW)
#include <string_view>
#endif

#ifdef LZ_HAS_CONCEPTS

    namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class I>
concept BasicIterable = requires(I i) {
                            { std::begin(i) } -> std::input_or_output_iterator;
                            { std::end(i) } -> std::input_or_output_iterator;
                        };

template<class I>
concept BidirectionalIterable = requires(I i) {
                                    { std::begin(i) } -> std::bidirectional_iterator;
                                    { std::end(i) } -> std::bidirectional_iterator;
                                };

template<class I>
concept Arithmetic = std::is_arithmetic_v<I>;

LZ_MODULE_EXPORT_SCOPE_END

} // End namespace lz

#define LZ_CONCEPT_ARITHMETIC lz::Arithmetic
#define LZ_CONCEPT_INTEGRAL std::integral
#define LZ_CONCEPT_INVOCABLE std::invocable
#define LZ_CONCEPT_ITERABLE lz::BasicIterable
#define LZ_CONCEPT_ITERATOR std::input_or_output_iterator
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR std::bidirectional_iterator
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE lz::BidirectionalIterable

#else // ^^^ has concepts !has concepts vvv
#define LZ_CONCEPT_ARITHMETIC class
#define LZ_CONCEPT_INTEGRAL class
#define LZ_CONCEPT_ITERATOR class
#define LZ_CONCEPT_INVOCABLE class
#define LZ_CONCEPT_ITERABLE class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE class
#endif // LZ_HAS_CONCEPTS

#ifndef NDEBUG
#include <exception>
#endif // NDEBUG

#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
#include <stacktrace>
#endif

namespace lz {
namespace internal {

template<class>
struct AlwaysFalse : std::false_type {};

[[noreturn]] inline void assertionFail(const char* file, const int line, const char* func, const char* message) {
#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
    auto st = std::stacktrace::current();
    auto str = std::to_string(st);
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\nStacktrace:\n%s\n", file, line, func,
                 message, str.c_str());
#else
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\n", file, line, func, message);
#endif
    std::terminate();
}

#define LZ_ASSERT(CONDITION, MSG) ((CONDITION) ? ((void)0) : (lz::internal::assertionFail(__FILE__, __LINE__, __func__, MSG)))

template<class Iterable>
constexpr auto begin(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).begin()) {
    return std::forward<Iterable>(c).begin();
}

template<class Iterable>
constexpr auto end(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).end()) {
    return std::forward<Iterable>(c).end();
}

template<class T, size_t N>
constexpr T* begin(T (&array)[N]) noexcept {
    return std::begin(array);
}

template<class T, size_t N>
constexpr T* end(T (&array)[N]) noexcept {
    return std::end(array);
}

#ifdef LZ_HAS_CXX_11

#define MAKE_OPERATOR(OP, VALUE_TYPE) OP<VALUE_TYPE>

template<std::size_t...>
struct IndexSequence {};

template<std::size_t N, std::size_t... Rest>
struct IndexSequenceHelper : public IndexSequenceHelper<N - 1, N - 1, Rest...> {};

template<std::size_t... Next>
struct IndexSequenceHelper<0, Next...> {
    using Type = IndexSequence<Next...>;
};

template<std::size_t N>
using MakeIndexSequence = typename IndexSequenceHelper<N>::Type;

template<class T>
using Decay = typename std::decay<T>::type;

template<std::size_t I, class T>
using TupleElement = typename std::tuple_element<I, T>::type;

#define MAKE_BIN_OP(OP, VALUE_TYPE) OP<VALUE_TYPE>
#else // ^^^ has cxx 11 vvv cxx > 11
template<std::size_t... N>
using IndexSequence = std::index_sequence<N...>;

template<std::size_t N>
using MakeIndexSequence = std::make_index_sequence<N>;

template<class T>
using Decay = std::decay_t<T>;

template<std::size_t I, class T>
using TupleElement = std::tuple_element_t<I, T>;

#define MAKE_BIN_OP(OP, VALUE_TYPE) OP<>

#endif // LZ_HAS_CXX_11

template<class Iterable>
using IterTypeFromIterable = decltype(begin(std::forward<Iterable>(std::declval<Iterable>())));

template<class Iterator>
using ValueType = typename std::iterator_traits<Iterator>::value_type;

template<class Iterator>
using RefType = typename std::iterator_traits<Iterator>::reference;

template<class Iterator>
using PointerType = typename std::iterator_traits<Iterator>::pointer;

template<class Iterator>
using DiffType = typename std::iterator_traits<Iterator>::difference_type;

template<class Iterator>
using IterCat = typename std::iterator_traits<Iterator>::iterator_category;

template<class Function, class... Args>
using FunctionReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));

template<class Iterable>
using ValueTypeIterable = typename std::iterator_traits<IterTypeFromIterable<Iterable>>::value_type;

template<class Iterable>
using DiffTypeIterable = typename std::iterator_traits<IterTypeFromIterable<Iterable>>::difference_type;

#ifdef LZ_HAS_EXECUTION
template<class T>
struct IsSequencedPolicy : std::is_same<T, std::execution::sequenced_policy> {};

template<class T>
struct IsForwardOrStronger : std::is_convertible<IterCat<T>, std::forward_iterator_tag> {};

template<class T>
constexpr bool IsSequencedPolicyV = IsSequencedPolicy<T>::value;

template<class T>
constexpr bool IsForwardOrStrongerV = IsForwardOrStronger<T>::value;

template<class Execution, class Iterator>
constexpr bool checkForwardAndPolicies() {
    static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::*...");
    constexpr bool isSequenced = IsSequencedPolicyV<Execution>;
    if constexpr (!isSequenced) {
        static_assert(IsForwardOrStrongerV<Iterator>,
                      "The iterator type must be forward iterator or stronger. Prefer using std::execution::seq");
    }
    return isSequenced;
}

#endif // LZ_HAS_EXECUTION

template<bool B>
struct EnableIfImpl {};

template<>
struct EnableIfImpl<true> {
    template<class T>
    using type = T;
};

template<bool B, class T = void>
using EnableIf = typename EnableIfImpl<B>::template type<T>;

template<bool B>
struct ConditionalImpl;

template<>
struct ConditionalImpl<true> {
    template<class IfTrue, class /* IfFalse */>
    using type = IfTrue;
};

template<>
struct ConditionalImpl<false> {
    template<class /* IfTrue */, class IfFalse>
    using type = IfFalse;
};

template<bool B, class IfTrue, class IfFalse>
using Conditional = typename ConditionalImpl<B>::template type<IfTrue, IfFalse>;

template<class T, class U, class... Vs>
struct IsAllSame : std::integral_constant<bool, std::is_same<T, U>::value && IsAllSame<U, Vs...>::value> {};

template<class T, class U>
struct IsAllSame<T, U> : std::is_same<T, U> {};

template<class IterTag>
struct IsBidirectionalTag : std::is_convertible<IterTag, std::bidirectional_iterator_tag> {};

template<class Iterator>
struct IsBidirectional : IsBidirectionalTag<IterCat<Iterator>> {};

template<class Iterator>
struct IsForward : std::is_convertible<IterCat<Iterator>, std::forward_iterator_tag> {};

template<class IterTag>
struct IsRandomAccessTag : std::is_convertible<IterTag, std::random_access_iterator_tag> {};

template<class Iterator>
struct IsRandomAccess : IsRandomAccessTag<IterCat<Iterator>> {};

template<class T>
class FakePointerProxy {
    T _t;

    using Pointer = decltype(std::addressof(_t));

public:
    constexpr explicit FakePointerProxy(const T& t) : _t(t) {
    }

    LZ_CONSTEXPR_CXX_17 Pointer operator->() const noexcept {
        return std::addressof(_t);
    }

    LZ_CONSTEXPR_CXX_17 Pointer operator->() noexcept {
        return std::addressof(_t);
    }
};

template<class>
class FunctionContainer;

template<class Fn, std::size_t... I>
struct TupleExpand {
    FunctionContainer<Fn> _fn{};

    constexpr TupleExpand() = default;

    explicit constexpr TupleExpand(Fn fn) : _fn(std::move(fn)) {
    }

    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto operator()(Tuple&& tuple) -> decltype(_fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }
};

template<class Fn, std::size_t... I>
constexpr TupleExpand<Fn, I...> makeExpandFn(Fn fn, IndexSequence<I...>) {
    return TupleExpand<Fn, I...>(std::move(fn));
}

template<LZ_CONCEPT_INTEGRAL Arithmetic>
LZ_CONSTEXPR_CXX_14 bool isEven(const Arithmetic value) noexcept {
    return (value % 2) == 0;
}

template<class... Ts>
LZ_CONSTEXPR_CXX_14 void decompose(const Ts&...) noexcept {
}

template<LZ_CONCEPT_INTEGRAL Arithmetic>
LZ_CONSTEXPR_CXX_14 Arithmetic roundEven(const Arithmetic a, const Arithmetic b) noexcept {
    LZ_ASSERT(a != 0 && b != 0, "division by zero error");
    if (b == 1) {
        return a;
    }
    if (b == -1) {
        return -a;
    }
    if (isEven(a) && isEven(b)) {
        return static_cast<Arithmetic>(a / b);
    }
    return static_cast<Arithmetic>(a / b) + 1;
}

template<class Iter>
DiffType<Iter> sizeHint(Iter first, Iter last) {
    if LZ_CONSTEXPR_IF (IsRandomAccess<Iter>::value) {
        return std::distance(std::move(first), std::move(last));
    }
    else {
        return 0;
    }
}

#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
void toStringFromBuff(const char value, char buff[std::numeric_limits<char>::digits10 + 3]) {
    buff[0] = value;
    std::fill(buff + 1, buff + std::numeric_limits<char>::digits10 + 3, '\0');
}

void toStringFromBuff(const bool value, char buff[6]) {
    std::snprintf(buff, 6, "%s", value ? "true" : "false");
}

template<class T>
void toStringFromBuff(const T value, char buff[std::numeric_limits<T>::digits10 + 3]) {
    constexpr static auto size = std::numeric_limits<T>::digits10 + 3;
#ifdef __cpp_lib_to_chars
    std::to_chars(std::begin(buff), std::end(buff), value);
#else
    if LZ_CONSTEXPR_IF (std::is_integral<T>::value) {
        if LZ_CONSTEXPR_IF (std::is_signed<T>::value) {
            std::snprintf(buff, size, "%lld", static_cast<long long>(value));
        }
        else {
            std::snprintf(buff, size, "%llu", static_cast<unsigned long long>(value));
        }
        return;
    }
    if LZ_CONSTEXPR_IF (std::is_floating_point<T>::value) {
        std::snprintf(buff, size, "%Lf", static_cast<long double>(value));
        return;
    }
#endif // __cpp_lib_to_chars
}
#endif // if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))

} // namespace internal

LZ_MODULE_EXPORT_SCOPE_BEGIN

#if defined(LZ_HAS_STRING_VIEW)
template<class C>
using BasicStringView = std::basic_string_view<C>;

using StringView = std::string_view;
#elif defined(LZ_STANDALONE)
template<class CharT>
class BasicStringView {
public:
    BasicStringView() = default;

    constexpr BasicStringView(const CharT* data, std::size_t size) noexcept : _data(data), _size(size) {
    }

    LZ_CONSTEXPR_CXX_17 BasicStringView(const CharT* data) noexcept : _data(data), _size(std::char_traits<CharT>::length(data)) {
    }

    constexpr BasicStringView(const CharT* begin, const CharT* end) noexcept :
        _data(begin),
        _size(static_cast<std::size_t>(end - begin)) {
    }

    constexpr const CharT* data() const noexcept {
        return _data;
    }

    constexpr std::size_t size() const noexcept {
        return _size;
    }

    constexpr const CharT* begin() const noexcept {
        return _data;
    }

    constexpr const CharT* end() const noexcept {
        return _data + _size;
    }

    constexpr std::size_t length() const noexcept {
        return _size;
    }

private:
    const CharT* _data{ nullptr };
    std::size_t _size{};
};

using StringView = BasicStringView<char>;
#else

template<class C>
using BasicStringView = fmt::basic_string_view<C>;

using StringView = fmt::string_view;
#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#if !defined(LZ_HAS_STRING_VIEW) && defined(LZ_STANDALONE)
template<typename CharT>
std::ostream& operator<<(std::ostream& os, const lz::BasicStringView<CharT>& view) {
    return os.write(view.data(), view.size());
}
#endif

#endif // LZ_LZ_TOOLS_HPP