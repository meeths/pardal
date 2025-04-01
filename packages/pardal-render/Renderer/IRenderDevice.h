
#pragma once
#include <String/String.h>
#include <Renderer/RenderDeviceInfo.h>
#include <Base/Expected.h>
#include <Memory/SharedPointer.h>
#include <Renderer/IRenderBuffer.h>
#include <Renderer/ITexture.h>
#include <Renderer/ITextureView.h>

// Created on 2025-03-23 by sisco


namespace pdl
{
class ApplicationWindow;
class ISurface;

class IRenderDevice
{
    friend class Renderer;
public:
   
    struct InitInfoBase
    {
        RenderDeviceType m_deviceType = RenderDeviceType::None;
        StringView m_applicationName;
        ApplicationWindow& m_applicationWindow;
        bool m_enableValidation = true;
        bool m_useVSync = true;
        bool m_useHDR = false;
    };

    virtual ~IRenderDevice() = default;
    virtual const RenderDeviceInfo& GetRenderDeviceInfo() const = 0;

    virtual void WaitForGPU() = 0;

    virtual Expected<SharedPointer<ISurface>,StringView> CreateSurface(ApplicationWindow& applicationWindow) = 0;
    virtual Expected<SharedPointer<ITexture>,StringView> CreateTexture(ITexture::TextureDescriptor _textureDescriptor) = 0;
    virtual Expected<SharedPointer<ITextureView>,StringView> CreateTextureView(ITextureView::TextureViewDescriptor _textureDescriptor) = 0;
    virtual Expected<SharedPointer<IRenderBuffer>, StringView> CreateRenderBuffer(IRenderBuffer::BufferDescriptor _bufferDescriptor) = 0;

protected:
    virtual bool Initialize(const InitInfoBase& desc) = 0;
};

}

