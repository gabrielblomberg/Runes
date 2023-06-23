#pragma once

#include <vector>
#include <list>
#include <functional>

namespace TypeList {

/**
 * @brief A list of types contained in a parameter pack.
 */
template<typename... Types>
struct TypeList;

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
template <typename Head, typename... Tail>
using PopFront = typename _PopFront<TypeList<Head, Tail...>>::type;

template <unsigned Index, typename List>
struct _Get;

template <unsigned Index, typename Head, typename... Tail>
struct _Get<Index, TypeList<Head, Tail...>> {
    using type = typename _Get<Index - 1, TypeList<Tail...>>::type;
};

template <typename Head, typename... Tail>
struct _Get<0, TypeList<Head, Tail...>> {
    using type = Head;
};

/**
 * @brief Get a type at an index to a type list.
 */
template<typename List, unsigned Index>
using Get = typename _Get<Index, List>::type;

template<typename Left, typename Right>
struct _Join;

template<
    typename LeftHead, typename RightHead,
    typename... LeftTail, typename... RightTail
>
struct _Join<
    TypeList<LeftHead, LeftTail...>,
    TypeList<RightHead, RightTail...>
> {
    using type = TypeList<LeftHead, LeftTail..., RightHead, RightTail...>;
};

/**
 * @brief Concatenate two type lists together.
 */
template<typename Left, typename Right>
using Join = _Join<Left, Right>::type;

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
using Tuple = _Tuple<List>::type;

template<typename List>
struct _Vector;

template<typename Head, typename... Tail>
struct _Vector<TypeList<Head, Tail...>> {
    using type = Join<
        TypeList<std::vector<Head>>,
        typename _Vector<TypeList<Tail...>>::type
    >;
};

template<typename Head>
struct _Vector<TypeList<Head>> {
    using type = TypeList<std::vector<Head>>;
};

/**
 * @brief Create a type list with std::vector<T> applied around each type T in
 * the list.
 */
template<typename List>
using Vector = _Vector<List>::type;

template<typename List>
struct _List;

template<typename Head, typename... Tail>
struct _List<TypeList<Head, Tail...>> {
    using type = Join<
        TypeList<std::list<Head>>,
        typename _List<TypeList<Tail...>>::type
    >;
};

template<typename Head>
struct _List<TypeList<Head>> {
    using type = TypeList<std::list<Head>>;
};

/**
 * @brief Create a type list with std::queue<T> applied around each type T in
 * the list.
 */
template<typename TList>
using List = _List<TList>::type;

template<typename List, typename Return = void, typename... Args>
struct _Function;

template<typename Return, typename Head, typename... Tail, typename... Args>
struct _Function<TypeList<Head, Tail...>, Return, Args...> {
    using type = Join<
        TypeList<std::function<Return(const Head&, Args...)>>,
        typename _Function<TypeList<Tail...>, Return, Args...>::type
    >;
};

template<typename Return, typename Head, typename... Args>
struct _Function<TypeList<Head>, Return, Args...> {
    using type = TypeList<std::function<Return(const Head&, Args...)>>;
};

/**
 * @brief Create a type list with std::function<Return(const T&, Args...)>
 * applied around each type T in the list.
 */
template<typename List, typename Return = void, typename... Args>
using Function = _Function<List, Return, Args...>::type;

} // namespace TypeList
