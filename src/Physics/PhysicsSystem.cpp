#include "Physics/PhysicsSystem.h"

#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace
{
using Forje::Math::Vector3;
using Forje::Scene::Entity;
using Forje::Scene::RigidBody;

struct AxisAlignedBox
{
    Vector3 minimum;
    Vector3 maximum;
};

struct BodyProxy
{
    Entity* entity{nullptr};
    AxisAlignedBox bounds;
};

Vector3 WorldHalfExtents(const Entity& entity)
{
    const Forje::Math::Matrix4 model = entity.transform.ModelMatrix();
    const Vector3 localHalfExtents = entity.collider->halfExtents;
    return {
        std::fabs(model(0, 0)) * localHalfExtents.x
            + std::fabs(model(0, 1)) * localHalfExtents.y
            + std::fabs(model(0, 2)) * localHalfExtents.z,
        std::fabs(model(1, 0)) * localHalfExtents.x
            + std::fabs(model(1, 1)) * localHalfExtents.y
            + std::fabs(model(1, 2)) * localHalfExtents.z,
        std::fabs(model(2, 0)) * localHalfExtents.x
            + std::fabs(model(2, 1)) * localHalfExtents.y
            + std::fabs(model(2, 2)) * localHalfExtents.z,
    };
}

AxisAlignedBox BoundsFor(const Entity& entity)
{
    const Vector3 halfExtents = WorldHalfExtents(entity);
    return {
        entity.transform.position - halfExtents,
        entity.transform.position + halfExtents,
    };
}

bool OverlapsOnYZ(const AxisAlignedBox& first, const AxisAlignedBox& second)
{
    return first.minimum.y < second.maximum.y && first.maximum.y > second.minimum.y
        && first.minimum.z < second.maximum.z && first.maximum.z > second.minimum.z;
}

bool CalculateContact(
    const Entity& first,
    const Entity& second,
    Vector3& normal,
    float& penetration)
{
    const Vector3 firstHalf = WorldHalfExtents(first);
    const Vector3 secondHalf = WorldHalfExtents(second);
    const Vector3 centerDelta = second.transform.position - first.transform.position;

    const float overlapX = firstHalf.x + secondHalf.x - std::fabs(centerDelta.x);
    const float overlapY = firstHalf.y + secondHalf.y - std::fabs(centerDelta.y);
    const float overlapZ = firstHalf.z + secondHalf.z - std::fabs(centerDelta.z);
    if (overlapX <= 0.0F || overlapY <= 0.0F || overlapZ <= 0.0F)
    {
        return false;
    }

    penetration = overlapX;
    normal = {centerDelta.x < 0.0F ? -1.0F : 1.0F, 0.0F, 0.0F};
    if (overlapY < penetration)
    {
        penetration = overlapY;
        normal = {0.0F, centerDelta.y < 0.0F ? -1.0F : 1.0F, 0.0F};
    }
    if (overlapZ < penetration)
    {
        penetration = overlapZ;
        normal = {0.0F, 0.0F, centerDelta.z < 0.0F ? -1.0F : 1.0F};
    }
    return true;
}

Vector3 ClampMagnitude(const Vector3& value, const float maximumLength)
{
    const float lengthSquared = Forje::Math::LengthSquared(value);
    const float maximumSquared = maximumLength * maximumLength;
    if (lengthSquared <= maximumSquared)
    {
        return value;
    }
    return Forje::Math::Normalize(value) * maximumLength;
}
} // namespace

namespace Forje::Physics
{
PhysicsSystem::PhysicsSystem(PhysicsSettings settings)
    : m_Settings(settings)
{
    if (m_Settings.fixedTimeStep <= 0.0F || m_Settings.maximumSubSteps <= 0
        || m_Settings.solverIterations <= 0 || m_Settings.maximumLinearSpeed <= 0.0F)
    {
        throw std::invalid_argument("Physics settings must contain positive step limits.");
    }
}

void PhysicsSystem::Update(Scene::Scene& scene, const float deltaTime)
{
    if (!std::isfinite(deltaTime) || deltaTime <= 0.0F)
    {
        return;
    }

    const float maximumAccumulatedTime =
        m_Settings.fixedTimeStep * static_cast<float>(m_Settings.maximumSubSteps);
    m_Accumulator = std::min(m_Accumulator + deltaTime, maximumAccumulatedTime);

    int completedSteps = 0;
    while (m_Accumulator >= m_Settings.fixedTimeStep
        && completedSteps < m_Settings.maximumSubSteps)
    {
        Step(scene, m_Settings.fixedTimeStep);
        m_Accumulator -= m_Settings.fixedTimeStep;
        ++completedSteps;
    }
}

const PhysicsSettings& PhysicsSystem::Settings() const noexcept
{
    return m_Settings;
}

float PhysicsSystem::InterpolationAlpha() const noexcept
{
    return m_Accumulator / m_Settings.fixedTimeStep;
}

void PhysicsSystem::Step(Scene::Scene& scene, const float deltaTime)
{
    for (Scene::Entity& entity : scene.Entities())
    {
        if (!entity.rigidBody || !entity.rigidBody->IsDynamic())
        {
            continue;
        }

        Scene::RigidBody& body = *entity.rigidBody;
        Vector3 acceleration = body.accumulatedForce * body.inverseMass;
        if (body.useGravity)
        {
            acceleration += m_Settings.gravity;
        }

        body.linearVelocity += acceleration * deltaTime;
        const float damping = std::exp(-std::max(0.0F, body.linearDamping) * deltaTime);
        body.linearVelocity *= damping;
        body.linearVelocity = ClampMagnitude(body.linearVelocity, m_Settings.maximumLinearSpeed);
        entity.transform.position += body.linearVelocity * deltaTime;
        body.accumulatedForce = {};
    }

    const std::vector<CollisionPair> pairs = BuildCollisionPairs(scene);
    for (int iteration = 0; iteration < m_Settings.solverIterations; ++iteration)
    {
        for (const CollisionPair& pair : pairs)
        {
            ResolveCollision(*pair.first, *pair.second);
        }
    }
}

std::vector<PhysicsSystem::CollisionPair> PhysicsSystem::BuildCollisionPairs(Scene::Scene& scene) const
{
    std::vector<BodyProxy> proxies;
    proxies.reserve(scene.EntityCount());

    for (Scene::Entity& entity : scene.Entities())
    {
        if (entity.collider && entity.rigidBody)
        {
            proxies.push_back({&entity, BoundsFor(entity)});
        }
    }

    std::sort(proxies.begin(), proxies.end(), [](const BodyProxy& left, const BodyProxy& right) {
        if (left.bounds.minimum.x == right.bounds.minimum.x)
        {
            return left.entity->id < right.entity->id;
        }
        return left.bounds.minimum.x < right.bounds.minimum.x;
    });

    std::vector<CollisionPair> pairs;
    for (std::size_t firstIndex = 0; firstIndex < proxies.size(); ++firstIndex)
    {
        const BodyProxy& first = proxies[firstIndex];
        for (std::size_t secondIndex = firstIndex + 1; secondIndex < proxies.size(); ++secondIndex)
        {
            const BodyProxy& second = proxies[secondIndex];
            if (second.bounds.minimum.x >= first.bounds.maximum.x)
            {
                break;
            }
            if (!first.entity->rigidBody->IsDynamic() && !second.entity->rigidBody->IsDynamic())
            {
                continue;
            }
            if (OverlapsOnYZ(first.bounds, second.bounds))
            {
                pairs.push_back({first.entity, second.entity});
            }
        }
    }
    return pairs;
}

void PhysicsSystem::ResolveCollision(Scene::Entity& first, Scene::Entity& second) const
{
    Vector3 normal;
    float penetration = 0.0F;
    if (!CalculateContact(first, second, normal, penetration))
    {
        return;
    }

    RigidBody& firstBody = *first.rigidBody;
    RigidBody& secondBody = *second.rigidBody;
    const float firstInverseMass = firstBody.IsDynamic() ? firstBody.inverseMass : 0.0F;
    const float secondInverseMass = secondBody.IsDynamic() ? secondBody.inverseMass : 0.0F;
    const float inverseMassSum = firstInverseMass + secondInverseMass;
    if (inverseMassSum <= 0.0F)
    {
        return;
    }

    constexpr float PenetrationSlop = 0.0005F;
    constexpr float CorrectionPercent = 0.65F;
    const float correctionMagnitude =
        std::max(penetration - PenetrationSlop, 0.0F) * CorrectionPercent / inverseMassSum;
    const Vector3 correction = normal * correctionMagnitude;
    first.transform.position -= correction * firstInverseMass;
    second.transform.position += correction * secondInverseMass;

    Vector3 relativeVelocity = secondBody.linearVelocity - firstBody.linearVelocity;
    const float velocityAlongNormal = Math::Dot(relativeVelocity, normal);
    if (velocityAlongNormal >= 0.0F)
    {
        return;
    }

    const float restitution = std::fabs(velocityAlongNormal) < 0.5F
        ? 0.0F
        : std::min(firstBody.restitution, secondBody.restitution);
    const float normalImpulseMagnitude =
        -(1.0F + restitution) * velocityAlongNormal / inverseMassSum;
    const Vector3 normalImpulse = normal * normalImpulseMagnitude;
    firstBody.linearVelocity -= normalImpulse * firstInverseMass;
    secondBody.linearVelocity += normalImpulse * secondInverseMass;

    relativeVelocity = secondBody.linearVelocity - firstBody.linearVelocity;
    Vector3 tangent = relativeVelocity - normal * Math::Dot(relativeVelocity, normal);
    if (Math::LengthSquared(tangent) <= 1.0e-8F)
    {
        return;
    }

    tangent = Math::Normalize(tangent);
    float tangentImpulseMagnitude = -Math::Dot(relativeVelocity, tangent) / inverseMassSum;
    const float friction = std::sqrt(
        std::max(0.0F, firstBody.friction) * std::max(0.0F, secondBody.friction));
    const float maximumFrictionImpulse = normalImpulseMagnitude * friction;
    tangentImpulseMagnitude = std::clamp(
        tangentImpulseMagnitude,
        -maximumFrictionImpulse,
        maximumFrictionImpulse);

    const Vector3 frictionImpulse = tangent * tangentImpulseMagnitude;
    firstBody.linearVelocity -= frictionImpulse * firstInverseMass;
    secondBody.linearVelocity += frictionImpulse * secondInverseMass;
}
} // namespace Forje::Physics
