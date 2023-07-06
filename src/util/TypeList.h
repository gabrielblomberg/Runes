#pragma once

#include <tuple>

namespace TypeList {

/**
 * @brief A list of types contained in a parameter pack.
 */
template<typename... Types>
struct TypeList;

// Empty

template<typename List>
struct IsEmpty {
    static const constexpr bool value = false;
};

template<>
struct IsEmpty<TypeList<>> {
    static const constexpr bool value = true;
};

// Size

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

// Front

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

// Push front.

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

// Pop front.

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

// Get a type using an index on a type list.

template <typename List, unsigned I>
struct _Get;

template <typename List, unsigned I>
struct _Get : public _Get<PopFront<List>, I - 1> {};

template <typename List>
struct _Get<List, 0> : public _Front<List> {};

/**
 * @brief Get a type from a type list from an index.
 */
template<typename List, unsigned I>
using Get = _Get<List, I>::type;

// Concatenate two type lists together.

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

// Transform the types in a typelist with a meta function.

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

} // namespace TypeList
