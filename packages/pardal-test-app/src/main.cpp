
#include "Application/ApplicationWindow.h"
#include "Renderer/Renderer.h"

int main(int argc, char** argv)
{
    pdl::IApplicationWindow::InitInfoBase info;
    info.m_windowTitle = "pardal-test-app";

    pdl::Renderer renderer;
    renderer.InitializeRenderDevice({
        .deviceType = pdl::RenderDeviceType::Vulkan,
        .applicationName = "pardal-test-app",
        .enableValidation = true
    });
    
    pdl::ApplicationWindow window(info);
    while (!window.IsCloseRequested())
    {
        window.Update();
    }

    
    return 0;
    
}
