#pragma once

#include "Math/Vector3.h"

#include <array>
#include <cmath>
#include <stdexcept>

namespace Forje::Math
{
class Matrix4
{
public:
    Matrix4() = default;

    static Matrix4 Identity() noexcept
    {
        Matrix4 result;
        result(0, 0) = 1.0F;
        result(1, 1) = 1.0F;
        result(2, 2) = 1.0F;
        result(3, 3) = 1.0F;
        return result;
    }

    static Matrix4 Perspective(
        const float verticalFieldOfViewRadians,
        const float aspectRatio,
        const float nearPlane,
        const float farPlane)
    {
        if (aspectRatio <= 0.0F || nearPlane <= 0.0F || farPlane <= nearPlane)
        {
            throw std::invalid_argument("Invalid perspective projection parameters.");
        }

        const float focalLength = 1.0F / std::tan(verticalFieldOfViewRadians * 0.5F);
        Matrix4 result;
        result(0, 0) = focalLength / aspectRatio;
        result(1, 1) = focalLength;
        result(2, 2) = (farPlane + nearPlane) / (nearPlane - farPlane);
        result(2, 3) = (2.0F * farPlane * nearPlane) / (nearPlane - farPlane);
        result(3, 2) = -1.0F;
        return result;
    }

    static Matrix4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept
    {
        const Vector3 forward = Normalize(target - eye);
        const Vector3 side = Normalize(Cross(forward, up));
        const Vector3 correctedUp = Cross(side, forward);

        Matrix4 result = Identity();
        result(0, 0) = side.x;
        result(0, 1) = side.y;
        result(0, 2) = side.z;
        result(0, 3) = -Dot(side, eye);
        result(1, 0) = correctedUp.x;
        result(1, 1) = correctedUp.y;
        result(1, 2) = correctedUp.z;
        result(1, 3) = -Dot(correctedUp, eye);
        result(2, 0) = -forward.x;
        result(2, 1) = -forward.y;
        result(2, 2) = -forward.z;
        result(2, 3) = Dot(forward, eye);
        return result;
    }

    static Matrix4 Translation(const Vector3& translation) noexcept
    {
        Matrix4 result = Identity();
        result(0, 3) = translation.x;
        result(1, 3) = translation.y;
        result(2, 3) = translation.z;
        return result;
    }

    static Matrix4 Scale(const Vector3& scale) noexcept
    {
        Matrix4 result = Identity();
        result(0, 0) = scale.x;
        result(1, 1) = scale.y;
        result(2, 2) = scale.z;
        return result;
    }

    static Matrix4 RotationX(const float radians) noexcept
    {
        Matrix4 result = Identity();
        const float cosine = std::cos(radians);
        const float sine = std::sin(radians);
        result(1, 1) = cosine;
        result(1, 2) = -sine;
        result(2, 1) = sine;
        result(2, 2) = cosine;
        return result;
    }

    static Matrix4 RotationY(const float radians) noexcept
    {
        Matrix4 result = Identity();
        const float cosine = std::cos(radians);
        const float sine = std::sin(radians);
        result(0, 0) = cosine;
        result(0, 2) = sine;
        result(2, 0) = -sine;
        result(2, 2) = cosine;
        return result;
    }

    static Matrix4 RotationZ(const float radians) noexcept
    {
        Matrix4 result = Identity();
        const float cosine = std::cos(radians);
        const float sine = std::sin(radians);
        result(0, 0) = cosine;
        result(0, 1) = -sine;
        result(1, 0) = sine;
        result(1, 1) = cosine;
        return result;
    }

    const float* Data() const noexcept
    {
        return m_Elements.data();
    }

    float& operator()(const std::size_t row, const std::size_t column) noexcept
    {
        return m_Elements[column * 4 + row];
    }

    float operator()(const std::size_t row, const std::size_t column) const noexcept
    {
        return m_Elements[column * 4 + row];
    }

private:
    std::array<float, 16> m_Elements{};
};

inline Matrix4 operator*(const Matrix4& left, const Matrix4& right) noexcept
{
    Matrix4 result;
    for (std::size_t column = 0; column < 4; ++column)
    {
        for (std::size_t row = 0; row < 4; ++row)
        {
            for (std::size_t index = 0; index < 4; ++index)
            {
                result(row, column) += left(row, index) * right(index, column);
            }
        }
    }
    return result;
}
} // namespace Forje::Math
