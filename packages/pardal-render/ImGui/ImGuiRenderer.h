
#pragma once
#ifdef PDL_FEATURE_IMGUI
#include "Base/BaseTypes.h"
#include "Containers/Vector.h"
#include "ImGui/ImGuiRenderable.h"
#include "Math/Vector2.h"
#include "Memory/UniquePointer.h"
#include "Threading/SRWSynchronized.h"

#ifdef PDL_VULKAN
#include "Renderer/IRHICommandBuffer.h"
#include "Renderer/Vulkan/lvk/HelpersImGui.h"
#endif

// Created on 2025-04-01 by sisco

namespace pdl
{
class IRenderer;
class ImGuiRenderable;
class ApplicationWindow;
#ifdef PDL_VULKAN
class VulkanRHIContext;
#endif

class ImGuiRenderer 
{
public:
    struct InitInfo
    {
        ApplicationWindow& m_window;
        IRenderer* m_renderer;
    };
    void Initialize(const InitInfo& initInfo);
    ~ImGuiRenderer();
    
    void Render();

    void RegisterRenderable(ImGuiRenderable* renderable);
    void UnregisterRenderable(ImGuiRenderable* renderable);


#ifdef PDL_VULKAN
    void BeginFrame(TextureHandle colorTarget);
    void EndFrame(IRHICommandBuffer& cmd);
#endif
private:
    void OnMouseMove(Math::Vector2 pos, bool lButton, bool rButton, bool mButton, unsigned int mods);
    void OnMouseWheelV(float pos);
    void OnMouseWheelH(float pos);
    void OnKeyUp(int16 key);
    void OnKeyDown(int16 key);
    void OnKeyInput(int16 key);

    enum class ViewMode
    {
        Full,
        NoMenu,
        Hidden
    };

    ViewMode m_viewMode = ViewMode::Full;
#ifdef PDL_VULKAN
    UniquePointer<lvk::ImGuiRenderer> m_lvkImGuiRenderer;
    VulkanRHIContext*                 m_rhiContext = nullptr;
#endif
    SRWSynchronized<Vector<ImGuiRenderable*>> m_renderables;
};

}
#endif
