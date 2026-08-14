#pragma once

#include "Scene/Components.h"

#include <cstdint>
#include <optional>
#include <string>

namespace Forje::Scene
{
using EntityId = std::uint64_t;

struct Entity
{
    EntityId id{0};
    std::string name;
    Transform transform;
    std::optional<Renderable> renderable;
    std::optional<BoxCollider> collider;
    std::optional<RigidBody> rigidBody;
};
} // namespace Forje::Scene
