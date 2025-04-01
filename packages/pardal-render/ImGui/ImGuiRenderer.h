
#pragma once
#include "Base/BaseTypes.h"
#include "Math/Vector2.h"

// Created on 2025-04-01 by sisco

namespace pdl
{
class IRenderDevice;
class ApplicationWindow;

class ImGuiRenderer
{
public:
    struct InitInfo
    {
        ApplicationWindow& m_window;
        IRenderDevice* m_device;
        bool m_useHDR = false;
    };
    ImGuiRenderer(const InitInfo& initInfo);
    ~ImGuiRenderer();
    
    void BeginFrame();
    void EndFrame();
    void Render();

    
private:

    void OnMouseMove(Math::Vector2 pos, bool lButton, bool rButton, bool mButton, unsigned int mods);
    void OnMouseWheelV(float pos);
    void OnMouseWheelH(float pos);
    void OnKeyUp(int16 key);
    void OnKeyDown(int16 key);
    void OnKeyInput(int16 key);

    
    IRenderDevice* m_device;
};

}

