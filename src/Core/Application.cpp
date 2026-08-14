#include "Core/Application.h"

#include "Graphics/Renderer.h"
#include "Input/Input.h"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr char WindowTitle[] = "ForjeEngine";
constexpr std::size_t MaximumCubeCount = 2000;

constexpr std::array<Forje::Math::Vector3, 8> CubeColors{
    Forje::Math::Vector3{0.20F, 0.58F, 1.0F},
    Forje::Math::Vector3{0.98F, 0.42F, 0.32F},
    Forje::Math::Vector3{0.35F, 0.88F, 0.55F},
    Forje::Math::Vector3{0.96F, 0.72F, 0.22F},
    Forje::Math::Vector3{0.68F, 0.42F, 1.0F},
    Forje::Math::Vector3{0.20F, 0.86F, 0.90F},
    Forje::Math::Vector3{1.0F, 0.48F, 0.72F},
    Forje::Math::Vector3{0.72F, 0.80F, 0.92F},
};

void HandleGlfwError(const int errorCode, const char* description)
{
    std::cerr << "GLFW error " << errorCode << ": "
              << (description != nullptr ? description : "Unknown error") << '\n';
}
} // namespace

namespace Forje::Core
{
Application::Application(const char* executablePath)
{
    if (executablePath != nullptr && executablePath[0] != '\0')
    {
        m_ExecutableDirectory = std::filesystem::absolute(executablePath).parent_path();
    }
    else
    {
        m_ExecutableDirectory = std::filesystem::current_path();
    }

    try
    {
        Initialize();
        InitializeScene();
    }
    catch (...)
    {
        Shutdown();
        throw;
    }
}

Application::~Application()
{
    Shutdown();
}

void Application::Initialize()
{
    glfwSetErrorCallback(HandleGlfwError);
    if (glfwInit() != GLFW_TRUE)
    {
        throw std::runtime_error("GLFW initialization failed.");
    }
    m_GlfwInitialized = true;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    m_Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle, nullptr, nullptr);
    if (m_Window == nullptr)
    {
        throw std::runtime_error("GLFW could not create the OpenGL window.");
    }

    glfwMakeContextCurrent(m_Window);
    if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0)
    {
        throw std::runtime_error("GLAD could not load the OpenGL function pointers.");
    }

    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(m_Window, HandleFramebufferResize);

    m_Input = std::make_unique<Input::Input>(m_Window);
    m_Input->SetCursorCaptured(true);
    m_Renderer = std::make_unique<Graphics::Renderer>(m_ExecutableDirectory / "assets");

    std::cout
        << "ForjeEngine Phase 2 controls:\n"
        << "  Move: WASD | Up: E/Space | Down: Q/Ctrl | Sprint: Shift\n"
        << "  Look: Mouse | Speed: Wheel or +/- | Spawn cube: F\n"
        << "  Release/capture mouse: Tab | Quit: Escape\n";
}

void Application::InitializeScene()
{
    Scene::Entity& ground = m_Scene.CreateCube(
        "Ground",
        {0.0F, -0.5F, 0.0F},
        {32.0F, 1.0F, 32.0F},
        {0.12F, 0.15F, 0.21F},
        Scene::BodyType::Static);
    ground.rigidBody->friction = 0.9F;

    Scene::Entity& defaultCube = m_Scene.CreateCube(
        "Default Cube",
        {0.0F, 3.5F, 0.0F},
        {1.0F, 1.0F, 1.0F},
        CubeColors[0]);
    defaultCube.rigidBody->restitution = 0.08F;

    UpdateWindowTitle();
}

void Application::Run()
{
    double previousTime = glfwGetTime();
    while (glfwWindowShouldClose(m_Window) == GLFW_FALSE)
    {
        glfwPollEvents();
        m_Input->Update();

        const double currentTime = glfwGetTime();
        const float deltaTime = std::min(
            static_cast<float>(currentTime - previousTime),
            0.1F);
        previousTime = currentTime;

        ProcessInput(deltaTime);
        m_Physics.Update(m_Scene, deltaTime);

        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetFramebufferSize(m_Window, &framebufferWidth, &framebufferHeight);
        m_Renderer->Render(m_Scene, m_Camera, framebufferWidth, framebufferHeight);

        glfwSwapBuffers(m_Window);
    }
}

void Application::Shutdown() noexcept
{
    if (m_Window != nullptr)
    {
        glfwMakeContextCurrent(m_Window);
        m_Renderer.reset();
        m_Input.reset();
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    if (m_GlfwInitialized)
    {
        glfwTerminate();
        m_GlfwInitialized = false;
    }
}

void Application::ProcessInput(const float deltaTime)
{
    if (m_Input->WasKeyPressed(Input::Key::Escape))
    {
        glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
    }

    if (m_Input->WasKeyPressed(Input::Key::Tab))
    {
        m_Input->SetCursorCaptured(!m_Input->IsCursorCaptured());
        UpdateWindowTitle();
    }

    const bool speedChanged = m_Input->GetScrollDelta() != 0.0F
        || m_Input->WasKeyPressed(Input::Key::IncreaseSpeed)
        || m_Input->WasKeyPressed(Input::Key::DecreaseSpeed);
    m_Camera.Update(*m_Input, deltaTime);

    if (m_Input->WasKeyPressed(Input::Key::SpawnCube))
    {
        SpawnCube();
    }
    else if (speedChanged)
    {
        UpdateWindowTitle();
    }
}

void Application::SpawnCube()
{
    const std::size_t currentCubeCount = m_SpawnedCubeCount + 1;
    if (currentCubeCount >= MaximumCubeCount)
    {
        std::cerr << "Cube limit reached (" << MaximumCubeCount << ").\n";
        return;
    }

    ++m_SpawnedCubeCount;
    Math::Vector3 spawnPosition = m_Camera.Position() + m_Camera.Forward() * 3.0F;
    spawnPosition.y = std::max(spawnPosition.y, 1.0F);

    Scene::Entity& cube = m_Scene.CreateCube(
        "Cube " + std::to_string(m_SpawnedCubeCount),
        spawnPosition,
        {1.0F, 1.0F, 1.0F},
        CubeColors[m_SpawnedCubeCount % CubeColors.size()]);
    cube.rigidBody->linearVelocity = m_Camera.Forward() * 3.5F;
    cube.rigidBody->restitution = 0.12F;

    UpdateWindowTitle();
}

void Application::UpdateWindowTitle() const
{
    std::ostringstream title;
    title << WindowTitle << " | Cubes: " << (m_SpawnedCubeCount + 1)
          << " | Speed: " << std::fixed << std::setprecision(1) << m_Camera.MovementSpeed()
          << " | F spawn | Tab "
          << (m_Input != nullptr && m_Input->IsCursorCaptured() ? "release mouse" : "capture mouse");
    glfwSetWindowTitle(m_Window, title.str().c_str());
}

void Application::HandleFramebufferResize(GLFWwindow*, const int width, const int height)
{
    glViewport(0, 0, width, height);
}
} // namespace Forje::Core
