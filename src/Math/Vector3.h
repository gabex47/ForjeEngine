#pragma once

#include <algorithm>
#include <cmath>

namespace Forje::Math
{
struct Vector3
{
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};

    constexpr Vector3() = default;
    constexpr Vector3(const float xValue, const float yValue, const float zValue)
        : x(xValue), y(yValue), z(zValue)
    {
    }

    constexpr Vector3 operator-() const noexcept
    {
        return {-x, -y, -z};
    }

    constexpr Vector3& operator+=(const Vector3& other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr Vector3& operator-=(const Vector3& other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr Vector3& operator*=(const float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vector3& operator/=(const float scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
};

constexpr Vector3 operator+(Vector3 left, const Vector3& right) noexcept
{
    return left += right;
}

constexpr Vector3 operator-(Vector3 left, const Vector3& right) noexcept
{
    return left -= right;
}

constexpr Vector3 operator*(Vector3 vector, const float scalar) noexcept
{
    return vector *= scalar;
}

constexpr Vector3 operator*(const float scalar, Vector3 vector) noexcept
{
    return vector *= scalar;
}

constexpr Vector3 operator/(Vector3 vector, const float scalar) noexcept
{
    return vector /= scalar;
}

constexpr Vector3 Multiply(const Vector3& left, const Vector3& right) noexcept
{
    return {left.x * right.x, left.y * right.y, left.z * right.z};
}

constexpr float Dot(const Vector3& left, const Vector3& right) noexcept
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

constexpr Vector3 Cross(const Vector3& left, const Vector3& right) noexcept
{
    return {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x,
    };
}

constexpr float LengthSquared(const Vector3& vector) noexcept
{
    return Dot(vector, vector);
}

inline float Length(const Vector3& vector) noexcept
{
    return std::sqrt(LengthSquared(vector));
}

inline Vector3 Normalize(const Vector3& vector) noexcept
{
    constexpr float MinimumLength = 1.0e-6F;
    const float length = Length(vector);
    return length > MinimumLength ? vector / length : Vector3{};
}

inline Vector3 Clamp(const Vector3& value, const Vector3& minimum, const Vector3& maximum) noexcept
{
    return {
        std::clamp(value.x, minimum.x, maximum.x),
        std::clamp(value.y, minimum.y, maximum.y),
        std::clamp(value.z, minimum.z, maximum.z),
    };
}
} // namespace Forje::Math
