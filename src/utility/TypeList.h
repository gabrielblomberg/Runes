#pragma once

#include <cstdint>
#include <tuple>
#include <variant>

namespace TypeList {

/**
 * @brief A list of types contained in a parameter pack.
 */
template<typename... Types>
struct TypeList;

// Empty

template<typename List>
struct _IsEmpty {
    static const constexpr bool value = false;
};

template<>
struct _IsEmpty<TypeList<>> {
    static const constexpr bool value = true;
};

/**
 * @brief Check if a type list is empty.
 */
template<typename List>
using IsEmpty = _IsEmpty<List>::value;

template<typename>
struct _Size;

template<typename... Types>
struct _Size<TypeList<Types...>> {
    static const constexpr std::size_t value = sizeof...(Types);
};

/**
 * @brief Get the number of types contained in a type list.
 */
template<typename List>
inline constexpr std::size_t Size = _Size<List>::value;

// Front

template<typename List>
struct _Front;

template<typename Head, typename... Tail>
struct _Front<TypeList<Head, Tail...>> {
    using type = Head;
};

/**
 * @brief Get the first type in a type list.
 */
template<typename List>
using Front = typename _Front<List>::type;

// Push front

template<typename Type, typename List>
struct _PushFront;

template<typename Type, typename Head, typename... Tail>
struct _PushFront<Type, TypeList<Head, Tail...>> {
    using type = TypeList<Type, Head, Tail...>;
};

/**
 * @brief Add a type to the front of a type list.
 */
template<typename Type, typename Head, typename... Tail>
using PushFront = typename _PushFront<Type, TypeList<Head, Tail...>>::type;

// Pop front

template<typename List>
struct _PopFront;

template<typename Head, typename... Tail>
struct _PopFront<TypeList<Head, Tail...>> {
    using type = TypeList<Tail...>;
};

/**
 * @brief Remove a type to the front of a type list.
 */
template<typename List>
using PopFront = typename _PopFront<List>::type;

// Get

template<typename List, std::uint64_t I>
struct _Get;

template<typename List, std::uint64_t I>
struct _Get : public _Get<PopFront<List>, I - 1> {};

template<typename List>
struct _Get<List, 0> : public _Front<List> {};

/**
 * @brief Get a type from a type list from an index.
 */
template<typename List, std::uint64_t I>
using Get = _Get<List, I>::type;

// Index

template<typename List, typename Type, std::uint64_t I>
struct _Index {
    static_assert(I < Size<List> && "typelist index out of range");
    static const constexpr std::size_t value = I;
    using type = std::
        conditional_t<std::is_same_v<Front<List>, Type>, Type, _Index<PopFront<List>, Type, I + 1>>;
};

/**
 * @brief Get the type at an index.
 */
template<typename List, typename Type>
inline constexpr std::size_t Index = _Index<List, Type, 0>::value;

// Find

template<typename List, typename Type, std::uint64_t I>
struct _Find {
    static_assert(I < Size<List> && "typelist does not contain type");

    struct Value {
        static const constexpr std::size_t value = I;
    };

    using value = std::conditional_t<
        std::is_same_v<Front<List>, Type>,
        Value,
        _Find<PopFront<List>, Type, I + 1>>::value;
};

/**
 * @brief Find for the index of a type in a type list.
 * @returns The first index of that type.
 */
template<typename List, typename T>
using Find = _Find<List, T, 0>::value;

// Contains

template<typename List, typename T, std::int64_t I>
struct _Contains {
    static const constexpr bool value = std::conditional_t<
        std::is_same_v<Front<List>, T>,
        std::true_type,
        _Contains<PopFront<List>, T, I - 1>>::value;
};

template<typename List, typename T>
struct _Contains<List, T, -1> {
    static const constexpr bool value = false;
};

/**
 * @brief Contains for the index of a type in a type list.
 * @returns The first index of that type.
 */
template<typename List, typename T>
using Contains = _Contains<List, T, Size<List> - 1>::value;

// Concatenate

template<typename Left, typename Right>
struct _Concatenate;

template<typename LeftHead, typename RightHead, typename... LeftTail, typename... RightTail>
struct _Concatenate<TypeList<LeftHead, LeftTail...>, TypeList<RightHead, RightTail...>> {
    using type = TypeList<LeftHead, LeftTail..., RightHead, RightTail...>;
};

/**
 * @brief Concatenate two type lists together.
 */
template<typename Left, typename Right>
using Concatenate = _Concatenate<Left, Right>::type;

// Map a template function to types in a typelist.

template<template<typename T> class Function, typename Head, typename... Tail>
struct _Map {
    using type = Concatenate<
        TypeList<typename Function<Head>::type>,
        typename _Map<Function, Tail...>::type>;
};

template<template<typename T> class Function, typename Head>
struct _Map<Function, Head> {
    using type = TypeList<typename Function<Head>::type>;
};

/**
 * @brief Map types in a type list with a meta function.
 */
template<template<typename T> class Function, typename List>
using Map = _Map<Function, List>::type;

// Wrap

template<template<typename T> class Wrapper, typename List>
struct _Wrap;

template<template<typename T> class Wrapper, typename Head, typename... Tail>
struct _Wrap<Wrapper, TypeList<Head, Tail...>> {
    using type =
        Concatenate<TypeList<Wrapper<Head>>, typename _Wrap<Wrapper, TypeList<Tail...>>::type>;
};

template<template<typename T> class Wrapper, typename Head>
struct _Wrap<Wrapper, TypeList<Head>> {
    using type = TypeList<Wrapper<Head>>;
};

/**
 * @brief Wrap each type in the list with another type.
 *
 * @tparam Wrapper The template to wrap the types of the list in.
 * @tparam List The list to apply the template to.
 */
template<template<typename T> class Wrapper, typename List>
using Wrap = _Wrap<Wrapper, List>::type;

// Filter

template<template<typename T> class Conditional, typename Filtered, typename Head, typename... Tail>
struct _Filter {
    using type = _Filter<
        Conditional,
        std::conditional_t<
            Conditional<Head>::value,
            Concatenate<Filtered, TypeList<Head>>,
            Filtered>,
        Tail...>;
};

template<template<typename T> class Conditional, typename Filtered, typename Head>
struct _Filter<Conditional, Filtered, TypeList<Head>> {
    using type = std::
        conditional_t<Conditional<Head>::value, Concatenate<Filtered, TypeList<Head>>, Filtered>;
};

/**
 * @brief Filter types in a type list with a meta function.
 */
template<typename List, template<typename T> class Conditional>
using Filter = _Filter<Conditional, TypeList<>, List>::type;

// Tuple.

template<typename Head, typename... Tail>
struct _TupleOf;

template<typename Head, typename... Tail>
struct _TupleOf<TypeList<Head, Tail...>> {
    using type = std::tuple<Head, Tail...>;
};

/**
 * @brief Convert a type list to a tuple of those types.
 */
template<typename List>
using TupleOf = _TupleOf<List>::type;

// Variant.

template<typename Head, typename... Tail>
struct _Variant;

template<typename Head, typename... Tail>
struct _Variant<TypeList<Head, Tail...>> {
    using type = std::variant<Head, Tail...>;
};

/**
 * @brief Convert a type list to a tuple of those types.
 */
template<typename List>
using VariantOf = _Variant<List>::type;

}  // namespace TypeList
