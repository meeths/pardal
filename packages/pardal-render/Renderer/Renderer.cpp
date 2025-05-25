#include <Renderer/Renderer.h>

#include <Application/ApplicationWindow.h>
#include <Base/DebugHelpers.h>
#include <Log/Log.h>
#include <Math/Vector3.h>

#ifdef PDL_VULKAN
#include <Renderer/Vulkan/VulkanBindlessDescriptors.h>
#include <Renderer/Vulkan/VulkanDevice.h>
#include <Renderer/Vulkan/VulkanInternalRenderer.h>
#endif
// Created on 2025-03-23 by sisco

const uint32_t triangle_spv[] = 
{
    0x07230203, 0x00010500, 0x00000028, 0x0000003d, 0x00000000, 0x00020011, 0x00000001, 0x0003000e, 0x00000000, 
    0x00000001, 0x0008000f, 0x00000000, 0x00000002, 0x74726576, 0x00007865, 0x0000002d, 0x00000030, 0x00000012, 
    0x0008000f, 0x00000004, 0x00000033, 0x67617266, 0x746e656d, 0x00000000, 0x0000003a, 0x00000037, 0x00030010, 
    0x00000033, 0x00000007, 0x00030003, 0x0000000b, 0x00000001, 0x000a0005, 0x00000030, 0x72746e65, 0x696f5079, 
    0x6150746e, 0x5f6d6172, 0x74726576, 0x432e7865, 0x726f6c6f, 0x00000000, 0x00040005, 0x00000002, 0x74726576, 
    0x00007865, 0x00040005, 0x00000037, 0x6f6c6f63, 0x00000072, 0x00090005, 0x0000003a, 0x72746e65, 0x696f5079, 
    0x6150746e, 0x5f6d6172, 0x67617266, 0x746e656d, 0x00000000, 0x00050005, 0x00000033, 0x67617266, 0x746e656d, 
    0x00000000, 0x00040047, 0x00000005, 0x00000006, 0x00000008, 0x00040047, 0x0000000c, 0x00000006, 0x0000000c, 
    0x00040047, 0x00000012, 0x0000000b, 0x0000002a, 0x00040047, 0x0000002d, 0x0000000b, 0x00000000, 0x00040047, 
    0x00000030, 0x0000001e, 0x00000000, 0x00040047, 0x00000037, 0x0000001e, 0x00000000, 0x00040047, 0x0000003a, 
    0x0000001e, 0x00000000, 0x00020013, 0x00000001, 0x00030021, 0x00000003, 0x00000001, 0x00030016, 0x00000006, 
    0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000002, 0x00040015, 0x00000008, 0x00000020, 0x00000001, 
    0x0004002b, 0x00000008, 0x00000009, 0x00000003, 0x0004001c, 0x00000005, 0x00000007, 0x00000009, 0x00040020, 
    0x0000000a, 0x00000007, 0x00000005, 0x00040017, 0x0000000d, 0x00000006, 0x00000003, 0x0004001c, 0x0000000c, 
    0x0000000d, 0x00000009, 0x00040020, 0x0000000e, 0x00000007, 0x0000000c, 0x00040020, 0x00000011, 0x00000001, 
    0x00000008, 0x00040015, 0x00000013, 0x00000020, 0x00000000, 0x0004002b, 0x00000006, 0x00000017, 0x00000000, 
    0x0004002b, 0x00000006, 0x00000018, 0xbf000000, 0x0005002c, 0x00000007, 0x00000016, 0x00000017, 0x00000018, 
    0x0004002b, 0x00000006, 0x0000001a, 0x3f000000, 0x0005002c, 0x00000007, 0x00000019, 0x0000001a, 0x0000001a, 
    0x0005002c, 0x00000007, 0x0000001b, 0x00000018, 0x0000001a, 0x0006002c, 0x00000005, 0x00000015, 0x00000016, 
    0x00000019, 0x0000001b, 0x00040020, 0x0000001d, 0x00000007, 0x00000007, 0x0004002b, 0x00000006, 0x00000021, 
    0x3f800000, 0x0005002c, 0x00000007, 0x00000020, 0x00000021, 0x00000021, 0x00040017, 0x00000022, 0x00000006, 
    0x00000004, 0x0006002c, 0x0000000d, 0x00000025, 0x00000021, 0x00000017, 0x00000017, 0x0006002c, 0x0000000d, 
    0x00000026, 0x00000017, 0x00000021, 0x00000017, 0x0006002c, 0x0000000d, 0x00000027, 0x00000017, 0x00000017, 
    0x00000021, 0x0006002c, 0x0000000c, 0x00000024, 0x00000025, 0x00000026, 0x00000027, 0x00040020, 0x00000029, 
    0x00000007, 0x0000000d, 0x00040020, 0x0000002c, 0x00000003, 0x00000022, 0x00040020, 0x0000002f, 0x00000003, 
    0x0000000d, 0x00040020, 0x00000036, 0x00000001, 0x0000000d, 0x0004003b, 0x00000011, 0x00000012, 0x00000001, 
    0x0004003b, 0x0000002c, 0x0000002d, 0x00000003, 0x0004003b, 0x0000002f, 0x00000030, 0x00000003, 0x0004003b, 
    0x00000036, 0x00000037, 0x00000001, 0x0004003b, 0x0000002c, 0x0000003a, 0x00000003, 0x00050036, 0x00000001, 
    0x00000002, 0x00000000, 0x00000003, 0x000200f8, 0x00000004, 0x0004003b, 0x0000000a, 0x0000000b, 0x00000007, 
    0x0004003b, 0x0000000e, 0x0000000f, 0x00000007, 0x0004003d, 0x00000008, 0x00000010, 0x00000012, 0x0004007c, 
    0x00000013, 0x00000014, 0x00000010, 0x0003003e, 0x0000000b, 0x00000015, 0x00050041, 0x0000001d, 0x0000001e, 
    0x0000000b, 0x00000014, 0x0004003d, 0x00000007, 0x0000001f, 0x0000001e, 0x00050050, 0x00000022, 0x00000023, 
    0x0000001f, 0x00000020, 0x0003003e, 0x0000000f, 0x00000024, 0x00050041, 0x00000029, 0x0000002a, 0x0000000f, 
    0x00000014, 0x0004003d, 0x0000000d, 0x0000002b, 0x0000002a, 0x0003003e, 0x0000002d, 0x00000023, 0x0003003e, 
    0x00000030, 0x0000002b, 0x000100fd, 0x00010038, 0x00050036, 0x00000001, 0x00000033, 0x00000000, 0x00000003, 
    0x000200f8, 0x00000034, 0x0004003d, 0x0000000d, 0x00000035, 0x00000037, 0x0008004f, 0x0000000d, 0x00000038, 
    0x00000035, 0x00000035, 0x00000000, 0x00000001, 0x00000002, 0x00050050, 0x00000022, 0x00000039, 0x00000038, 
    0x00000021, 0x0003003e, 0x0000003a, 0x00000039, 0x000100fd, 0x00010038, 
};

const size_t triangle_spv_sizeInBytes = 1356;
namespace pdl
{
    class RenderPass;

    bool Renderer::InitializeRenderDevice(const IRenderDevice::InitInfoBase& initInfo)
    {
        switch (initInfo.m_deviceType)
        {
#ifdef PDL_VULKAN
        case RenderDeviceType::Vulkan:
            {
                auto vulkanDevice = MakeSharedPointer<VulkanDevice>(); 
                m_internalRenderer = MakeSharedPointer<VulkanInternalRenderer>(*vulkanDevice.get());
                m_device = vulkanDevice;
            }
            break;
#endif            
        default: ;
            pdlLogError("Renderer::InitializeRenderDevice: Unknown device type <%s>", to_string(initInfo.m_deviceType));
            return false;
            break;
        }

        if(!m_device->Initialize(initInfo))
        {
            return false;
        }

        IInternalRenderer::InitInfo internalRendererInitInfo
        {
            .m_window = initInfo.m_applicationWindow,
            .m_initialSurfaceSize = initInfo.m_applicationWindow.GetWindowSize(),
            .m_useVSync = initInfo.m_useVSync,
            .m_useHDR = initInfo.m_useHDR,
            .m_createDepthBuffer = true,
        };
        m_internalRenderer->Initialize(internalRendererInitInfo);
        initInfo.m_applicationWindow.AddResizeCallback([internalRenderer = m_internalRenderer.get()](Math::Vector2i newSize)
        {
            internalRenderer->OnResize(newSize);
        });

#ifdef PDL_VULKAN
        switch (initInfo.m_deviceType)
        {
        case RenderDeviceType::Vulkan:
            m_bindlessDescriptors = MakeSharedPointer<VulkanBindlessDescriptors>(*static_cast<VulkanDevice*>(m_device.get()));
            break;
        default:
            pdlNotImplemented();
        }
#endif
        BindlessDescriptors::BindlessDescriptorInfo bindlessDescriptorInfo {};
        m_bindlessDescriptors->Initialize(bindlessDescriptorInfo);
        
        ImGuiRenderer::InitInfo imguiInitInfo
        {
            .m_window = initInfo.m_applicationWindow,
            .m_device = m_device.get(),
            .m_useHDR = initInfo.m_useHDR
        };
        
        m_imguiRenderer = MakeSharedPointer<ImGuiRenderer>(imguiInitInfo);

        m_frameInfo.eyePos = Math::Vector3(0.0f, 0.0f, 0.0f);
        m_frameInfo.projection = GetRenderDevice()->GetRenderDeviceInfo().identityProjection;
        m_frameInfo.view = glm::identity<Math::Matrix44>();

        pdl::IRenderBuffer::BufferDescriptor frameInfoBufferDescriptor;
        frameInfoBufferDescriptor.size = sizeof(PerFrameInfo);
        frameInfoBufferDescriptor.usage = pdl::BufferUsage::ShaderResource;
        frameInfoBufferDescriptor.memoryType = pdl::MemoryType::Upload;

        auto frameInfoBuffer = GetRenderDevice()->CreateRenderBuffer(frameInfoBufferDescriptor);
        if (!frameInfoBuffer)
        {
            pdlLogError("Failed to create frame info buffer: %s", frameInfoBuffer.error().data());
            return false;
        }
        m_frameInfoBuffer = frameInfoBuffer.value();
        UpdateFrameInfoBuffer();

        pdl::Vector<uint8> triangleSpv(triangle_spv_sizeInBytes);
        uint8* triangleSpvData = (uint8*)triangle_spv;
        memcpy(triangleSpv.data(), triangleSpvData, triangleSpv.size());

        pdl::IShaderObject::ShaderObjectDescriptor vertexShaderDesc;
        vertexShaderDesc.m_shaderType = pdl::ShaderType::Vertex;
        vertexShaderDesc.m_entryPoint = "vertex";
        vertexShaderDesc.m_shaderData = &triangleSpv;
        vertexShaderDesc.m_descriptorSet = GetBindlessDescriptors();
    
        pdl::IShaderObject::ShaderObjectDescriptor fragShaderDesc;
        fragShaderDesc.m_shaderType = pdl::ShaderType::Fragment;
        fragShaderDesc.m_entryPoint = "fragment";
        fragShaderDesc.m_shaderData = &triangleSpv;
        fragShaderDesc.m_descriptorSet = GetBindlessDescriptors();
    
        auto vertexShaderObj = GetRenderDevice()->CreateShaderObject(vertexShaderDesc);
        auto fragShaderObj = GetRenderDevice()->CreateShaderObject(fragShaderDesc);

        m_testShader = GetRenderDevice()->CreateShader({{vertexShaderObj.value().get(), fragShaderObj.value().get()}}).value();
        
        return true;
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::Update(float deltaTime)
    {
        m_frameInfo.deltaTime = deltaTime;
        m_frameInfo.totalTime += deltaTime;
        ++m_frameInfo.frameIndex;
    }

    bool Renderer::BeginFrame()
    {
        UpdateFrameInfoBuffer();
        m_bindlessDescriptors->WriteDescriptors();
        m_imguiRenderer->BeginFrame();
        return m_internalRenderer->BeginFrame();
    }

    bool Renderer::EndFrame()
    {
        bool frameEnded = m_internalRenderer->EndFrame();
        m_imguiRenderer->EndFrame();
        return frameEnded;
    }

    RenderPass& Renderer::GetMainRenderPass()
    {
        return m_internalRenderer->GetMainRenderPass();
    }

    bool Renderer::BeginRenderPass(const RenderPass& renderPass)
    {
        auto passStarted =  m_internalRenderer->BeginRenderPass(renderPass);
        m_testShader->Bind(m_internalRenderer->GetCommandBuffer());

        return passStarted;
    }
    
    bool Renderer::EndRenderPass()
    {
        m_imguiRenderer->Render();
        return m_internalRenderer->EndRenderPass();
    }

    void Renderer::SetPipelineState(const PipelineState& pipelineState)
    {
        auto cmd = m_internalRenderer->GetCommandBuffer();
        pdlAssert(cmd && "No available command buffer to set pipeline state");
        m_internalRenderer->SetPipelineState(m_internalRenderer->GetCommandBuffer(), pipelineState);
    }

    void Renderer::SetViewport(const Math::Rectangle& viewport)
    {
        auto cmd = m_internalRenderer->GetCommandBuffer();
        pdlAssert(cmd && "No available command buffer to set viewport");
        m_internalRenderer->SetViewport(cmd, viewport);
    }

    void Renderer::SetScissor(const Math::Rectanglei& scissor)
    {
        auto cmd = m_internalRenderer->GetCommandBuffer();
        pdlAssert(cmd && "No available command buffer to set scissor");
        m_internalRenderer->SetScissor(cmd, scissor);
    }

    void Renderer::UpdateFrameInfoBuffer()
    {
        pdlAssert(m_frameInfoBuffer);
        PerFrameInfo* frameInfo = reinterpret_cast<PerFrameInfo*>(m_frameInfoBuffer->Map());
        memcpy(frameInfo, &m_frameInfo, sizeof(PerFrameInfo));
        m_frameInfoBuffer->Unmap();
    }
}

