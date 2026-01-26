
#pragma once
#include "String/String.h"

// Created on 2026-01-26 by sisco

namespace pdl
{
    struct RenderDeviceInfo;
    class ApplicationWindow;

class IRenderer
{
public:
    struct InitInfo
    {
        StringView m_applicationName;
        ApplicationWindow& m_applicationWindow;
        bool m_enableValidation = true;
        bool m_useVSync = true;
        bool m_useHDR = false;
    };
    
    virtual ~IRenderer() = default;
    
    virtual const RenderDeviceInfo& GetDeviceInfo() const = 0;
    
};

}

