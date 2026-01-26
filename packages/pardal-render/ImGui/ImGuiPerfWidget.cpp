
#include <ImGui/ImGuiPerfWidget.h>
#ifdef PDL_FEATURE_IMGUI

#include "imgui.h"
#include "ImGuiPardalColors.h"
#include "ImGuiRenderer.h"
#include "implot.h"
#include "Base/ServiceLocator.h"
#include "Containers/ScrollingVector.h"

// Created on 2026-01-25 by sisco

namespace Details
{
    DefineGlobalConstexprVariableAccessor(float, BackgroundAlpha, 0.5f);
    
    struct DataPoint
    {
        float x;
        float y;
    };
    static pdl::ScrollingVector<DataPoint> g_scrollingFrameMsBuffer(1000);
    static pdl::ScrollingVector<DataPoint> g_scrollingMainMsBuffer(1000);
}

namespace pdl
{
    ImGuiPerfWidget::ImGuiPerfWidget()
    {
        ServiceLocator<ImGuiRenderer>::Ref().RegisterRenderable(this);
    }

    ImGuiPerfWidget::~ImGuiPerfWidget()
    {
        ServiceLocator<ImGuiRenderer>::Ref().UnregisterRenderable(this);
    }

    void ImGuiPerfWidget::Update(float _frameTime, float _mainTime)
    {
        m_lastDt = _frameTime;
        m_runningTime += m_lastDt;
        Details::g_scrollingFrameMsBuffer.Push({.x = m_runningTime, .y = m_lastDt});
        Details::g_scrollingMainMsBuffer.Push({.x = m_runningTime, .y = _mainTime});
    }

    void ImGuiPerfWidget::ImGuiMenuSetup()
    {
        ImGuiRenderable::ImGuiMenuSetup();
        
        if (ImGui::BeginMenu("Performance"))
        {
            if (ImGui::BeginMenu("Widget"))
                {
                if (ImGui::MenuItem("Tiny counter", "Ctrl+P", m_perfMode == PerfMode::TinyFPS) )
                {
                    m_perfMode = PerfMode::TinyFPS;       
                }
                if (ImGui::MenuItem("Normal counter", "Ctrl+P", m_perfMode == PerfMode::NormalFPS) )
                {
                    m_perfMode = PerfMode::NormalFPS;       
                }
                if (ImGui::MenuItem("Full Graph", "Ctrl+P", m_perfMode == PerfMode::FullGraph) )
                {
                    m_perfMode = PerfMode::FullGraph;       
                }       
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }

    void ImGuiPerfWidget::ImGuiRender()
    {
        CheckHotkeys();

        switch (m_perfMode) {
        case PerfMode::TinyFPS:
        case PerfMode::NormalFPS:
            {
                ImGui::SetNextWindowBgAlpha(Details::BackgroundAlpha());
        
                ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->WorkSize.x, ImGui::GetMainViewport()->WorkPos.y), ImGuiCond_Always, ImVec2(1.0f, 0));
        
                if (ImGui::Begin("##ImGuiPerfWidget", nullptr, 
                    ImGuiWindowFlags_NoDecoration | 
                    ImGuiWindowFlags_AlwaysAutoResize | 
                    ImGuiWindowFlags_NoSavedSettings | 
                    ImGuiWindowFlags_NoFocusOnAppearing | 
                    ImGuiWindowFlags_NoNav | 
                    ImGuiWindowFlags_NoInputs))
                {
                    if (m_perfMode == PerfMode::TinyFPS)
                        RenderTinyFPS();
                    else if (m_perfMode == PerfMode::NormalFPS)
                        RenderNormalFPS();
                    else
                        pdlAssert(false && "Unknown perf mode");
                    
                }
                ImGui::End();       

            }
            break;
        case PerfMode::FullGraph:
            RenderFullGraph();
            break;
        }
    }

    void ImGuiPerfWidget::RenderTinyFPS()
    {
        ImGui::TextColored(ImGuiPardalColors::NormalText(),"%.1f FPS", 1.0f / m_lastDt);
    }

    void ImGuiPerfWidget::RenderNormalFPS()
    {
        ImGui::TextColored(ImGuiPardalColors::GreenText(),"%.1f ", 1.0f / m_lastDt);
        ImGui::SameLine(); ImGui::TextColored(ImGuiPardalColors::TitleText(),"FPS");
        ImGui::TextColored(ImGuiPardalColors::NormalText(),"%.3f ", m_lastDt);
        ImGui::SameLine(); ImGui::TextColored(ImGuiPardalColors::TitleText(),"ms");
    }

    void ImGuiPerfWidget::RenderFullGraph()
    {
        ImGui::SetNextWindowBgAlpha(0.1);
        
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->WorkPos.x, ImGui::GetMainViewport()->WorkPos.y + ImGui::GetMainViewport()->WorkSize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->WorkSize.x, 0), ImGuiCond_Always);
        
        if (ImGui::Begin("##ImGuiPerfWidget", nullptr, 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_AlwaysAutoResize | 
            ImGuiWindowFlags_NoSavedSettings | 
            ImGuiWindowFlags_NoFocusOnAppearing | 
            ImGuiWindowFlags_NoNav))
        {
            static ImPlotAxisFlags yFlags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_LockMin | ImPlotAxisFlags_NoHighlight;
            static ImPlotAxisFlags xFlags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoHighlight;

            float minT = Details::g_scrollingFrameMsBuffer.GetSamples()[Details::g_scrollingFrameMsBuffer.GetOffset()].x;
        
            if (ImPlot::BeginPlot("##ScrollingMS", ImVec2(-1,ImGui::GetMainViewport()->WorkSize.y / 2.0f), ImPlotFlags_NoTitle | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText )) {
                ImPlot::SetupAxes(nullptr, nullptr, xFlags, yFlags);
                ImPlot::SetupAxisLimits(ImAxis_X1, minT, m_runningTime, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1,0,240);
                ImPlot::PushStyleColor(ImPlotCol_FrameBg, {0,0,0,0.1});
                ImPlot::PushStyleColor(ImPlotCol_PlotBg, {0,0,0,0.2});
                ImPlot::PushStyleColor(ImPlotCol_LegendBg, {0,0,0,0.1});
                ImPlot::PushStyleColor(ImPlotCol_InlayText, {1,1,1,0.3});
                
                ImPlot::SetNextLineStyle({1, 0.7, 0.5, 0.6}, 1.5f);
                ImPlot::PlotLine("Frame", 
                    &Details::g_scrollingFrameMsBuffer.GetSamples()[0].x, 
                    &Details::g_scrollingFrameMsBuffer.GetSamples()[0].y, 
                    Details::g_scrollingFrameMsBuffer.GetSamples().size(),
                    ImPlotLineFlags_Shaded, 
                     Details::g_scrollingFrameMsBuffer.GetOffset(),
                sizeof(float) * 2);

                ImPlot::SetNextLineStyle({0.4, 0.6, 1.0, 0.6}, 1.5f);
                ImPlot::PlotLine("Main", 
                    &Details::g_scrollingMainMsBuffer.GetSamples()[0].x, 
                    &Details::g_scrollingMainMsBuffer.GetSamples()[0].y, 
                    Details::g_scrollingMainMsBuffer.GetSamples().size(),
                    ImPlotLineFlags_Shaded, 
                     Details::g_scrollingMainMsBuffer.GetOffset(),
                sizeof(float) * 2);
                ImPlot::PopStyleColor(4);

                if (ImPlot::IsPlotHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Frame: %.3fms", m_lastDt);
                    ImGui::Text("CPU: %.3fms", Details::g_scrollingMainMsBuffer.GetSamples()[Details::g_scrollingMainMsBuffer.GetOffset()].y);
                    ImGui::EndTooltip();
                    
                }
                ImPlot::EndPlot();
            }
        }
        ImGui::End();       


    }

    void ImGuiPerfWidget::CheckHotkeys()
    {
        if (ImGui::IsKeyChordPressed(ImGuiKey_P | ImGuiMod_Ctrl))
        {
            m_perfMode = static_cast<PerfMode>((static_cast<int>(m_perfMode) + 1) % 3);
        }
    }
}
#endif
