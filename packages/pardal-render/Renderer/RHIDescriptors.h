
#pragma once
#include "Renderer/RendererTypes.h"
#include "Renderer/RenderererConstants.h"
#include "Base/BaseTypes.h"
#include "Containers/Array.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

struct Viewport
{
    float x        = 0.0f;
    float y        = 0.0f;
    float width    = 1.0f;
    float height   = 1.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;
};

struct ScissorRect
{
    uint32 x      = 0;
    uint32 y      = 0;
    uint32 width  = 0;
    uint32 height = 0;
};

struct TextureDesc
{
    TextureType      type           = TextureType::Texture2D;
    Format           format         = Format::R8G8B8A8_UNORM;
    uint32           width          = 1;
    uint32           height         = 1;
    uint32           depth          = 1;
    uint32           numMipLevels   = 1;
    uint32           numLayers      = 1;
    TextureUsage     usage          = TextureUsage::ShaderResource;
    MultiSampleCount numSamples     = MultiSampleCount::One;
    const void*      initialData    = nullptr;
    size_t           initialDataSize = 0;
    const char*      debugName      = "";
};

struct BufferDesc
{
    uint64      size        = 0;
    BufferUsage usage       = BufferUsage::ConstantBuffer;
    MemoryType  storage     = MemoryType::DeviceLocal;
    const void* initialData = nullptr;
    const char* debugName   = "";
};

struct SamplerDesc
{
    TextureFilteringMode  minFilter           = TextureFilteringMode::Linear;
    TextureFilteringMode  magFilter           = TextureFilteringMode::Linear;
    TextureFilteringMode  mipFilter           = TextureFilteringMode::Linear;
    TextureAddressingMode addressU            = TextureAddressingMode::Wrap;
    TextureAddressingMode addressV            = TextureAddressingMode::Wrap;
    TextureAddressingMode addressW            = TextureAddressingMode::Wrap;
    CompareOp             depthCompareOp      = CompareOp::LessOrEqual;
    bool                  depthCompareEnabled = false;
    float                 mipLodMin           = 0.0f;
    float                 mipLodMax           = 15.0f;
    const char*           debugName           = "";
};

struct ShaderModuleDesc
{
    const void* spirvData  = nullptr;
    size_t      spirvSize  = 0;
    const char* debugName  = "";
};

struct VertexAttribute
{
    uint32 location = 0;
    uint32 binding  = 0;
    Format format   = Format::R32G32B32_FLOAT;
    uint32 offset   = 0;
};

struct VertexBinding
{
    uint32 binding = 0;
    uint32 stride  = 0;
};

struct VertexInput
{
    static constexpr uint32 MaxAttributes = 16;
    static constexpr uint32 MaxBindings   = 4;

    Array<VertexAttribute, MaxAttributes> attributes  = {};
    Array<VertexBinding, MaxBindings>     bindings    = {};
    uint32                                numAttributes = 0;
    uint32                                numBindings   = 0;
};

struct ColorAttachmentDesc
{
    Format           format          = Format::R8G8B8A8_UNORM;
    BlendMode        blendMode       = {};
    ColorChannelMask colorWriteMask  = ColorChannelMask::All;
};

struct RenderPipelineDesc
{
    ShaderModuleHandle vertexShader;
    ShaderModuleHandle fragmentShader;

    VertexInput      vertexInput;
    PrimitiveTopology topology    = PrimitiveTopology::TriangleList;

    CullMode    cullMode     = CullMode::None;
    FrontFace   frontFace    = FrontFace::CounterClockwise;
    PolygonMode polygonMode  = PolygonMode::Fill;

    DepthTest   depthTest;
    StencilTest stencilTest;
    DepthBias   depthBias;

    Array<ColorAttachmentDesc, RenderererConstants::MaxColorAttachments()> colorAttachments = {};
    uint32 numColorAttachments = 0;

    Format depthFormat   = Format::Unknown;
    Format stencilFormat = Format::Unknown;

    MultiSampleCount        numSamples                = MultiSampleCount::One;
    ConservativeRasterization conservativeRasterization = ConservativeRasterization::Off;

    const char* debugName = "";
};

struct ComputePipelineDesc
{
    ShaderModuleHandle computeShader;
    const char*        debugName = "";
};

} // namespace pdl
