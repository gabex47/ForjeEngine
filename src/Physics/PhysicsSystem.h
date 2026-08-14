#pragma once

#include "Math/Vector3.h"

#include <vector>

namespace Forje::Scene
{
class Scene;
struct Entity;
}

namespace Forje::Physics
{
struct PhysicsSettings
{
    Math::Vector3 gravity{0.0F, -9.81F, 0.0F};
    float fixedTimeStep{1.0F / 120.0F};
    float maximumLinearSpeed{80.0F};
    int maximumSubSteps{8};
    int solverIterations{8};
};

class PhysicsSystem final
{
public:
    explicit PhysicsSystem(PhysicsSettings settings = {});

    void Update(Scene::Scene& scene, float deltaTime);
    const PhysicsSettings& Settings() const noexcept;
    float InterpolationAlpha() const noexcept;

private:
    struct CollisionPair
    {
        Scene::Entity* first{nullptr};
        Scene::Entity* second{nullptr};
    };

    void Step(Scene::Scene& scene, float deltaTime);
    std::vector<CollisionPair> BuildCollisionPairs(Scene::Scene& scene) const;
    void ResolveCollision(Scene::Entity& first, Scene::Entity& second) const;

    PhysicsSettings m_Settings;
    float m_Accumulator{0.0F};
};
} // namespace Forje::Physics
