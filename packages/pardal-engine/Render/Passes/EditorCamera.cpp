#include "Render/Passes/EditorCamera.h"

#include "Application/IApplicationWindow.h"
#include "Input/InputManager.h"
#include "Input/KeyboardState.h"
#include "Input/MouseState.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

// Created on 2026-03-28 by Sisco

// Win32 virtual-key codes used directly as KeyboardState indices.
// Defined here to avoid a <windows.h> include in the header.
#ifndef VK_SHIFT
#define VK_SHIFT   0x10
#define VK_MENU    0x12   // Alt
#define VK_KEY_W   0x57
#define VK_KEY_A   0x41
#define VK_KEY_S   0x53
#define VK_KEY_D   0x44
#define VK_KEY_Q   0x51
#define VK_KEY_E   0x45
#define VK_KEY_F   0x46
#endif

namespace pdl
{

static constexpr float kPitchLimit  = glm::radians(89.0f);
static constexpr float kMouseSens   = 0.004f;   // radians per pixel
static constexpr float kOrbitSens   = 0.005f;   // radians per pixel
static constexpr float kPanSens     = 0.003f;   // units per pixel, scaled by distance
static constexpr float kDollyScale  = 0.1f;     // fraction of distance per scroll tick
static constexpr float kFlySpeedMin = 0.5f;
static constexpr float kFlySpeedMax = 100.0f;
static constexpr float kFlySpeedMul = 1.2f;     // scroll multiplier in fly mode

// ---------------------------------------------------------------------------

void EditorCamera::Initialize(IApplicationWindow& window)
{
    m_window = &window;
    UpdateView();
    UpdateProj();
}

// ---------------------------------------------------------------------------

void EditorCamera::SetAspect(float aspect)
{
    m_aspect = aspect;
    UpdateProj();
}

// ---------------------------------------------------------------------------

void EditorCamera::Update(float deltaTime, const InputManager& input)
{
    const MouseState&    mouse = input.GetMouseState();
    const KeyboardState& kbd   = input.GetKeyboardState();

    const Math::Vector2 mousePos    = mouse.mCursorPosition;
    const Math::Vector2 mouseDelta  = m_prevMouseValid
                                    ? (mousePos - m_prevMousePos)
                                    : Math::Vector2(0.0f, 0.0f);

    const bool rmbDown  = mouse.mRMB.pressed;
    const bool lmbDown  = mouse.mLMB.pressed;
    const bool mmbDown  = mouse.mMMB.pressed;
    const bool altDown  = kbd.mKeys[VK_MENU].pressed;
    const bool shiftDown = kbd.mKeys[VK_SHIFT].pressed;

    // ── Fly mode (RMB held, no Alt) ─────────────────────────────────────────
    const bool flyMode = rmbDown && !altDown;

    if (flyMode != m_flyModeActive)
    {
        m_flyModeActive = flyMode;
        if (m_window)
            m_window->SetCursorVisible(!flyMode);
    }

    if (flyMode)
    {
        // Look: mouse drag rotates yaw/pitch.
        // Negate both axes to match standard FPS convention: drag-right turns right,
        // drag-down looks down.
        m_yaw   -= mouseDelta.x * kMouseSens;
        m_pitch += mouseDelta.y * kMouseSens;
        m_pitch  = glm::clamp(m_pitch, -kPitchLimit, kPitchLimit);

        // Rebuild direction vectors after rotation.
        UpdateView();

        // Movement: WASD on the view plane, QE for vertical.
        const float speed = m_flySpeed * (shiftDown ? 3.0f : 1.0f);
        Math::Vector3 move(0.0f);
        if (kbd.mKeys[VK_KEY_W].pressed) move += m_forward;
        if (kbd.mKeys[VK_KEY_S].pressed) move -= m_forward;
        if (kbd.mKeys[VK_KEY_D].pressed) move += m_right;
        if (kbd.mKeys[VK_KEY_A].pressed) move -= m_right;
        if (kbd.mKeys[VK_KEY_E].pressed) move += Math::Vector3(0.0f, 1.0f, 0.0f);
        if (kbd.mKeys[VK_KEY_Q].pressed) move -= Math::Vector3(0.0f, 1.0f, 0.0f);

        if (glm::length(move) > 1e-6f)
        {
            move = glm::normalize(move) * speed * deltaTime;
            m_eye    += move;
            m_pivot  += move;
            m_distance = glm::length(m_pivot - m_eye);
        }

        // Scroll adjusts fly speed.
        if (mouse.mWheelV != 0.0f)
        {
            m_flySpeed *= (mouse.mWheelV > 0.0f) ? kFlySpeedMul : (1.0f / kFlySpeedMul);
            m_flySpeed  = glm::clamp(m_flySpeed, kFlySpeedMin, kFlySpeedMax);
        }
    }

    // ── Orbit (Alt + LMB drag) ──────────────────────────────────────────────
    else if (altDown && lmbDown)
    {
        m_yaw   += mouseDelta.x * kOrbitSens;
        m_pitch += mouseDelta.y * kOrbitSens;
        m_pitch  = glm::clamp(m_pitch, -kPitchLimit, kPitchLimit);
    }

    // ── Pan (MMB drag) ───────────────────────────────────────────────────────
    else if (mmbDown)
    {
        const float panScale = kPanSens * m_distance;
        const Math::Vector3 pan = (-mouseDelta.x * m_right + mouseDelta.y * m_up) * panScale;
        m_pivot += pan;
        m_eye   += pan;
    }

    // ── Dolly (scroll, no RMB) ───────────────────────────────────────────────
    else if (mouse.mWheelV != 0.0f)
    {
        const float factor = 1.0f - mouse.mWheelV * kDollyScale;
        m_distance = glm::max(m_distance * factor, 0.01f);
    }

    // ── F — reset ────────────────────────────────────────────────────────────
    if (kbd.mKeys[VK_KEY_F].justPressed)
    {
        m_pivot    = kDefaultPivot;
        m_yaw      = kDefaultYaw;
        m_pitch    = kDefaultPitch;
        m_distance = kDefaultDistance;
    }

    // ── Rebuild view every frame ──────────────────────────────────────────────
    UpdateView();

    m_prevMousePos   = mousePos;
    m_prevMouseValid = true;
}

// ---------------------------------------------------------------------------

void EditorCamera::UpdateView()
{
    // Derive forward from spherical yaw/pitch.
    m_forward = glm::normalize(Math::Vector3(
        glm::cos(m_pitch) * glm::sin(m_yaw),
        glm::sin(m_pitch),
        glm::cos(m_pitch) * glm::cos(m_yaw)));

    m_right = glm::normalize(glm::cross(m_forward, Math::Vector3(0.0f, 1.0f, 0.0f)));
    m_up    = glm::normalize(glm::cross(m_right, m_forward));

    // Eye derived from pivot + direction.
    m_eye = m_pivot - m_forward * m_distance;

    m_view = glm::lookAt(m_eye, m_pivot, Math::Vector3(0.0f, 1.0f, 0.0f));
}

// ---------------------------------------------------------------------------

void EditorCamera::UpdateProj()
{
    m_proj = glm::perspectiveRH_ZO(glm::radians(m_fovY), m_aspect, m_near, m_far);
    m_proj[1][1] *= -1.0f;   // Vulkan: flip Y
}

} // namespace pdl
