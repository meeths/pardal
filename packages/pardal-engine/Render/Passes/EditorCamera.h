#pragma once

#include "Math/Matrix44.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

// Created on 2026-03-28 by Sisco

namespace pdl
{
class InputManager;
class IApplicationWindow;

// Unity3D-style scene-view camera.
//
// Control summary:
//   RMB held          — Fly mode: WASD/QE to move, mouse to look, scroll to
//                       adjust fly speed, Shift = 3× speed boost.
//   Alt + LMB drag    — Orbit around the current pivot point.
//   MMB drag          — Pan (translate pivot and eye in the view plane).
//   Scroll (no mods)  — Dolly: move eye and pivot along the view axis.
//   F                 — Reset to default position/orientation.
//
// The camera maintains a pivot point (orbit centre) and an eye position
// derived from it: eye = pivot - forward * distance.
class EditorCamera
{
public:
    // Call once to bind the window for cursor show/hide during fly mode.
    void Initialize(IApplicationWindow& window);

    // Call every frame before rendering.
    void Update(float deltaTime, const InputManager& input);

    const Math::Matrix44& GetView()      const { return m_view; }
    const Math::Matrix44& GetProj()      const { return m_proj; }
    const Math::Vector3&  GetPosition()  const { return m_eye; }

    // Must be called whenever the viewport dimensions change.
    void SetAspect(float aspect);

private:
    void UpdateView();
    void UpdateProj();

    // ── Camera state ────────────────────────────────────────────────────────
    Math::Vector3 m_pivot    = {0.0f, 0.0f,  0.0f};
    float         m_yaw      = 0.0f;          // radians, around world Y
    float         m_pitch    = -0.35f;        // radians, clamped [-89°, 89°] — negative = above horizon
    float         m_distance = 5.0f;          // eye–pivot distance

    // Cached derived values
    Math::Vector3 m_eye     = {1.5f, 1.5f, 3.0f};
    Math::Vector3 m_forward = {0.0f, 0.0f, -1.0f};
    Math::Vector3 m_right   = {1.0f, 0.0f,  0.0f};
    Math::Vector3 m_up      = {0.0f, 1.0f,  0.0f};

    // ── Projection ──────────────────────────────────────────────────────────
    float m_aspect  = 16.0f / 9.0f;
    float m_fovY    = 60.0f;   // degrees
    float m_near    = 0.1f;
    float m_far     = 1000.0f;

    // ── Matrices ────────────────────────────────────────────────────────────
    Math::Matrix44 m_view = Math::Matrix44(1.0f);
    Math::Matrix44 m_proj = Math::Matrix44(1.0f);

    // ── Input state ─────────────────────────────────────────────────────────
    Math::Vector2 m_prevMousePos   = {0.0f, 0.0f};
    bool          m_prevMouseValid = false;
    float         m_flySpeed       = 3.0f;   // units/second (scroll adjusts this)

    // ── Fly-mode cursor hiding ───────────────────────────────────────────────
    IApplicationWindow* m_window       = nullptr;
    bool                m_flyModeActive = false;

    // Default state for F-reset
    static constexpr Math::Vector3 kDefaultPivot    = {0.0f, 0.0f, 0.0f};
    static constexpr float         kDefaultYaw      = 0.0f;
    static constexpr float         kDefaultPitch    = -0.35f;
    static constexpr float         kDefaultDistance = 5.0f;
};

} // namespace pdl
