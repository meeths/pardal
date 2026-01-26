
#pragma once
#ifdef PDL_FEATURE_IMGUI
#include "ImGui/ImGuiRenderable.h"

// Created on 2026-01-25 by sisco

namespace pdl
{

class ImGuiPerfWidget : public ImGuiRenderable
{
public:
    ImGuiPerfWidget();
    ~ImGuiPerfWidget() override;
    void Update(float _frameTime, float _mainTime);
    void ImGuiMenuSetup() override;
    void ImGuiRender() override;
private:
    void RenderTinyFPS();
    void RenderNormalFPS();
    void RenderFullGraph();
    
    enum class PerfMode
    {
        TinyFPS,
        NormalFPS,
        FullGraph
    };
    void CheckHotkeys();
    
    float m_lastDt = 0.0f;
    float m_runningTime = 0.0f;
    
    PerfMode m_perfMode = PerfMode::NormalFPS;
};

}
#endif