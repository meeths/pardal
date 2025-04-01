
#include <ImGui/ImGuiRenderer.h>
#include <imgui.h>
#include <memory>

#include "Application/ApplicationWindow.h"


#ifdef PDL_PLATFORM_WINDOWS
#include <backends/imgui_impl_win32.cpp>
#endif

#ifdef PDL_VULKAN
#include <backends/imgui_impl_vulkan.cpp>
#include <Renderer/Vulkan/VulkanDevice.h>
#endif
// Created on 2025-04-01 by sisco

namespace pdl
{
    ImGuiRenderer::ImGuiRenderer(const InitInfo& initInfo)
    {
        m_device = initInfo.m_device;
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        io.DisplaySize.x = static_cast<float>(initInfo.m_window.GetWindowSize().x);
        io.DisplaySize.y = static_cast<float>(initInfo.m_window.GetWindowSize().y);
        io.Fonts->Build();
        // Setup Platform/Renderer backends
#ifdef PDL_PLATFORM_WINDOWS
        ImGui_ImplWin32_Init(initInfo.m_window.GetNativeWindow());
#endif
#ifdef PDL_VULKAN
        ImGui_ImplVulkan_InitInfo imguiVulkanInitInfo = {};
        static_cast<VulkanDevice*>(initInfo.m_device)->FillImGuiInitInfo(imguiVulkanInitInfo);
        imguiVulkanInitInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        VkFormat colorAttachmentFormats[] = {
            initInfo.m_useHDR ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM
        };
        imguiVulkanInitInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = colorAttachmentFormats;
        imguiVulkanInitInfo.PipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
        ImGui_ImplVulkan_Init(&imguiVulkanInitInfo);
#endif
        initInfo.m_window.AddMouseMoveCallback([this](Math::Vector2 pos, bool lButton, bool rButton, bool mButton, unsigned int mods) {
            OnMouseMove(pos, lButton, rButton, mButton, mods);
        });
        initInfo.m_window.AddMouseWheelVCallback([this](float delta)
        {
            OnMouseWheelV(delta);
        });
        initInfo.m_window.AddMouseWheelHCallback([this](float delta)
        {
            OnMouseWheelH(delta);
        });
        initInfo.m_window.AddKeyDownCallback([this](int16 key){ OnKeyDown(key); });
        initInfo.m_window.AddKeyUpCallback([this](int16 key){ OnKeyUp(key); });
        initInfo.m_window.AddKeyInputCallback([this](int16 key){ OnKeyInput(key); });
    }

    ImGuiRenderer::~ImGuiRenderer()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiRenderer::BeginFrame()
    {
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

#ifdef PDL_PLATFORM_WINDOWS
        ImGui_ImplWin32_NewFrame();
#endif
#ifdef PDL_VULKAN
        ImGui_ImplVulkan_NewFrame();
#endif
    }

    void ImGuiRenderer::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImGuiRenderer::Render()
    {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<VulkanDevice*>(m_device)->GetVulkanDeviceQueue().GetCommandBuffer());
    }

    void ImGuiRenderer::OnMouseMove(Math::Vector2 pos, bool lButton, bool rButton, bool mButton, unsigned int mods)
    {
        ImGui::GetIO().MousePos = ImVec2(pos.x, pos.y);
        ImGui::GetIO().MouseDown[0] = lButton;
        ImGui::GetIO().MouseDown[1] = rButton;
        ImGui::GetIO().MouseDown[2] = mButton;
        ImGui::GetIO().KeyCtrl = mods != 0;
    }

    void ImGuiRenderer::OnMouseWheelV(float pos)
    {
        ImGui::GetIO().MouseWheel = pos;
    }

    void ImGuiRenderer::OnMouseWheelH(float pos)
    {
        ImGui::GetIO().MouseWheelH = pos;
    }

    void ImGuiRenderer::OnKeyUp(int16 key)
    {
        ImGui::GetIO().KeysData[key].Down = false;
    }

    void ImGuiRenderer::OnKeyDown(int16 key)
    {
        ImGui::GetIO().KeysData[key].Down = true;
    }

    void ImGuiRenderer::OnKeyInput(int16 key)
    {
        ImGui::GetIO().AddInputCharacter(key);
    }
}


