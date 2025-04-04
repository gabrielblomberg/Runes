#pragma once

#include <cmath>
#include <type_traits>

#include <SFML/System/Vector2.hpp>

template<typename T>
class Vector2
{
public:

    inline Vector2()
        : x(T{})
        , y(T{})
    {}

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

    template<typename U>
    Vector2<std::common_type_t<T, U>> operator+(const Vector2<U> &other) {
        return {x + other.x, y + other.y};
    }

    template<typename U>
    Vector2<std::common_type_t<T, U>> operator-(const Vector2<U> &other) {
        return {x - other.x, y - other.y};
    }

    template<typename U>
    Vector2<std::common_type_t<T, U>> operator*(const Vector2<U> &other) {
        return {x * other.x, y * other.y};
    }

    template<typename U>
    Vector2<std::common_type_t<T, U>> operator*(U factor) {
        return {x * factor, y * factor};
    }

    template<typename U>
    Vector2<std::common_type_t<T, U>> operator/(const Vector2<U> &other) {
        return {x / other.x, y / other.y};
    }

    template<typename U>
    Vector2<std::common_type_t<T, U>> operator/(U factor) {
        return {x / factor, y / factor};
    }

    template<typename U>
    operator sf::Vector2<U>() {
        return sf::Vector2<U>(x, y);
    }

    T x, y;
};

template<typename T>
std::ostream &operator<<(std::ostream &out, Vector2<T> vector2)
{
    return out << "Vector2(" << vector2.x << ", " << vector2.y << ")";
}

using Vector2d = Vector2<double>;
using Vector2i = Vector2<int>;
