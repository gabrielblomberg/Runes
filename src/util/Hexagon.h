#pragma once

#include <iostream>
#include <array>
#include <cmath>
#include <functional>
#include <map>
#include <tuple>
#include <type_traits>

#define PI 3.141592653589793
#define SQRT3 1.7320508075688772

namespace Hexagon {

// Hexagon Type

/**
 * @brief A generic hexagon with cubic coordinate values (q, r, s) stored as
 * type T.
 */
template<typename T>
class Hexagon
{
public:

    constexpr Hexagon() = default;

    /**
     * @brief Construct a hexagon from axial coordinates.
     * 
     * Computes the third hexagonal axis coordinate s.
     * 
     * @param q The first coordinate.
     * @param r The second coordinate.
     */
    constexpr Hexagon(T q, T r) noexcept;

    /**
     * @brief Construct a hexagon from cube coordinates.
     * 
     * @param q The first hexagonal coordinate.
     * @param r The second hexagonal coordinate.
     * @param s The third hexagonal coordinate.
     */
    constexpr Hexagon(T q, T r, T s) noexcept;

    /**
     * @brief Computes the length of the line from the origin to the centre of
     * this hexagon.
     * 
     * @returns The length from the origin to this hexagon.
     */
    inline T length() const noexcept;

    /**
     * @brief Computes the distance to another hexagon.
     * 
     * @param hexagon The hexagon to compute the distance to.
     * @return The distance to the hexagon.
     */
    inline T distance(const Hexagon<T> &hexagon) const noexcept;

    /**
     * @brief Returns the unit hexagon in a given direction, incrementing
     * clockwise.
     * 
     * For flat orientation - south east: 0, north east: 1, north: 2,
     * north west: 3, south west: 4, south : 5.
     * 
     * For pointy orientation - east: 0, north east: 1, north  west: 2,
     * west: 3, south west: 4, south east : 5.
     * 
     * @param direction The direction to 
     * @return A unit hexagon in the provided direction.
     */
    static const Hexagon<T> &direction(int direction) noexcept;

    /**
     * @brief Get the neighbor of this hexagon in a direction.
     * 
     * For flat orientation - south east: 0, north east: 1, north: 2,
     * north west: 3, south west: 4, south : 5.
     * 
     * For pointy orientation - east: 0, north east: 1, north  west: 2,
     * west: 3, south west: 4, south east : 5.
     * 
     * @param direction 
     * @return The hexagon neighboring in the provided direction.
     */
    Hexagon<T> neighbor(int direction) const noexcept;

    /**
     * @brief Get all the neighbors of this hexagon.
     * @return This hexagons neighbors.
     */
    std::array<Hexagon<T>, 6> neighbors() const noexcept;

    /**
     * @brief Round a floating point hexagon to an integer hexagon.
     * @return A hexagon with discrete coordinates.
     */
    Hexagon<int> round() const noexcept;

    /**
     * @brief Implicitly convert hexagons of other types to double.
     * 
     * @param hexagon The hexagon to convert to Hexagon<double>
     * @return The hexagon with double components.
     */
    operator Hexagon<double>() {
        return Hexagon<double>(q, r, s);
    }

    /**
     * @brief Implicitly convert hexagons of other types to int.
     * 
     * Rounds towards the nearest integer hex.
     * 
     * @param hexagon The hexagon to convert to Hexagon<double>
     * @return The hexagon with double components.
     */
    operator Hexagon<int>() {
        return Hexagon<double>(q, r, s).round();
    }

    /// The first hexagonal axis. Points north for flat and north east
    /// for pointy.
    T q;

    /// The second hexagonal axis. Points south east for flat and east
    /// for pointy.
    T r;

    /// The third hexagonal axis. Points south west for both flat and
    /// pointy.
    T s;
};

template<typename T>
constexpr Hexagon<T>::Hexagon(T q, T r) noexcept
    : q(q)
    , r(r)
    , s(-q - r)
{}

template<typename T>
constexpr Hexagon<T>::Hexagon(T q, T r, T s) noexcept
    : q(q)
    , r(r)
    , s(s)
{}

/// The unit hexagons along each orthogonal cubic axis.
static const constexpr std::array<Hexagon<int>, 6> DIRECTIONS {{
    Hexagon<int>(1, 0, -1), Hexagon<int>(1, -1, 0), Hexagon<int>(0, -1, 1), 
    Hexagon<int>(-1, 0, 1), Hexagon<int>(-1, 1, 0), Hexagon<int>(0, 1, -1)
}};

template<typename T>
Hexagon<T> operator+(const Hexagon<T> &left, const Hexagon<T> &right) noexcept {
    return Hexagon(left.q + right.q, left.r + right.r, left.s + right.s);
}

template<typename T>
Hexagon<T> operator-(const Hexagon<T> &left, const Hexagon<T> &right) noexcept {
    return Hexagon(left.q - right.q, left.r - right.r, left.s - right.s);
}

template<typename T>
Hexagon<T> operator*(const Hexagon<T> &left, const Hexagon<T> &right) noexcept {
    return Hexagon(left.q * right.q, left.r * right.r, left.s * right.s);
}

template<typename T>
Hexagon<T> operator/(const Hexagon<T> &left, const Hexagon<T> &right) noexcept {
    return Hexagon(left.q / right.q, left.r / right.r, left.s / right.s);
}

/**
 * @brief Compares two hexagons based on the tuple comparison of (q, r).
 */
template<typename T>
bool operator<(const Hexagon<T> &left, const Hexagon<T> &right) noexcept {
    return std::make_tuple(left.q, right.r) < std::make_tuple(right.q, right.r);
}

template<typename T>
bool operator==(const Hexagon<T> &left, const Hexagon<T> &right) noexcept {
    return left.q == right.q && left.r == right.r && left.s == right.s;
}

template<typename T>
bool operator!=(const Hexagon<T> &left, const Hexagon<T> &right) noexcept {
    return !(left == right);
}

template<typename T>
inline T Hexagon<T>::length() const noexcept{
    return (std::abs(q) + std::abs(r) + std::abs(s)) / 2;
}

template<typename T>
inline T Hexagon<T>::distance(const Hexagon<T> &hexagon) const noexcept {
    return (*this - hexagon).length();
}

template<typename T>
inline const Hexagon<T> &Hexagon<T>::direction(int direction) noexcept {
    return Hexagon<T>::DIRECTIONS[direction];
}

template<typename T>
inline Hexagon<T> Hexagon<T>::neighbor(int direction) const noexcept {
    return *this + Hexagon<T>::DIRECTIONS[(6 + (direction % 6)) % 6];
}

template<typename T>
Hexagon<int> Hexagon<T>::round() const noexcept
{
    int round_q = std::round(q);
    int round_r = std::round(r);
    int round_s = std::round(s);

    double q_diff = std::abs(q - std::round(q));
    double r_diff = std::abs(r - std::round(r));
    double s_diff = std::abs(s - std::round(s));

    if (q_diff > r_diff && q_diff > s_diff)
        return Hexagon<int>(-round_r - round_s, round_r, round_s);

    if (r_diff > s_diff)
        return Hexagon<int>(round_q, -round_q - round_s, round_s);

    return Hexagon<int>(round_q, round_r, -round_q - round_r);
}

} // namespace Hexagon

template<typename T>
std::ostream &operator<<(std::ostream &out, Hexagon::Hexagon<T> hexagon)
{
    return out << "Hexagon(" << hexagon.q << ", " << hexagon.r << ")";
}

namespace std {
    template<typename T>
    struct hash<Hexagon::Hexagon<T>>
    {
        size_t operator()(const Hexagon::Hexagon<T>& hexagon) const noexcept
        {
            size_t hq = hash<T>{}(hexagon.q);
            size_t hr = hash<T>{}(hexagon.r);
            return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
        }
    };
}

namespace Hexagon {

// Hexagon Grids

/**
 * @brief 
 * 
 */
enum class GridType {
    POINTY,
    FLAT
};

/**
 * @brief A grid of discrete hexagons.
 */
template <GridType Type = GridType::FLAT>
class Grid
{
public:

    Grid() = default;

    Grid(double size_x, double size_y, double origin_x, double origin_y);

    /**
     * @brief Convert a hexagon position to a cartesian position.
     * 
     * @param hexagon The hexagon to convert to cartesian coordinates.
     * @return The cartesian (x, y) position at the centre of the hexagon.
     */
    inline std::tuple<double, double> to_pixel(const Hexagon<int> &hexagon) const;

    /**
     * @brief Convert a cartesian position to a hexagon.
     * 
     * @param x The x cordinate of the hexagon.
     * @param y The y coordinate of the hexagon.
     * 
     * @return The hexagon at the cartesian position.
     */
    inline Hexagon<double> to_hexagon(double x, double y) const;

    /**
     * @brief Return the corner offset from the centre of a hexagon to a given
     * corner.
     * 
     * @param corner The corner to get the offset of.
     * @return The offset of the corner.
     */
    inline std::tuple<double, double> corner_offset(int corner) const;

    /**
     * @brief Return the cartesian coordinates of all corners on a hexagon.
     * 
     * @param hexagon The hexagon to get the corners of.
     * @return The cartesian coordinates of each corner.
     */
    inline std::array<std::tuple<double, double>, 6> corners(
        const Hexagon<double> &hexagon
    ) const;

private:

     /// The orientation type of the grid.
    struct Orientation {};

    /// The (x, y) size of the hexagons.
    std::tuple<double, double> m_size;

    /// The (x, y) origin of the hexagonal grid.
    std::tuple<double, double> m_origin;
};

template<>
struct Grid<GridType::POINTY>::Orientation {
    static const constexpr double start = 0.5;
    static const constexpr double f0 = SQRT3;
    static const constexpr double f1 = SQRT3 / 2.0;
    static const constexpr double f2 = 0.0;
    static const constexpr double f3 = 3.0 / 2.0;
    static const constexpr double b0 = SQRT3 / 3.0;
    static const constexpr double b1 = -1.0 / 3.0;
    static const constexpr double b2 = 0.0;
    static const constexpr double b3 = 2.0 / 3.0;
};

template<>
struct Grid<GridType::FLAT>::Orientation {
    static const constexpr double start = 0.0;
    static const constexpr double f0 = 3.0 / 2.0;
    static const constexpr double f1 = 0.0;
    static const constexpr double f2 = SQRT3 / 2.0;
    static const constexpr double f3 = SQRT3;
    static const constexpr double b0 = 2.0 / 3.0;
    static const constexpr double b1 = 0.0;
    static const constexpr double b2 = -1.0 / 3.0;
    static const constexpr double b3 = SQRT3 / 3.0;
};

template<GridType Type>
Grid<Type>::Grid(double size_x, double size_y, double origin_x, double origin_y)
    : m_size(size_x, size_y)
    , m_origin(origin_x, origin_y)
{}

template<GridType Type>
std::tuple<double, double> Grid<Type>::to_pixel(
    const Hexagon<int> &hexagon
) const {
    double x = (
        Grid<Type>::Orientation::f0 * hexagon.q +
        Grid<Type>::Orientation::f1 * hexagon.r
    ) * std::get<0>(m_size);

    double y = (
        Grid<Type>::Orientation::f2 * hexagon.q +
        Grid<Type>::Orientation::f3 * hexagon.r
    ) * std::get<1>(m_size);

    return std::make_tuple(
        x + std::get<0>(m_origin),
        y + std::get<1>(m_origin)
    );
}

template<GridType Type>
Hexagon<double> Grid<Type>::to_hexagon(double x, double y) const
{
    double px = (x - std::get<0>(m_origin)) / std::get<0>(m_size);
    double py = (y - std::get<1>(m_origin)) / std::get<1>(m_size);

    double q = (
        Grid<Type>::Orientation::b0 * px +
        Grid<Type>::Orientation::b1 * py
    );

    double r = (
        Grid<Type>::Orientation::b2 * px +
        Grid<Type>::Orientation::b3 * py
    );

    return Hexagon<double>(q, r, -q - r);
}

template<GridType Type>
std::tuple<double, double> Grid<Type>::corner_offset(int corner) const
{
    double angle = 2.0 * PI * (Grid<Type>::Orientation::start + corner) / 6;

    return std::make_tuple(
        std::get<0>(m_size) * std::cos(angle),
        std::get<1>(m_size) * std::sin(angle)
    );
}

} // namespace Hexagon
