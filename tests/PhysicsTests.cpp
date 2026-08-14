#include "Math/Matrix4.h"
#include "Physics/PhysicsSystem.h"
#include "Scene/Scene.h"

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
void Require(const bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

bool ApproximatelyEqual(const float left, const float right, const float tolerance)
{
    return std::fabs(left - right) <= tolerance;
}

void Simulate(Forje::Physics::PhysicsSystem& physics, Forje::Scene::Scene& scene, const int steps)
{
    for (int step = 0; step < steps; ++step)
    {
        physics.Update(scene, 1.0F / 120.0F);
    }
}

void TestEntityIdentity()
{
    Forje::Scene::Scene scene;
    const Forje::Scene::EntityId first = scene.CreateEntity("First").id;
    const Forje::Scene::EntityId second = scene.CreateEntity("Second").id;
    Require(first != 0 && second > first, "Scene entities must receive stable, unique IDs.");
    Require(scene.EntityCount() == 2, "Scene entity count is incorrect.");
}

void TestSceneValidation()
{
    Forje::Scene::Scene scene;
    bool rejectedInvalidScale = false;
    try
    {
        scene.CreateCube(
            "Invalid",
            {0.0F, 0.0F, 0.0F},
            {1.0F, 0.0F, 1.0F},
            {1.0F, 1.0F, 1.0F});
    }
    catch (const std::invalid_argument&)
    {
        rejectedInvalidScale = true;
    }
    Require(rejectedInvalidScale, "Scene accepted a cube with an invalid scale.");
}

void TestGravity()
{
    Forje::Scene::Scene scene;
    Forje::Scene::Entity& cube = scene.CreateCube(
        "Falling cube",
        {0.0F, 10.0F, 0.0F},
        {1.0F, 1.0F, 1.0F},
        {1.0F, 1.0F, 1.0F});
    Forje::Physics::PhysicsSystem physics;
    Simulate(physics, scene, 60);

    Require(cube.transform.position.y < 9.0F, "Gravity did not move the dynamic body.");
    Require(cube.rigidBody->linearVelocity.y < 0.0F, "Gravity did not change body velocity.");
}

void TestGroundCollision()
{
    Forje::Scene::Scene scene;
    scene.CreateCube(
        "Ground",
        {0.0F, -0.5F, 0.0F},
        {20.0F, 1.0F, 20.0F},
        {0.2F, 0.2F, 0.2F},
        Forje::Scene::BodyType::Static);
    const Forje::Scene::EntityId cubeId = scene.CreateCube(
        "Cube",
        {0.0F, 4.0F, 0.0F},
        {1.0F, 1.0F, 1.0F},
        {1.0F, 1.0F, 1.0F}).id;

    Forje::Physics::PhysicsSystem physics;
    Simulate(physics, scene, 600);
    const Forje::Scene::Entity& cube = scene.Entities()[1];

    Require(cube.id == cubeId, "Physics changed entity identity.");
    Require(
        ApproximatelyEqual(cube.transform.position.y, 0.5F, 0.02F),
        "A falling cube did not settle on the ground.");
    Require(
        std::fabs(cube.rigidBody->linearVelocity.y) < 0.1F,
        "Ground contact did not produce a stable resting velocity.");
}

void TestCubeStack()
{
    Forje::Scene::Scene scene;
    scene.CreateCube(
        "Ground",
        {0.0F, -0.5F, 0.0F},
        {20.0F, 1.0F, 20.0F},
        {0.2F, 0.2F, 0.2F},
        Forje::Scene::BodyType::Static);
    scene.CreateCube(
        "Lower",
        {0.0F, 2.0F, 0.0F},
        {1.0F, 1.0F, 1.0F},
        {1.0F, 1.0F, 1.0F});
    scene.CreateCube(
        "Upper",
        {0.0F, 4.0F, 0.0F},
        {1.0F, 1.0F, 1.0F},
        {1.0F, 1.0F, 1.0F});

    Forje::Physics::PhysicsSystem physics;
    Simulate(physics, scene, 900);

    const float lowerY = scene.Entities()[1].transform.position.y;
    const float upperY = scene.Entities()[2].transform.position.y;
    Require(ApproximatelyEqual(lowerY, 0.5F, 0.06F), "Lower cube did not settle.");
    Require(ApproximatelyEqual(upperY, 1.5F, 0.08F), "Cube stack is not stable.");
    Require(upperY - lowerY > 0.96F, "Stacked cubes remain interpenetrated.");
}

void TestModelMatrix()
{
    Forje::Scene::Transform transform;
    transform.position = {2.0F, 3.0F, 4.0F};
    transform.scale = {2.0F, 4.0F, 6.0F};
    const Forje::Math::Matrix4 matrix = transform.ModelMatrix();

    Require(matrix(0, 0) == 2.0F && matrix(1, 1) == 4.0F && matrix(2, 2) == 6.0F,
        "Model matrix scale is incorrect.");
    Require(matrix(0, 3) == 2.0F && matrix(1, 3) == 3.0F && matrix(2, 3) == 4.0F,
        "Model matrix translation is incorrect.");
}
} // namespace

int main()
{
    try
    {
        TestEntityIdentity();
        TestSceneValidation();
        TestGravity();
        TestGroundCollision();
        TestCubeStack();
        TestModelMatrix();
        std::cout << "All ForjeEngine foundation tests passed.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Test failure: " << exception.what() << '\n';
        return 1;
    }
}
