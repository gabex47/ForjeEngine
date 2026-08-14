#pragma once

#include "Camera/Camera.h"
#include "Physics/PhysicsSystem.h"
#include "Scene/Scene.h"

#include <cstddef>
#include <filesystem>
#include <memory>

struct GLFWwindow;

namespace Forje::Graphics
{
class Renderer;
}

namespace Forje::Input
{
class Input;
}

namespace Forje::Core
{
class Application final
{
public:
    explicit Application(const char* executablePath);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void Run();

private:
    void Initialize();
    void InitializeScene();
    void Shutdown() noexcept;
    void ProcessInput(float deltaTime);
    void SpawnCube();
    void UpdateWindowTitle() const;

    static void HandleFramebufferResize(GLFWwindow* window, int width, int height);

    GLFWwindow* m_Window{nullptr};
    std::filesystem::path m_ExecutableDirectory;
    bool m_GlfwInitialized{false};
    std::unique_ptr<Input::Input> m_Input;
    std::unique_ptr<Graphics::Renderer> m_Renderer;
    Camera::Camera m_Camera;
    Scene::Scene m_Scene;
    Physics::PhysicsSystem m_Physics;
    std::size_t m_SpawnedCubeCount{0};
};
} // namespace Forje::Core
