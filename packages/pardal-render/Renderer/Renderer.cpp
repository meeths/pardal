#include <Renderer/Renderer.h>

#include <Application/ApplicationWindow.h>
#include <Base/DebugHelpers.h>
#include <Log/Log.h>
#include <Math/Vector3.h>

#include "Shaders/SlangShaderCompiler.h"
#include "Vulkan/VulkanShader.h"

#ifdef PDL_VULKAN
#include <Renderer/Vulkan/VulkanBindlessDescriptors.h>
#include <Renderer/Vulkan/VulkanDevice.h>
#include <Renderer/Vulkan/VulkanInternalRenderer.h>
#endif
// Created on 2025-03-23 by sisco

const char* vert_code = R"(
#version 450

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = {
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
};

vec3 colors[3] ={
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
};

[shader("vertex")]
void vertexmain() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
)";

const char* frag_code = R"(
#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

[shader("fragment")]
void fragmentmain() {
    outColor = vec4(fragColor, 1.0);
}
)";

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
                m_internalRenderer = MakeSharedPointer<VulkanInternalRenderer>(*vulkanDevice.Get());
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
        initInfo.m_applicationWindow.AddResizeCallback([internalRenderer = m_internalRenderer.Get()](Math::Vector2i newSize)
        {
            internalRenderer->OnResize(newSize);
        });

#ifdef PDL_VULKAN
        switch (initInfo.m_deviceType)
        {
        case RenderDeviceType::Vulkan:
            m_bindlessDescriptors = MakeSharedPointer<VulkanBindlessDescriptors>(*static_cast<VulkanDevice*>(m_device.Get()));
            break;
        default:
            pdlNotImplemented();
        }
#endif
        BindlessDescriptors::BindlessDescriptorInfo bindlessDescriptorInfo {};
        m_bindlessDescriptors->Initialize(bindlessDescriptorInfo);
        
#ifdef PDL_FEATURE_IMGUI
        ImGuiRenderer::InitInfo imguiInitInfo
        {
            .m_window = initInfo.m_applicationWindow,
            .m_device = m_device.Get(),
            .m_useHDR = initInfo.m_useHDR
        };
        
        ImGuiRenderer::Instance().Initialize(imguiInitInfo);
#endif
        
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

        SlangShaderCompiler::InitInfo compilerInitInfo = {
            .m_target = SlangShaderCompiler::Target::SPIRV,
            .m_compilerOptions = SlangShaderCompiler::CompilerOptions::TargetVulkan | SlangShaderCompiler::CompilerOptions::EnableGLSLSupport
        };
        SlangShaderCompiler shaderCompiler(compilerInitInfo);

        auto vertShaderResults = shaderCompiler.CompileShader({"shader.vert", vert_code,  "vertexmain"});
        auto fragShaderResults = shaderCompiler.CompileShader({"shader.frag", frag_code,  "fragmentmain"});
        
        IShaderObject::ShaderObjectDescriptor vertexShaderDesc;
        vertexShaderDesc.m_shaderType = pdl::ShaderType::Vertex;
        vertexShaderDesc.m_entryPoint = "vertexmain";
        vertexShaderDesc.m_shaderData = &vertShaderResults.value();
        vertexShaderDesc.m_descriptorSet = GetBindlessDescriptors();
    
        IShaderObject::ShaderObjectDescriptor fragShaderDesc;
        fragShaderDesc.m_shaderType = pdl::ShaderType::Fragment;
        fragShaderDesc.m_entryPoint = "fragmentmain";
        fragShaderDesc.m_shaderData = &fragShaderResults.value();
        fragShaderDesc.m_descriptorSet = GetBindlessDescriptors();
    
        vertexShaderDesc.m_nextShaderObject = &fragShaderDesc;
        
        auto vertexShaderObj = GetRenderDevice()->CreateShaderObject(vertexShaderDesc);
        auto fragShaderObj = GetRenderDevice()->CreateShaderObject(fragShaderDesc);

        m_testShader = GetRenderDevice()->CreateShader({{vertexShaderObj.value().Get(), fragShaderObj.value().Get()}}).value();
        
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
#ifdef PDL_FEATURE_IMGUI
        ImGuiRenderer::Instance().BeginFrame();
#endif
        return m_internalRenderer->BeginFrame();
    }

    bool Renderer::EndFrame()
    {
        bool frameEnded = m_internalRenderer->EndFrame();
#ifdef PDL_FEATURE_IMGUI
        ImGuiRenderer::Instance().EndFrame();
#endif
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
#ifdef PDL_FEATURE_IMGUI
        ImGuiRenderer::Instance().Render();
#endif
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

