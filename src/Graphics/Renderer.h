#pragma once

#include <filesystem>
#include <memory>

namespace Forje::Camera
{
class Camera;
}

namespace Forje::Scene
{
class Scene;
}

namespace Forje::Graphics
{
class Mesh;
class Shader;

class Renderer final
{
public:
    explicit Renderer(const std::filesystem::path& assetDirectory);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Render(
        const Scene::Scene& scene,
        const Camera::Camera& camera,
        int framebufferWidth,
        int framebufferHeight) const;

private:
    std::unique_ptr<Shader> m_CubeShader;
    std::unique_ptr<Mesh> m_CubeMesh;
};
} // namespace Forje::Graphics
