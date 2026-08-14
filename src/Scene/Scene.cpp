#include "Scene/Scene.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace Forje::Scene
{
Entity& Scene::CreateEntity(std::string name)
{
    Entity entity;
    entity.id = m_NextEntityId++;
    entity.name = std::move(name);
    m_Entities.push_back(std::move(entity));
    return m_Entities.back();
}

Entity& Scene::CreateCube(
    std::string name,
    const Math::Vector3& position,
    const Math::Vector3& scale,
    const Math::Vector3& color,
    const BodyType bodyType,
    const float mass)
{
    const bool validScale = std::isfinite(scale.x) && std::isfinite(scale.y)
        && std::isfinite(scale.z) && scale.x > 0.0F && scale.y > 0.0F && scale.z > 0.0F;
    const bool validPosition =
        std::isfinite(position.x) && std::isfinite(position.y) && std::isfinite(position.z);
    if (!validScale || !validPosition)
    {
        throw std::invalid_argument("Cube position must be finite and scale must be positive.");
    }
    if (bodyType == BodyType::Dynamic && (!std::isfinite(mass) || mass <= 0.0F))
    {
        throw std::invalid_argument("A dynamic cube requires a finite, positive mass.");
    }

    Entity& entity = CreateEntity(std::move(name));
    entity.transform.position = position;
    entity.transform.scale = scale;
    entity.renderable = Renderable{color};
    entity.collider = BoxCollider{};

    RigidBody body;
    body.type = bodyType;
    body.SetMass(bodyType == BodyType::Static ? 0.0F : mass);
    body.useGravity = bodyType == BodyType::Dynamic;
    entity.rigidBody = body;
    return entity;
}

std::vector<Entity>& Scene::Entities() noexcept
{
    return m_Entities;
}

const std::vector<Entity>& Scene::Entities() const noexcept
{
    return m_Entities;
}

std::size_t Scene::EntityCount() const noexcept
{
    return m_Entities.size();
}
} // namespace Forje::Scene
