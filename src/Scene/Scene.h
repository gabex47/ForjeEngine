#pragma once

#include "Scene/Entity.h"

#include <cstddef>
#include <string>
#include <vector>

namespace Forje::Scene
{
class Scene final
{
public:
    Entity& CreateEntity(std::string name = "Entity");
    Entity& CreateCube(
        std::string name,
        const Math::Vector3& position,
        const Math::Vector3& scale,
        const Math::Vector3& color,
        BodyType bodyType = BodyType::Dynamic,
        float mass = 1.0F);

    std::vector<Entity>& Entities() noexcept;
    const std::vector<Entity>& Entities() const noexcept;
    std::size_t EntityCount() const noexcept;

private:
    std::vector<Entity> m_Entities;
    EntityId m_NextEntityId{1};
};
} // namespace Forje::Scene
