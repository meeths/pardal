#ifdef PDL_FEATURE_IMGUI
#include <ImGui/ImGuiRenderer.h>
#include <imgui.h>
#include <implot.h>

#include "Application/ApplicationWindow.h"


#ifdef PDL_PLATFORM_WINDOWS
#include <backends/imgui_impl_win32.cpp>
#endif

#ifdef PDL_VULKAN
#include <backends/imgui_impl_vulkan.cpp>
#include <Renderer/Vulkan/VulkanDevice.h>
#endif
// Created on 2025-04-01 by sisco

#include <algorithm>


namespace Details
{
    ImGuiKey VirtualKeyToImGuiKey(WPARAM wParam)
    {
        switch (wParam)
        {
            case VK_CONTROL: return ImGuiMod_Ctrl;
            
            case VK_SHIFT: return ImGuiMod_Shift;
            case VK_MENU: return ImGuiMod_Alt;
            // case VK_RWIN: return ImGuiMod_Super;
            // case VK_LWIN: return ImGuiMod_Super;
            case VK_TAB: return ImGuiKey_Tab;
            case VK_LEFT: return ImGuiKey_LeftArrow;
            case VK_RIGHT: return ImGuiKey_RightArrow;
            case VK_UP: return ImGuiKey_UpArrow;
            case VK_DOWN: return ImGuiKey_DownArrow;
            case VK_PRIOR: return ImGuiKey_PageUp;
            case VK_NEXT: return ImGuiKey_PageDown;
            case VK_HOME: return ImGuiKey_Home;
            case VK_END: return ImGuiKey_End;
            case VK_INSERT: return ImGuiKey_Insert;
            case VK_DELETE: return ImGuiKey_Delete;
            case VK_BACK: return ImGuiKey_Backspace;
            case VK_SPACE: return ImGuiKey_Space;
            case VK_RETURN: return ImGuiKey_Enter;
            case VK_ESCAPE: return ImGuiKey_Escape;
            case VK_OEM_7: return ImGuiKey_Apostrophe;
            case VK_OEM_COMMA: return ImGuiKey_Comma;
            case VK_OEM_MINUS: return ImGuiKey_Minus;
            case VK_OEM_PERIOD: return ImGuiKey_Period;
            case VK_OEM_2: return ImGuiKey_Slash;
            case VK_OEM_1: return ImGuiKey_Semicolon;
            case VK_OEM_PLUS: return ImGuiKey_Equal;
            case VK_OEM_4: return ImGuiKey_LeftBracket;
            case VK_OEM_5: return ImGuiKey_Backslash;
            case VK_OEM_6: return ImGuiKey_RightBracket;
            case VK_OEM_3: return ImGuiKey_GraveAccent;
            case VK_CAPITAL: return ImGuiKey_CapsLock;
            case VK_SCROLL: return ImGuiKey_ScrollLock;
            case VK_NUMLOCK: return ImGuiKey_NumLock;
            case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
            case VK_PAUSE: return ImGuiKey_Pause;
            case VK_NUMPAD0: return ImGuiKey_Keypad0;
            case VK_NUMPAD1: return ImGuiKey_Keypad1;
            case VK_NUMPAD2: return ImGuiKey_Keypad2;
            case VK_NUMPAD3: return ImGuiKey_Keypad3;
            case VK_NUMPAD4: return ImGuiKey_Keypad4;
            case VK_NUMPAD5: return ImGuiKey_Keypad5;
            case VK_NUMPAD6: return ImGuiKey_Keypad6;
            case VK_NUMPAD7: return ImGuiKey_Keypad7;
            case VK_NUMPAD8: return ImGuiKey_Keypad8;
            case VK_NUMPAD9: return ImGuiKey_Keypad9;
            case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
            case VK_DIVIDE: return ImGuiKey_KeypadDivide;
            case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
            case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
            case VK_ADD: return ImGuiKey_KeypadAdd;
            case VK_RETURN + 256: return ImGuiKey_KeypadEnter;
            case VK_LSHIFT: return ImGuiKey_LeftShift;
            case VK_LCONTROL: return ImGuiKey_LeftCtrl;
            case VK_LMENU: return ImGuiKey_LeftAlt;
            case VK_LWIN: return ImGuiKey_LeftSuper;
            case VK_RSHIFT: return ImGuiKey_RightShift;
            case VK_RCONTROL: return ImGuiKey_RightCtrl;
            case VK_RMENU: return ImGuiKey_RightAlt;
            case VK_RWIN: return ImGuiKey_RightSuper;
            case VK_APPS: return ImGuiKey_Menu;
            case '0': return ImGuiKey_0;
            case '1': return ImGuiKey_1;
            case '2': return ImGuiKey_2;
            case '3': return ImGuiKey_3;
            case '4': return ImGuiKey_4;
            case '5': return ImGuiKey_5;
            case '6': return ImGuiKey_6;
            case '7': return ImGuiKey_7;
            case '8': return ImGuiKey_8;
            case '9': return ImGuiKey_9;
            case 'A': return ImGuiKey_A;
            case 'B': return ImGuiKey_B;
            case 'C': return ImGuiKey_C;
            case 'D': return ImGuiKey_D;
            case 'E': return ImGuiKey_E;
            case 'F': return ImGuiKey_F;
            case 'G': return ImGuiKey_G;
            case 'H': return ImGuiKey_H;
            case 'I': return ImGuiKey_I;
            case 'J': return ImGuiKey_J;
            case 'K': return ImGuiKey_K;
            case 'L': return ImGuiKey_L;
            case 'M': return ImGuiKey_M;
            case 'N': return ImGuiKey_N;
            case 'O': return ImGuiKey_O;
            case 'P': return ImGuiKey_P;
            case 'Q': return ImGuiKey_Q;
            case 'R': return ImGuiKey_R;
            case 'S': return ImGuiKey_S;
            case 'T': return ImGuiKey_T;
            case 'U': return ImGuiKey_U;
            case 'V': return ImGuiKey_V;
            case 'W': return ImGuiKey_W;
            case 'X': return ImGuiKey_X;
            case 'Y': return ImGuiKey_Y;
            case 'Z': return ImGuiKey_Z;
            case VK_F1: return ImGuiKey_F1;
            case VK_F2: return ImGuiKey_F2;
            case VK_F3: return ImGuiKey_F3;
            case VK_F4: return ImGuiKey_F4;
            case VK_F5: return ImGuiKey_F5;
            case VK_F6: return ImGuiKey_F6;
            case VK_F7: return ImGuiKey_F7;
            case VK_F8: return ImGuiKey_F8;
            case VK_F9: return ImGuiKey_F9;
            case VK_F10: return ImGuiKey_F10;
            case VK_F11: return ImGuiKey_F11;
            case VK_F12: return ImGuiKey_F12;
            case VK_F13: return ImGuiKey_F13;
            case VK_F14: return ImGuiKey_F14;
            case VK_F15: return ImGuiKey_F15;
            case VK_F16: return ImGuiKey_F16;
            case VK_F17: return ImGuiKey_F17;
            case VK_F18: return ImGuiKey_F18;
            case VK_F19: return ImGuiKey_F19;
            case VK_F20: return ImGuiKey_F20;
            case VK_F21: return ImGuiKey_F21;
            case VK_F22: return ImGuiKey_F22;
            case VK_F23: return ImGuiKey_F23;
            case VK_F24: return ImGuiKey_F24;
            case VK_BROWSER_BACK: return ImGuiKey_AppBack;
            case VK_BROWSER_FORWARD: return ImGuiKey_AppForward;
            default: return ImGuiKey_None;
        }
    }
}
namespace pdl
{
    
    void ImGuiRenderer::Initialize(const InitInfo& initInfo)
    {
#ifdef PDL_VULKAN
        m_device = initInfo.m_device;
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        io.DisplaySize.x = static_cast<float>(initInfo.m_window.GetWindowSize().x);
        io.DisplaySize.y = static_cast<float>(initInfo.m_window.GetWindowSize().y);
        // io.Fonts->Build();
        // Setup Platform/Renderer backends
#ifdef PDL_PLATFORM_WINDOWS
        ImGui_ImplWin32_Init(initInfo.m_window.GetNativeWindow());
#endif
#ifdef PDL_VULKAN
        ImGui_ImplVulkan_InitInfo imguiVulkanInitInfo = {};
        static_cast<VulkanDevice*>(initInfo.m_device)->FillImGuiInitInfo(imguiVulkanInitInfo);
        imguiVulkanInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        VkFormat colorAttachmentFormats[] = {
            initInfo.m_useHDR ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM
        };
        imguiVulkanInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = colorAttachmentFormats;
        imguiVulkanInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
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
#endif
    }

    ImGuiRenderer::~ImGuiRenderer()
    {
#ifdef PDL_VULKAN        
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
#endif
    }

    void ImGuiRenderer::BeginFrame()
    {
        #ifdef PDL_VULKAN        
        pdlAssert(m_device && "ImGuiRenderer::BeginFrame: Not initialized");
        ImGui::NewFrame();
#ifdef PDL_PLATFORM_WINDOWS
        ImGui_ImplWin32_NewFrame();
#endif
#ifdef PDL_VULKAN
        ImGui_ImplVulkan_NewFrame();
#endif
#endif
    }

    void ImGuiRenderer::EndFrame()
    {
        ImGui::EndFrame();
    }

    void ImGuiRenderer::Render()
    {
#ifdef PDL_VULKAN        
        CheckHotkeys();
        
        if (m_viewMode != ViewMode::Hidden)
        {
            auto renderables = m_renderables.LockForRead();
            
            if (m_viewMode != ViewMode::NoMenu)
            {
                if (ImGui::BeginMainMenuBar())
                {
                    for (auto renderable : *renderables)
                    {
                        renderable->ImGuiMenuSetup();
                    }
                    ImGui::EndMainMenuBar();
                }
            }
            
            for (auto renderable : *renderables)
            {
                renderable->ImGuiRender();
            }
        }
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), static_cast<VulkanDevice*>(m_device)->GetVulkanDeviceQueue().GetCommandBuffer()->GetVkCommandBuffer());
#endif        
    }

    void ImGuiRenderer::RegisterRenderable(ImGuiRenderable* renderable)
    {
        auto renderables = m_renderables.LockForWrite();
        renderables->push_back(renderable);
    }

    void ImGuiRenderer::UnregisterRenderable(ImGuiRenderable* renderable)
    {
        auto renderables = m_renderables.LockForWrite();
        renderables->erase(std::remove(renderables->begin(), renderables->end(), renderable), renderables->end());
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
        ImGui::GetIO().AddMouseWheelEvent(0, pos);
    }

    void ImGuiRenderer::OnMouseWheelH(float pos)
    {
        ImGui::GetIO().AddMouseWheelEvent(pos, 0);
    }

    void ImGuiRenderer::OnKeyUp(int16 key)
    {
        ImGui::GetIO().AddKeyEvent(Details::VirtualKeyToImGuiKey(key), false);
    }

    void ImGuiRenderer::OnKeyDown(int16 key)
    {
        ImGui::GetIO().AddKeyEvent(Details::VirtualKeyToImGuiKey(key), true);

    }

    void ImGuiRenderer::OnKeyInput(int16 key)
    {
        ImGui::GetIO().AddInputCharacter(key);
    }

    void ImGuiRenderer::CheckHotkeys()
    {
        if (ImGui::IsKeyPressed(ImGuiKey_F11))
        {
            m_viewMode = static_cast<ViewMode>((static_cast<int>(m_viewMode) + 1) % 3);  
        }
    }
}

#endif 