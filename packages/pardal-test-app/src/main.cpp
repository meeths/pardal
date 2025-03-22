
#include "Application/ApplicationWindow.h"

int main(int argc, char** argv)
{
    pdl::IApplicationWindow::InitInfoBase info;
    info.m_windowTitle = "pardal-test-app";
    
    pdl::ApplicationWindow window(info);
    while (!window.IsCloseRequested())
    {
        window.Update();
    }
    return 0;
    
}
