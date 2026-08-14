#include "Camera/Camera.h"

#include "Input/Input.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr Forje::Math::Vector3 WorldUp{0.0F, 1.0F, 0.0F};
}

namespace Forje::Camera
{
Camera::Camera()
{
    UpdateBasis();
}

void Camera::Update(const Input::Input& input, const float deltaTime)
{
    if (!std::isfinite(deltaTime) || deltaTime <= 0.0F)
    {
        return;
    }

    AdjustMovementSpeed(input);

    const Input::MouseDelta mouse = input.GetMouseDelta();
    const float smoothingFactor = 1.0F - std::exp(-m_MouseSmoothing * deltaTime);
    m_SmoothedMouseDelta.x += (mouse.x - m_SmoothedMouseDelta.x) * smoothingFactor;
    m_SmoothedMouseDelta.y += (mouse.y - m_SmoothedMouseDelta.y) * smoothingFactor;

    m_YawRadians += m_SmoothedMouseDelta.x * m_MouseSensitivity;
    m_PitchRadians += m_SmoothedMouseDelta.y * m_MouseSensitivity;
    constexpr float MaximumPitch = 1.55334F;
    m_PitchRadians = std::clamp(m_PitchRadians, -MaximumPitch, MaximumPitch);
    UpdateBasis();

    Math::Vector3 movement;
    if (input.IsKeyDown(Input::Key::W))
    {
        movement += m_Forward;
    }
    if (input.IsKeyDown(Input::Key::S))
    {
        movement -= m_Forward;
    }
    if (input.IsKeyDown(Input::Key::D))
    {
        movement += m_Right;
    }
    if (input.IsKeyDown(Input::Key::A))
    {
        movement -= m_Right;
    }
    if (input.IsKeyDown(Input::Key::E) || input.IsKeyDown(Input::Key::Space))
    {
        movement += WorldUp;
    }
    if (input.IsKeyDown(Input::Key::Q) || input.IsKeyDown(Input::Key::LeftControl))
    {
        movement -= WorldUp;
    }

    if (Math::LengthSquared(movement) > 0.0F)
    {
        const float sprintMultiplier = input.IsKeyDown(Input::Key::LeftShift) ? 3.0F : 1.0F;
        m_Position += Math::Normalize(movement) * m_MovementSpeed * sprintMultiplier * deltaTime;
    }
}

Math::Matrix4 Camera::ViewMatrix() const noexcept
{
    return Math::Matrix4::LookAt(m_Position, m_Position + m_Forward, m_Up);
}

Math::Matrix4 Camera::ProjectionMatrix(const float aspectRatio) const
{
    return Math::Matrix4::Perspective(
        m_VerticalFieldOfViewRadians,
        aspectRatio,
        m_NearPlane,
        m_FarPlane);
}

const Math::Vector3& Camera::Position() const noexcept
{
    return m_Position;
}

const Math::Vector3& Camera::Forward() const noexcept
{
    return m_Forward;
}

float Camera::MovementSpeed() const noexcept
{
    return m_MovementSpeed;
}

void Camera::UpdateBasis() noexcept
{
    m_Forward = Math::Normalize({
        std::cos(m_YawRadians) * std::cos(m_PitchRadians),
        std::sin(m_PitchRadians),
        std::sin(m_YawRadians) * std::cos(m_PitchRadians),
    });
    m_Right = Math::Normalize(Math::Cross(m_Forward, WorldUp));
    m_Up = Math::Normalize(Math::Cross(m_Right, m_Forward));
}

void Camera::AdjustMovementSpeed(const Input::Input& input)
{
    float speedScale = std::pow(1.18F, input.GetScrollDelta());
    if (input.WasKeyPressed(Input::Key::IncreaseSpeed))
    {
        speedScale *= 1.25F;
    }
    if (input.WasKeyPressed(Input::Key::DecreaseSpeed))
    {
        speedScale /= 1.25F;
    }
    m_MovementSpeed = std::clamp(m_MovementSpeed * speedScale, 0.5F, 80.0F);
}
} // namespace Forje::Camera
