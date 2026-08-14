#include "Graphics/Renderer.h"

#include "Camera/Camera.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Math/Vector3.h"
#include "Scene/Scene.h"

#include <glad/gl.h>

namespace Forje::Graphics
{
Renderer::Renderer(const std::filesystem::path& assetDirectory)
{
    const std::filesystem::path shaderDirectory = assetDirectory / "shaders";
    m_CubeShader = std::make_unique<Shader>(
        shaderDirectory / "cube.vert",
        shaderDirectory / "cube.frag");
    m_CubeMesh = std::make_unique<Mesh>(Mesh::CubeVertices(), Mesh::CubeIndices());

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_MULTISAMPLE);
}

Renderer::~Renderer() = default;

void Renderer::Render(
    const Scene::Scene& scene,
    const Camera::Camera& camera,
    const int framebufferWidth,
    const int framebufferHeight) const
{
    if (framebufferWidth <= 0 || framebufferHeight <= 0)
    {
        return;
    }

    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glClearColor(0.025F, 0.035F, 0.06F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspectRatio =
        static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
    m_CubeShader->Use();
    m_CubeShader->SetMatrix4("uView", camera.ViewMatrix());
    m_CubeShader->SetMatrix4("uProjection", camera.ProjectionMatrix(aspectRatio));
    m_CubeShader->SetVector3("uViewPosition", camera.Position());
    m_CubeShader->SetVector3(
        "uLightDirection",
        Math::Normalize(Math::Vector3{-0.55F, -1.0F, -0.35F}));

    for (const Scene::Entity& entity : scene.Entities())
    {
        if (!entity.renderable)
        {
            continue;
        }

        m_CubeShader->SetMatrix4("uModel", entity.transform.ModelMatrix());
        m_CubeShader->SetVector3("uBaseColor", entity.renderable->color);
        m_CubeMesh->Draw();
    }

    glBindVertexArray(0);
}
} // namespace Forje::Graphics
