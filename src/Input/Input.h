#pragma once

#include <array>
#include <cstddef>

struct GLFWwindow;

namespace Forje::Input
{
enum class Key : std::size_t
{
    W,
    A,
    S,
    D,
    Q,
    E,
    Space,
    LeftControl,
    LeftShift,
    Escape,
    Tab,
    SpawnCube,
    IncreaseSpeed,
    DecreaseSpeed,
    Count,
};

struct MouseDelta
{
    float x{0.0F};
    float y{0.0F};
};

class Input final
{
public:
    explicit Input(GLFWwindow* window);
    ~Input();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    void Update();
    bool IsKeyDown(Key key) const noexcept;
    bool WasKeyPressed(Key key) const noexcept;

    MouseDelta GetMouseDelta() const noexcept;
    float GetScrollDelta() const noexcept;
    bool IsCursorCaptured() const noexcept;
    void SetCursorCaptured(bool captured);

private:
    static void HandleScroll(GLFWwindow* window, double xOffset, double yOffset);

    static constexpr std::size_t KeyCount = static_cast<std::size_t>(Key::Count);

    GLFWwindow* m_Window{nullptr};
    std::array<bool, KeyCount> m_CurrentKeys{};
    std::array<bool, KeyCount> m_PreviousKeys{};
    MouseDelta m_MouseDelta{};
    double m_LastMouseX{0.0};
    double m_LastMouseY{0.0};
    double m_PendingScroll{0.0};
    float m_ScrollDelta{0.0F};
    bool m_FirstMouseSample{true};
    bool m_CursorCaptured{false};
};
} // namespace Forje::Input
