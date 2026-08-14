#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <algorithm>

namespace Forje::Scene
{
struct Transform
{
    Math::Vector3 position{};
    Math::Vector3 rotationRadians{};
    Math::Vector3 scale{1.0F, 1.0F, 1.0F};

    Math::Matrix4 ModelMatrix() const noexcept
    {
        return Math::Matrix4::Translation(position)
            * Math::Matrix4::RotationZ(rotationRadians.z)
            * Math::Matrix4::RotationY(rotationRadians.y)
            * Math::Matrix4::RotationX(rotationRadians.x)
            * Math::Matrix4::Scale(scale);
    }
};

struct Renderable
{
    Math::Vector3 color{0.2F, 0.55F, 1.0F};
};

struct BoxCollider
{
    Math::Vector3 halfExtents{0.5F, 0.5F, 0.5F};
};

enum class BodyType
{
    Static,
    Dynamic,
};

struct RigidBody
{
    BodyType type{BodyType::Dynamic};
    Math::Vector3 linearVelocity{};
    Math::Vector3 accumulatedForce{};
    float inverseMass{1.0F};
    float restitution{0.05F};
    float friction{0.65F};
    float linearDamping{0.08F};
    bool useGravity{true};

    bool IsDynamic() const noexcept
    {
        return type == BodyType::Dynamic && inverseMass > 0.0F;
    }

    void SetMass(const float mass) noexcept
    {
        if (type == BodyType::Static || mass <= 0.0F)
        {
            inverseMass = 0.0F;
            type = BodyType::Static;
            return;
        }

        inverseMass = 1.0F / std::max(mass, 1.0e-4F);
    }

    void AddForce(const Math::Vector3& force) noexcept
    {
        if (IsDynamic())
        {
            accumulatedForce += force;
        }
    }
};
} // namespace Forje::Scene
