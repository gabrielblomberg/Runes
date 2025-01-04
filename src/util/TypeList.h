#pragma once

#include <tuple>
#include <variant>
#include <cstdint>

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

template <typename>
struct _Size;

template <typename... Types>
struct _Size<TypeList<Types...>> {
    static const constexpr std::size_t value = sizeof...(Types);
};

/**
 * @brief Get the number of types contained in a type list.
 */
template <typename List>
inline constexpr std::size_t Size = _Size<List>::value;

template <typename List>
struct _Front;

template <typename Head, typename... Tail>
struct _Front<TypeList<Head, Tail...>> {
    using type = Head;
};

/**
 * @brief Get the first type in a type list.
 */
template <typename List>
using Front = typename _Front<List>::type;

template <typename Type, typename List>
struct _PushFront;

template <typename Type, typename Head, typename... Tail>
struct _PushFront<Type, TypeList<Head, Tail...>> {
    using type = TypeList<Type, Head, Tail...>;
};

/**
 * @brief Add a type to the front of a type list.
 */
template <typename Type, typename Head, typename... Tail>
using PushFront = typename _PushFront<Type, TypeList<Head, Tail...>>::type;

template <typename List>
struct _PopFront;

template <typename Head, typename... Tail>
struct _PopFront<TypeList<Head, Tail...>> {
    using type = TypeList<Tail...>;
};

/**
 * @brief Remove a type to the front of a type list.
 */
template <typename List>
using PopFront = typename _PopFront<List>::type;

template <typename List, std::uint64_t I>
struct _Get;

template <typename List, std::uint64_t I>
struct _Get : public _Get<PopFront<List>, I - 1> {};

template <typename List>
struct _Get<List, 0> : public _Front<List> {};

/**
 * @brief Get a type from a type list from an index.
 */
template<typename List, std::uint64_t I>
using Get = _Get<List, I>::type;

template <typename List, typename Type, std::uint64_t I>
struct _Index {
    static_assert(I < Size<List>);
    static const constexpr std::size_t value = I;
    using type = std::conditional_t<
        std::is_same_v<Front<List>, Type>,
        Type,
        _Index<PopFront<List>, Type, I + 1>
    >;
};

/**
 * @brief Get the type at an index.
 */
template <typename List, typename Type>
using Index = _Index<List, Type, 0>::value;

template<typename List, typename Type, std::uint64_t I>
struct _Find {

    struct Value {
        using value = I;
    };

    static_assert(I < Size<List> && "find failed");
    using value = std::conditional_t<
        std::is_same_v<Front<List>, Type>,
        Value,
        _Find<PopFront<List>, Type, I + 1>
    >::value;
};

/**
 * @brief Find for the index of a type in a type list.
 * @returns The first index of that type.
 */
template<typename List, typename T>
using Find = _Find<List, T, 0>::value;

template<typename Left, typename Right>
struct _Concatenate;

template<
    typename LeftHead, typename RightHead,
    typename... LeftTail, typename... RightTail
>
struct _Concatenate<
    TypeList<LeftHead, LeftTail...>,
    TypeList<RightHead, RightTail...>
> {
    using type = TypeList<LeftHead, LeftTail..., RightHead, RightTail...>;
};

/**
 * @brief Concatenate two type lists together.
 */
template<typename Left, typename Right>
using Concatenate = _Concatenate<Left, Right>::type;

template<template<typename T> class MetaFunction, typename Head, typename... Tail>
struct _Transform
{
    using type = Concatenate<
        TypeList<typename MetaFunction<Head>::type>,
        typename _Transform<MetaFunction, Tail...>::type
    >;
};

template<template<typename T> class MetaFunction, typename Head>
struct _Transform<MetaFunction, TypeList<Head>>
{
    using type = TypeList<typename MetaFunction<Head>::type>;
};

/**
 * @brief Transform types in a type list with a meta function.
 */
template<typename List, template<typename T> class MetaFunction>
using Transform = _Transform<MetaFunction, List>::type;

template<template<typename T> class Template, typename Head, typename... Tail>
struct _Apply
{
    using type = Concatenate<
        TypeList<Template<Head>>,
        typename _Apply<Template, Tail...>::type
    >;
};

template<template<typename T> class Template, typename Head>
struct _Apply<Template, TypeList<Head>>
{
    using type = Template<Head>;
};

/**
 * @brief Apply a template type to each type in the list, yielding the new list.
 */
template<template<typename T> class Template, typename List>
using Apply = _Apply<Template, List>::type;

// Tuple.

template<typename Head, typename... Tail>
struct _Tuple;

template<typename Head, typename... Tail>
struct _Tuple<TypeList<Head, Tail...>> {
    using type = std::tuple<Head, Tail...>;
};

/**
 * @brief Convert a type list to a tuple of those types.
 */
template<typename List>
using TupleOf = _Tuple<List>::type;

// Tuple.

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

} // namespace TypeList
