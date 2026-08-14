#include "Input/Input.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>
#include <stdexcept>

namespace
{
constexpr std::array<int, static_cast<std::size_t>(Forje::Input::Key::Count)> KeyMappings{
    GLFW_KEY_W,
    GLFW_KEY_A,
    GLFW_KEY_S,
    GLFW_KEY_D,
    GLFW_KEY_Q,
    GLFW_KEY_E,
    GLFW_KEY_SPACE,
    GLFW_KEY_LEFT_CONTROL,
    GLFW_KEY_LEFT_SHIFT,
    GLFW_KEY_ESCAPE,
    GLFW_KEY_TAB,
    GLFW_KEY_F,
    GLFW_KEY_EQUAL,
    GLFW_KEY_MINUS,
};
} // namespace

namespace Forje::Input
{
Input::Input(GLFWwindow* window)
    : m_Window(window)
{
    if (m_Window == nullptr)
    {
        throw std::invalid_argument("Input requires a valid GLFW window.");
    }

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetScrollCallback(m_Window, HandleScroll);
}

Input::~Input()
{
    if (m_Window != nullptr && glfwGetWindowUserPointer(m_Window) == this)
    {
        glfwSetScrollCallback(m_Window, nullptr);
        glfwSetWindowUserPointer(m_Window, nullptr);
    }
}

void Input::Update()
{
    m_PreviousKeys = m_CurrentKeys;
    for (std::size_t index = 0; index < KeyCount; ++index)
    {
        m_CurrentKeys[index] = glfwGetKey(m_Window, KeyMappings[index]) == GLFW_PRESS;
    }

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(m_Window, &mouseX, &mouseY);
    if (m_FirstMouseSample)
    {
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;
        m_FirstMouseSample = false;
        m_MouseDelta = {};
    }
    else if (m_CursorCaptured)
    {
        m_MouseDelta = {
            static_cast<float>(mouseX - m_LastMouseX),
            static_cast<float>(m_LastMouseY - mouseY),
        };
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;
    }
    else
    {
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;
        m_MouseDelta = {};
    }

    m_ScrollDelta = static_cast<float>(m_PendingScroll);
    m_PendingScroll = 0.0;
}

bool Input::IsKeyDown(const Key key) const noexcept
{
    return m_CurrentKeys[static_cast<std::size_t>(key)];
}

bool Input::WasKeyPressed(const Key key) const noexcept
{
    const std::size_t index = static_cast<std::size_t>(key);
    return m_CurrentKeys[index] && !m_PreviousKeys[index];
}

MouseDelta Input::GetMouseDelta() const noexcept
{
    return m_MouseDelta;
}

float Input::GetScrollDelta() const noexcept
{
    return m_ScrollDelta;
}

bool Input::IsCursorCaptured() const noexcept
{
    return m_CursorCaptured;
}

void Input::SetCursorCaptured(const bool captured)
{
    if (m_CursorCaptured == captured)
    {
        return;
    }

    m_CursorCaptured = captured;
    m_FirstMouseSample = true;
    glfwSetInputMode(
        m_Window,
        GLFW_CURSOR,
        captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    if (glfwRawMouseMotionSupported() == GLFW_TRUE)
    {
        glfwSetInputMode(
            m_Window,
            GLFW_RAW_MOUSE_MOTION,
            captured ? GLFW_TRUE : GLFW_FALSE);
    }
}

void Input::HandleScroll(GLFWwindow* window, double, const double yOffset)
{
    auto* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input != nullptr)
    {
        input->m_PendingScroll += yOffset;
    }
}
} // namespace Forje::Input
