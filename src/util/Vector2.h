#pragma once

#include <cmath>
#include <type_traits>

template<typename T>
class Vector2
{
public:

    inline Vector2(T x, T y)
        : x(x)
        , y(y)
    {}

    /**
     * @brief Calculate the length of the vector.
     * @return The length of the vector.
     */
    inline double norm() {
        return std::sqrt(x * x + y * y);
    }

    T x, y;
};

template<typename T, typename U>
Vector2<std::common_type_t<T, U>> operator+(
    const Vector2<T> &first,
    const Vector2<U> &second
) {
    return {first.x + second.x, first.y + second.y};
}

template<typename T, typename U>
Vector2<std::common_type_t<T, U>> operator-(
    const Vector2<T> &first,
    const Vector2<U> &second
) {
    return {first.x - second.x, first.y - second.y};
}

template<typename T, typename U>
Vector2<std::common_type_t<T, U>> operator*(
    const Vector2<T> &first,
    const Vector2<U> &second
) {
    return {first.x * second.x, first.y * second.y};
}

template<typename T, typename U>
Vector2<std::common_type_t<T, U>> operator*(const Vector2<T> &first, U second) {
    return {first.x * second, first.y * second};
}

template<typename T, typename U>
Vector2<std::common_type_t<T, U>> operator/(
    const Vector2<T> &first,
    const Vector2<U> &second
) {
    return {first.x / second.x, first.y / second.y};
}

template<typename T, typename U>
Vector2<std::common_type_t<T, U>> operator/(const Vector2<T> &first, U second)
{
    return {first.x / second, first.y / second};
}

template<typename T>
std::ostream &operator<<(std::ostream &out, Vector2<T> vector2)
{
    return out << "Vector2(" << vector2.x << ", " << vector2.y << ")";
}

using Vector2d = Vector2<double>;
using Vector2i = Vector2<int>;
