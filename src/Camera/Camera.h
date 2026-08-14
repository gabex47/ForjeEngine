#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

namespace Forje::Input
{
class Input;
}

namespace Forje::Camera
{
class Camera final
{
public:
    Camera();

    void Update(const Input::Input& input, float deltaTime);

    Math::Matrix4 ViewMatrix() const noexcept;
    Math::Matrix4 ProjectionMatrix(float aspectRatio) const;
    const Math::Vector3& Position() const noexcept;
    const Math::Vector3& Forward() const noexcept;
    float MovementSpeed() const noexcept;

private:
    void UpdateBasis() noexcept;
    void AdjustMovementSpeed(const Input::Input& input);

    Math::Vector3 m_Position{8.0F, 6.0F, 12.0F};
    Math::Vector3 m_Forward{0.0F, 0.0F, -1.0F};
    Math::Vector3 m_Right{1.0F, 0.0F, 0.0F};
    Math::Vector3 m_Up{0.0F, 1.0F, 0.0F};
    Math::Vector3 m_SmoothedMouseDelta{};
    float m_YawRadians{-2.16F};
    float m_PitchRadians{-0.34F};
    float m_MovementSpeed{7.0F};
    float m_MouseSensitivity{0.0022F};
    float m_MouseSmoothing{22.0F};
    float m_VerticalFieldOfViewRadians{1.13446F};
    float m_NearPlane{0.05F};
    float m_FarPlane{500.0F};
};
} // namespace Forje::Camera
