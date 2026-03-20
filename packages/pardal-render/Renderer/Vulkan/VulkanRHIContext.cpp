#include "Renderer/Vulkan/VulkanRHIContext.h"
#include "Renderer/RenderererDevices.h"
#include "Renderer/Vulkan/lvk/vulkan/VulkanClasses.h"
#include "Log/Log.h"

// Created on 2026-03-20 by sisco

namespace
{

// ---------------------------------------------------------------------------
// Format translation
// ---------------------------------------------------------------------------

lvk::Format toFormatLvk(pdl::Format format)
{
    switch (format)
    {
    case pdl::Format::R8_UNORM:             return lvk::Format_R_UN8;
    case pdl::Format::R16_UINT:             return lvk::Format_R_UI16;
    case pdl::Format::R32_UINT:             return lvk::Format_R_UI32;
    case pdl::Format::R16_UNORM:            return lvk::Format_R_UN16;
    case pdl::Format::R16_FLOAT:            return lvk::Format_R_F16;
    case pdl::Format::R32_FLOAT:            return lvk::Format_R_F32;
    case pdl::Format::R8G8_UNORM:           return lvk::Format_RG_UN8;
    case pdl::Format::R16G16_UINT:          return lvk::Format_RG_UI16;
    case pdl::Format::R32G32_UINT:          return lvk::Format_RG_UI32;
    case pdl::Format::R16G16_UNORM:         return lvk::Format_RG_UN16;
    case pdl::Format::R16G16_FLOAT:         return lvk::Format_RG_F16;
    case pdl::Format::R32G32_FLOAT:         return lvk::Format_RG_F32;
    case pdl::Format::R8G8B8A8_UNORM:       return lvk::Format_RGBA_UN8;
    case pdl::Format::R32G32B32A32_UINT:    return lvk::Format_RGBA_UI32;
    case pdl::Format::R16G16B16A16_FLOAT:   return lvk::Format_RGBA_F16;
    case pdl::Format::R32G32B32A32_FLOAT:   return lvk::Format_RGBA_F32;
    case pdl::Format::R8G8B8A8_UNORM_SRGB:  return lvk::Format_RGBA_SRGB8;
    case pdl::Format::B8G8R8A8_UNORM:       return lvk::Format_BGRA_UN8;
    case pdl::Format::B8G8R8A8_UNORM_SRGB:  return lvk::Format_BGRA_SRGB8;
    case pdl::Format::R10G10B10A2_UNORM:    return lvk::Format_A2B10G10R10_UN;
    case pdl::Format::BC7_UNORM:            return lvk::Format_BC7_RGBA;
    case pdl::Format::D16_UNORM:            return lvk::Format_Z_UN16;
    case pdl::Format::D32_FLOAT:            return lvk::Format_Z_F32;
    case pdl::Format::D32_FLOAT_S8_UINT:    return lvk::Format_Z_F32_S_UI8;
    default:                                return lvk::Format_Invalid;
    }
}

pdl::Format fromFormatLvk(lvk::Format format)
{
    switch (format)
    {
    case lvk::Format_R_UN8:          return pdl::Format::R8_UNORM;
    case lvk::Format_R_UI16:         return pdl::Format::R16_UINT;
    case lvk::Format_R_UI32:         return pdl::Format::R32_UINT;
    case lvk::Format_R_UN16:         return pdl::Format::R16_UNORM;
    case lvk::Format_R_F16:          return pdl::Format::R16_FLOAT;
    case lvk::Format_R_F32:          return pdl::Format::R32_FLOAT;
    case lvk::Format_RG_UN8:         return pdl::Format::R8G8_UNORM;
    case lvk::Format_RG_UI16:        return pdl::Format::R16G16_UINT;
    case lvk::Format_RG_UI32:        return pdl::Format::R32G32_UINT;
    case lvk::Format_RG_UN16:        return pdl::Format::R16G16_UNORM;
    case lvk::Format_RG_F16:         return pdl::Format::R16G16_FLOAT;
    case lvk::Format_RG_F32:         return pdl::Format::R32G32_FLOAT;
    case lvk::Format_RGBA_UN8:       return pdl::Format::R8G8B8A8_UNORM;
    case lvk::Format_RGBA_UI32:      return pdl::Format::R32G32B32A32_UINT;
    case lvk::Format_RGBA_F16:       return pdl::Format::R16G16B16A16_FLOAT;
    case lvk::Format_RGBA_F32:       return pdl::Format::R32G32B32A32_FLOAT;
    case lvk::Format_RGBA_SRGB8:     return pdl::Format::R8G8B8A8_UNORM_SRGB;
    case lvk::Format_BGRA_UN8:       return pdl::Format::B8G8R8A8_UNORM;
    case lvk::Format_BGRA_SRGB8:     return pdl::Format::B8G8R8A8_UNORM_SRGB;
    case lvk::Format_A2B10G10R10_UN: return pdl::Format::R10G10B10A2_UNORM;
    case lvk::Format_BC7_RGBA:       return pdl::Format::BC7_UNORM;
    case lvk::Format_Z_UN16:         return pdl::Format::D16_UNORM;
    case lvk::Format_Z_F32:          return pdl::Format::D32_FLOAT;
    case lvk::Format_Z_F32_S_UI8:    return pdl::Format::D32_FLOAT_S8_UINT;
    default:                         return pdl::Format::Unknown;
    }
}

lvk::VertexFormat toVertexFormatLvk(pdl::Format format)
{
    switch (format)
    {
    case pdl::Format::R32_FLOAT:            return lvk::VertexFormat::Float1;
    case pdl::Format::R32G32_FLOAT:         return lvk::VertexFormat::Float2;
    case pdl::Format::R32G32B32_FLOAT:      return lvk::VertexFormat::Float3;
    case pdl::Format::R32G32B32A32_FLOAT:   return lvk::VertexFormat::Float4;
    case pdl::Format::R8G8B8A8_UNORM:       return lvk::VertexFormat::UByte4Norm;
    case pdl::Format::R8G8B8A8_UINT:        return lvk::VertexFormat::UByte4;
    case pdl::Format::R8G8B8A8_SNORM:       return lvk::VertexFormat::Byte4Norm;
    case pdl::Format::R8G8B8A8_SINT:        return lvk::VertexFormat::Byte4;
    case pdl::Format::R16G16_FLOAT:         return lvk::VertexFormat::HalfFloat2;
    case pdl::Format::R16G16B16A16_FLOAT:   return lvk::VertexFormat::HalfFloat4;
    case pdl::Format::R16G16_UNORM:         return lvk::VertexFormat::UShort2Norm;
    case pdl::Format::R16G16B16A16_UNORM:   return lvk::VertexFormat::UShort4Norm;
    case pdl::Format::R32_UINT:             return lvk::VertexFormat::UInt1;
    case pdl::Format::R32G32_UINT:          return lvk::VertexFormat::UInt2;
    case pdl::Format::R32G32B32_UINT:       return lvk::VertexFormat::UInt3;
    case pdl::Format::R32G32B32A32_UINT:    return lvk::VertexFormat::UInt4;
    case pdl::Format::R32_SINT:             return lvk::VertexFormat::Int1;
    case pdl::Format::R32G32_SINT:          return lvk::VertexFormat::Int2;
    case pdl::Format::R32G32B32_SINT:       return lvk::VertexFormat::Int3;
    case pdl::Format::R32G32B32A32_SINT:    return lvk::VertexFormat::Int4;
    default:                                return lvk::VertexFormat::Invalid;
    }
}

// ---------------------------------------------------------------------------
// Enum translation
// ---------------------------------------------------------------------------

lvk::StorageType toStorageTypeLvk(pdl::MemoryType t)
{
    switch (t)
    {
    case pdl::MemoryType::DeviceLocal:  return lvk::StorageType_Device;
    case pdl::MemoryType::HostVisible:  return lvk::StorageType_HostVisible;
    case pdl::MemoryType::MemoryLess:   return lvk::StorageType_Memoryless;
    default:                            return lvk::StorageType_Device;
    }
}

uint8_t toBufferUsageLvk(pdl::BufferUsage usage)
{
    uint8_t result = 0;
    if (!!(usage & pdl::BufferUsage::IndexBuffer))                     result |= lvk::BufferUsageBits_Index;
    if (!!(usage & pdl::BufferUsage::VertexBuffer))                    result |= lvk::BufferUsageBits_Vertex;
    if (!!(usage & pdl::BufferUsage::ConstantBuffer))                  result |= lvk::BufferUsageBits_Uniform;
    if (!!(usage & pdl::BufferUsage::ShaderResource))                  result |= lvk::BufferUsageBits_Storage;
    if (!!(usage & pdl::BufferUsage::UnorderedAccess))                 result |= lvk::BufferUsageBits_Storage;
    if (!!(usage & pdl::BufferUsage::IndirectArgument))                result |= lvk::BufferUsageBits_Indirect;
    if (!!(usage & pdl::BufferUsage::AccelerationStructure))           result |= lvk::BufferUsageBits_AccelStructStorage;
    if (!!(usage & pdl::BufferUsage::AccelerationStructureBuildInput)) result |= lvk::BufferUsageBits_AccelStructBuildInputReadOnly;
    if (!!(usage & pdl::BufferUsage::ShaderTable))                     result |= lvk::BufferUsageBits_ShaderBindingTable;
    return result;
}

uint8_t toTextureUsageLvk(pdl::TextureUsage usage)
{
    uint8_t result = 0;
    if (!!(usage & pdl::TextureUsage::ShaderResource))  result |= lvk::TextureUsageBits_Sampled;
    if (!!(usage & pdl::TextureUsage::UnorderedAccess)) result |= lvk::TextureUsageBits_Storage;
    if (!!(usage & pdl::TextureUsage::RenderTarget))    result |= lvk::TextureUsageBits_Attachment;
    if (!!(usage & pdl::TextureUsage::DepthWrite))      result |= lvk::TextureUsageBits_Attachment;
    if (!!(usage & pdl::TextureUsage::DepthRead))       result |= lvk::TextureUsageBits_Sampled;
    return result;
}

lvk::TextureType toTextureTypeLvk(pdl::TextureType t)
{
    switch (t)
    {
    case pdl::TextureType::Texture2D:   return lvk::TextureType_2D;
    case pdl::TextureType::Texture3D:   return lvk::TextureType_3D;
    case pdl::TextureType::TextureCube: return lvk::TextureType_Cube;
    default:                            return lvk::TextureType_2D;
    }
}

lvk::SamplerFilter toSamplerFilterLvk(pdl::TextureFilteringMode m)
{
    return m == pdl::TextureFilteringMode::Point ? lvk::SamplerFilter_Nearest : lvk::SamplerFilter_Linear;
}

lvk::SamplerMip toSamplerMipLvk(pdl::TextureFilteringMode m)
{
    return m == pdl::TextureFilteringMode::Point ? lvk::SamplerMip_Nearest : lvk::SamplerMip_Linear;
}

lvk::SamplerWrap toSamplerWrapLvk(pdl::TextureAddressingMode m)
{
    switch (m)
    {
    case pdl::TextureAddressingMode::Wrap:           return lvk::SamplerWrap_Repeat;
    case pdl::TextureAddressingMode::ClampToEdge:    return lvk::SamplerWrap_Clamp;
    case pdl::TextureAddressingMode::ClampToBorder:  return lvk::SamplerWrap_ClampToBorder;
    case pdl::TextureAddressingMode::MirrorRepeat:   return lvk::SamplerWrap_MirrorRepeat;
    case pdl::TextureAddressingMode::MirrorOnce:     return lvk::SamplerWrap_MirrorClampToEdge;
    default:                                         return lvk::SamplerWrap_Repeat;
    }
}

lvk::CompareOp toCompareOpLvk(pdl::CompareOp op)
{
    switch (op)
    {
    case pdl::CompareOp::Never:          return lvk::CompareOp_Never;
    case pdl::CompareOp::Less:           return lvk::CompareOp_Less;
    case pdl::CompareOp::Equal:          return lvk::CompareOp_Equal;
    case pdl::CompareOp::LessOrEqual:    return lvk::CompareOp_LessEqual;
    case pdl::CompareOp::Greater:        return lvk::CompareOp_Greater;
    case pdl::CompareOp::NotEqual:       return lvk::CompareOp_NotEqual;
    case pdl::CompareOp::GreaterOrEqual: return lvk::CompareOp_GreaterEqual;
    case pdl::CompareOp::Always:         return lvk::CompareOp_AlwaysPass;
    default:                             return lvk::CompareOp_AlwaysPass;
    }
}

lvk::StencilOp toStencilOpLvk(pdl::StencilTest::StencilOp op)
{
    switch (op)
    {
    case pdl::StencilTest::StencilOp::Keep:           return lvk::StencilOp_Keep;
    case pdl::StencilTest::StencilOp::Zero:           return lvk::StencilOp_Zero;
    case pdl::StencilTest::StencilOp::Replace:        return lvk::StencilOp_Replace;
    case pdl::StencilTest::StencilOp::IncrementClamp: return lvk::StencilOp_IncrementClamp;
    case pdl::StencilTest::StencilOp::DecrementClamp: return lvk::StencilOp_DecrementClamp;
    case pdl::StencilTest::StencilOp::Invert:         return lvk::StencilOp_Invert;
    case pdl::StencilTest::StencilOp::IncrementWrap:  return lvk::StencilOp_IncrementWrap;
    case pdl::StencilTest::StencilOp::DecrementWrap:  return lvk::StencilOp_DecrementWrap;
    default:                                          return lvk::StencilOp_Keep;
    }
}

lvk::BlendOp toBlendOpLvk(pdl::BlendMode::BlendOp op)
{
    switch (op)
    {
    case pdl::BlendMode::BlendOp::Add:             return lvk::BlendOp_Add;
    case pdl::BlendMode::BlendOp::Subtract:        return lvk::BlendOp_Subtract;
    case pdl::BlendMode::BlendOp::ReverseSubtract: return lvk::BlendOp_ReverseSubtract;
    case pdl::BlendMode::BlendOp::Min:             return lvk::BlendOp_Min;
    case pdl::BlendMode::BlendOp::Max:             return lvk::BlendOp_Max;
    default:                                       return lvk::BlendOp_Add;
    }
}

lvk::BlendFactor toBlendFactorLvk(pdl::BlendMode::BlendFactor f)
{
    switch (f)
    {
    case pdl::BlendMode::BlendFactor::Zero:             return lvk::BlendFactor_Zero;
    case pdl::BlendMode::BlendFactor::One:              return lvk::BlendFactor_One;
    case pdl::BlendMode::BlendFactor::SrcColor:         return lvk::BlendFactor_SrcColor;
    case pdl::BlendMode::BlendFactor::OneMinusSrcColor: return lvk::BlendFactor_OneMinusSrcColor;
    case pdl::BlendMode::BlendFactor::DstColor:         return lvk::BlendFactor_DstColor;
    case pdl::BlendMode::BlendFactor::OneMinusDstColor: return lvk::BlendFactor_OneMinusDstColor;
    case pdl::BlendMode::BlendFactor::SrcAlpha:         return lvk::BlendFactor_SrcAlpha;
    default:                                            return lvk::BlendFactor_One;
    }
}

lvk::Topology toTopologyLvk(pdl::PrimitiveTopology t)
{
    switch (t)
    {
    case pdl::PrimitiveTopology::PointList:     return lvk::Topology_Point;
    case pdl::PrimitiveTopology::LineList:      return lvk::Topology_Line;
    case pdl::PrimitiveTopology::LineStrip:     return lvk::Topology_LineStrip;
    case pdl::PrimitiveTopology::TriangleList:  return lvk::Topology_Triangle;
    case pdl::PrimitiveTopology::TriangleStrip: return lvk::Topology_TriangleStrip;
    case pdl::PrimitiveTopology::PatchList:     return lvk::Topology_Patch;
    default:                                    return lvk::Topology_Triangle;
    }
}

lvk::CullMode toCullModeLvk(pdl::CullMode m)
{
    switch (m)
    {
    case pdl::CullMode::None:  return lvk::CullMode_None;
    case pdl::CullMode::Front: return lvk::CullMode_Front;
    case pdl::CullMode::Back:  return lvk::CullMode_Back;
    default:                   return lvk::CullMode_None;
    }
}

lvk::WindingMode toWindingModeLvk(pdl::FrontFace f)
{
    return f == pdl::FrontFace::Clockwise ? lvk::WindingMode_CW : lvk::WindingMode_CCW;
}

lvk::PolygonMode toPolygonModeLvk(pdl::PolygonMode m)
{
    switch (m)
    {
    case pdl::PolygonMode::Fill:  return lvk::PolygonMode_Fill;
    case pdl::PolygonMode::Line:  return lvk::PolygonMode_Line;
    case pdl::PolygonMode::Point: return lvk::PolygonMode_Point;
    default:                      return lvk::PolygonMode_Fill;
    }
}

uint32_t toSampleCountLvk(pdl::MultiSampleCount s)
{
    switch (s)
    {
    case pdl::MultiSampleCount::None:      return 1;
    case pdl::MultiSampleCount::One:       return 1;
    case pdl::MultiSampleCount::Two:       return 2;
    case pdl::MultiSampleCount::Four:      return 4;
    case pdl::MultiSampleCount::Eight:     return 8;
    case pdl::MultiSampleCount::Sixteen:   return 16;
    case pdl::MultiSampleCount::ThirtyTwo: return 32;
    case pdl::MultiSampleCount::SixtyFour: return 64;
    default:                               return 1;
    }
}

lvk::LoadOp toLoadOpLvk(pdl::LoadOp op)
{
    switch (op)
    {
    case pdl::LoadOp::Invalid:  return lvk::LoadOp_Invalid;
    case pdl::LoadOp::DontCare: return lvk::LoadOp_DontCare;
    case pdl::LoadOp::Load:     return lvk::LoadOp_Load;
    case pdl::LoadOp::Clear:    return lvk::LoadOp_Clear;
    case pdl::LoadOp::None:     return lvk::LoadOp_None;
    default:                    return lvk::LoadOp_Invalid;
    }
}

lvk::StoreOp toStoreOpLvk(pdl::StoreOp op)
{
    switch (op)
    {
    case pdl::StoreOp::DontCare:    return lvk::StoreOp_DontCare;
    case pdl::StoreOp::Store:       return lvk::StoreOp_Store;
    case pdl::StoreOp::MsaaResolve: return lvk::StoreOp_MsaaResolve;
    case pdl::StoreOp::None:        return lvk::StoreOp_None;
    default:                        return lvk::StoreOp_DontCare;
    }
}

lvk::ResolveMode toResolveModeLvk(pdl::ResolveMode m)
{
    switch (m)
    {
    case pdl::ResolveMode::None:        return lvk::ResolveMode_None;
    case pdl::ResolveMode::SampleZero:  return lvk::ResolveMode_SampleZero;
    case pdl::ResolveMode::Average:     return lvk::ResolveMode_Average;
    case pdl::ResolveMode::Min:         return lvk::ResolveMode_Min;
    case pdl::ResolveMode::Max:         return lvk::ResolveMode_Max;
    default:                            return lvk::ResolveMode_Average;
    }
}

lvk::IndexFormat toIndexFormatLvk(pdl::IndexFormat f)
{
    return f == pdl::IndexFormat::UInt16 ? lvk::IndexFormat_UI16 : lvk::IndexFormat_UI32;
}

// ---------------------------------------------------------------------------
// RenderPass translation (no handle lookups needed)
// ---------------------------------------------------------------------------

lvk::RenderPass toRenderPassLvk(const pdl::RenderPass& rp)
{
    lvk::RenderPass lvkRp;

    const uint32_t numColor = rp.GetNumColorAttachments();
    for (uint32_t i = 0; i < numColor; ++i)
    {
        const auto& src = rp.m_colorAttachments[i];
        auto& dst       = lvkRp.color[i];
        dst.loadOp      = toLoadOpLvk(src.loadOp);
        dst.storeOp     = toStoreOpLvk(src.storeOp);
        dst.resolveMode = toResolveModeLvk(src.resolveMode);
        dst.layer       = src.layer;
        dst.level       = src.level;
        dst.clearColor.float32[0] = src.clearColor.x;
        dst.clearColor.float32[1] = src.clearColor.y;
        dst.clearColor.float32[2] = src.clearColor.z;
        dst.clearColor.float32[3] = src.clearColor.w;
    }

    if (rp.m_depthAttachment.loadOp != pdl::LoadOp::Invalid)
    {
        lvkRp.depth.loadOp      = toLoadOpLvk(rp.m_depthAttachment.loadOp);
        lvkRp.depth.storeOp     = toStoreOpLvk(rp.m_depthAttachment.storeOp);
        lvkRp.depth.clearDepth  = rp.m_depthAttachment.clearDepth;
        lvkRp.depth.layer       = rp.m_depthAttachment.layer;
        lvkRp.depth.level       = rp.m_depthAttachment.level;
    }

    if (rp.m_stencilAttachment.loadOp != pdl::LoadOp::Invalid)
    {
        lvkRp.stencil.loadOp        = toLoadOpLvk(rp.m_stencilAttachment.loadOp);
        lvkRp.stencil.storeOp       = toStoreOpLvk(rp.m_stencilAttachment.storeOp);
        lvkRp.stencil.clearStencil  = rp.m_stencilAttachment.clearStencil;
    }

    return lvkRp;
}

// ---------------------------------------------------------------------------
// lvk::Result → Expected
// ---------------------------------------------------------------------------

pdl::Expected<void, pdl::String> fromLvkResult(lvk::Result result)
{
    if (result.isOk())
        return {};
    return pdl::Unexpected<pdl::String>(result.message);
}

} // anonymous namespace

// ===========================================================================
// VulkanRHIContext
// ===========================================================================

namespace pdl
{

VulkanRHIContext::VulkanRHIContext(lvk::IContext* lvkCtx)
    : m_lvkCtx(lvkCtx)
{
    m_commandBuffer.m_owner = this;
    m_deviceInfo.deviceType = RenderDeviceType::Vulkan;
}

VulkanRHIContext::~VulkanRHIContext() = default;

UniquePointer<VulkanRHIContext> VulkanRHIContext::CreateHeadless(bool enableValidation)
{
    lvk::ContextConfig cfg;
    cfg.enableValidation      = enableValidation;
    cfg.enableHeadlessSurface = true;

    auto lvkCtx = MakeUniquePointer<lvk::VulkanContext>(cfg, nullptr);
    if (!lvkCtx)
        return nullptr;

    lvk::HWDeviceDesc devices[4];
    const uint32_t numDevices = lvkCtx->queryDevices(devices, 4);
    if (!numDevices)
        return nullptr;

    int selected = 0;
    for (uint32_t i = 0; i < numDevices; ++i)
    {
        if (devices[i].type == lvk::HWDeviceType_Discrete)
        {
            selected = static_cast<int>(i);
            break;
        }
    }

    lvk::Result res = lvkCtx->initContext(devices[selected]);
    if (!res.isOk())
        return nullptr;

    auto rhi = MakeUniquePointer<VulkanRHIContext>(lvkCtx.get());
    rhi->m_ownedLvkCtx = std::move(lvkCtx);
    return rhi;
}

lvk::ICommandBuffer& VulkanRHIContext::GetLVKCommandBuffer(IRHICommandBuffer& cmd)
{
    return *static_cast<VulkanRHICommandBuffer&>(cmd).m_lvkCmdBuf;
}

// ---------------------------------------------------------------------------
// Private lookup helpers — RHI handle → lvk handle via pool
// ---------------------------------------------------------------------------

lvk::TextureHandle VulkanRHIContext::ToLvk(TextureHandle h) const
{
    const lvk::TextureHandle* p = m_textures.Get(h);
    pdlAssert(p);
    return p ? *p : lvk::TextureHandle{};
}

lvk::BufferHandle VulkanRHIContext::ToLvk(BufferHandle h) const
{
    const lvk::BufferHandle* p = m_buffers.Get(h);
    pdlAssert(p);
    return p ? *p : lvk::BufferHandle{};
}

lvk::SamplerHandle VulkanRHIContext::ToLvk(SamplerHandle h) const
{
    const lvk::SamplerHandle* p = m_samplers.Get(h);
    pdlAssert(p);
    return p ? *p : lvk::SamplerHandle{};
}

lvk::ShaderModuleHandle VulkanRHIContext::ToLvk(ShaderModuleHandle h) const
{
    const lvk::ShaderModuleHandle* p = m_shaderModules.Get(h);
    pdlAssert(p);
    return p ? *p : lvk::ShaderModuleHandle{};
}

lvk::RenderPipelineHandle VulkanRHIContext::ToLvk(RenderPipelineHandle h) const
{
    const RenderPipelineEntry* p = m_renderPipelines.Get(h);
    pdlAssert(p);
    return p ? p->lvkHandle : lvk::RenderPipelineHandle{};
}

lvk::ComputePipelineHandle VulkanRHIContext::ToLvk(ComputePipelineHandle h) const
{
    const lvk::ComputePipelineHandle* p = m_computePipelines.Get(h);
    pdlAssert(p);
    return p ? *p : lvk::ComputePipelineHandle{};
}

lvk::TextureHandle VulkanRHIContext::GetLVKTexture(TextureHandle h) const
{
    return ToLvk(h);
}

lvk::Framebuffer VulkanRHIContext::ToFramebufferLvk(const Framebuffer& fb) const
{
    lvk::Framebuffer lvkFb;
    const uint32_t numColor = fb.GetNumColorAttachments();
    for (uint32_t i = 0; i < numColor; ++i)
    {
        if (fb.m_colorAttachments[i].m_texture.IsValid())
            lvkFb.color[i].texture = ToLvk(fb.m_colorAttachments[i].m_texture);
        if (fb.m_colorAttachments[i].m_resolveMSAATexture.IsValid())
            lvkFb.color[i].resolveTexture = ToLvk(fb.m_colorAttachments[i].m_resolveMSAATexture);
    }
    if (fb.m_depthStencilTexture.m_texture.IsValid())
        lvkFb.depthStencil.texture = ToLvk(fb.m_depthStencilTexture.m_texture);
    return lvkFb;
}

// ---------------------------------------------------------------------------
// Resource creation
// ---------------------------------------------------------------------------

Expected<TextureHandle, String> VulkanRHIContext::CreateTexture(const TextureDesc& desc)
{
    lvk::TextureDesc lvkDesc;
    lvkDesc.type            = toTextureTypeLvk(desc.type);
    lvkDesc.format          = toFormatLvk(desc.format);
    lvkDesc.dimensions      = { desc.width, desc.height, desc.depth };
    lvkDesc.numLayers       = desc.numLayers;
    lvkDesc.numSamples      = toSampleCountLvk(desc.numSamples);
    lvkDesc.usage           = toTextureUsageLvk(desc.usage);
    lvkDesc.numMipLevels    = desc.numMipLevels;
    lvkDesc.data            = desc.initialData;
    lvkDesc.debugName       = desc.debugName ? desc.debugName : "";

    lvk::Result result;
    lvk::Holder<lvk::TextureHandle> holder = m_lvkCtx->createTexture(lvkDesc, nullptr, &result);
    if (!result.isOk())
        return Unexpected<String>(result.message);

    return m_textures.Create(holder.release());
}

Expected<BufferHandle, String> VulkanRHIContext::CreateBuffer(const BufferDesc& desc)
{
    lvk::BufferDesc lvkDesc;
    lvkDesc.usage     = toBufferUsageLvk(desc.usage);
    lvkDesc.storage   = toStorageTypeLvk(desc.storage);
    lvkDesc.size      = static_cast<size_t>(desc.size);
    lvkDesc.data      = desc.initialData;
    lvkDesc.debugName = desc.debugName ? desc.debugName : "";

    lvk::Result result;
    lvk::Holder<lvk::BufferHandle> holder = m_lvkCtx->createBuffer(lvkDesc, nullptr, &result);
    if (!result.isOk())
        return Unexpected<String>(result.message);

    return m_buffers.Create(holder.release());
}

Expected<SamplerHandle, String> VulkanRHIContext::CreateSampler(const SamplerDesc& desc)
{
    lvk::SamplerStateDesc lvkDesc;
    lvkDesc.minFilter           = toSamplerFilterLvk(desc.minFilter);
    lvkDesc.magFilter           = toSamplerFilterLvk(desc.magFilter);
    lvkDesc.mipMap              = toSamplerMipLvk(desc.mipFilter);
    lvkDesc.wrapU               = toSamplerWrapLvk(desc.addressU);
    lvkDesc.wrapV               = toSamplerWrapLvk(desc.addressV);
    lvkDesc.wrapW               = toSamplerWrapLvk(desc.addressW);
    lvkDesc.depthCompareOp      = toCompareOpLvk(desc.depthCompareOp);
    lvkDesc.depthCompareEnabled = desc.depthCompareEnabled;
    lvkDesc.mipLodMin           = static_cast<uint8_t>(desc.mipLodMin);
    lvkDesc.mipLodMax           = static_cast<uint8_t>(desc.mipLodMax);
    lvkDesc.debugName           = desc.debugName ? desc.debugName : "";

    lvk::Result result;
    lvk::Holder<lvk::SamplerHandle> holder = m_lvkCtx->createSampler(lvkDesc, &result);
    if (!result.isOk())
        return Unexpected<String>(result.message);

    return m_samplers.Create(holder.release());
}

Expected<ShaderModuleHandle, String> VulkanRHIContext::CreateShaderModule(const ShaderModuleDesc& desc)
{
    lvk::Result result;
    // Stage is embedded in SPIR-V; Stage_Vert is used as a placeholder.
    lvk::Holder<lvk::ShaderModuleHandle> holder = m_lvkCtx->createShaderModule(
        lvk::ShaderModuleDesc(desc.spirvData, desc.spirvSize, lvk::Stage_Vert,
                              desc.debugName ? desc.debugName : ""),
        &result);
    if (!result.isOk())
        return Unexpected<String>(result.message);

    return m_shaderModules.Create(holder.release());
}

Expected<RenderPipelineHandle, String> VulkanRHIContext::CreateRenderPipeline(const RenderPipelineDesc& desc)
{
    lvk::RenderPipelineDesc lvkDesc;
    lvkDesc.smVert       = ToLvk(desc.vertexShader);
    lvkDesc.smFrag       = ToLvk(desc.fragmentShader);
    lvkDesc.topology     = toTopologyLvk(desc.topology);
    lvkDesc.cullMode     = toCullModeLvk(desc.cullMode);
    lvkDesc.frontFace    = toWindingModeLvk(desc.frontFace);
    lvkDesc.polygonMode  = toPolygonModeLvk(desc.polygonMode);
    lvkDesc.samplesCount = toSampleCountLvk(desc.numSamples);
    lvkDesc.depthFormat  = toFormatLvk(desc.depthFormat);
    lvkDesc.debugName    = desc.debugName ? desc.debugName : "";

    // Vertex input
    for (uint32_t i = 0; i < desc.vertexInput.numAttributes; ++i)
    {
        const auto& src                   = desc.vertexInput.attributes[i];
        lvkDesc.vertexInput.attributes[i] = {
            .location = src.location,
            .binding  = src.binding,
            .format   = toVertexFormatLvk(src.format),
            .offset   = src.offset,
        };
    }
    for (uint32_t i = 0; i < desc.vertexInput.numBindings; ++i)
    {
        const auto& src = desc.vertexInput.bindings[i];
        // LVK's inputBindings is indexed by the binding slot, not sequentially.
        lvkDesc.vertexInput.inputBindings[src.binding] = {
            .stride    = src.stride,
            .inputRate = lvk::VertexInputRate_Vertex,
        };
    }

    // Color attachments
    for (uint32_t i = 0; i < desc.numColorAttachments; ++i)
    {
        const auto& src  = desc.colorAttachments[i];
        lvkDesc.color[i] = {
            .format              = toFormatLvk(src.format),
            .blendEnabled        = src.blendMode.m_enabled,
            .rgbBlendOp          = toBlendOpLvk(src.blendMode.m_equation.m_op),
            .alphaBlendOp        = toBlendOpLvk(src.blendMode.m_equation.m_opAlpha),
            .srcRGBBlendFactor   = toBlendFactorLvk(src.blendMode.m_equation.m_srcFactor),
            .srcAlphaBlendFactor = toBlendFactorLvk(src.blendMode.m_equation.m_srcFactorAlpha),
            .dstRGBBlendFactor   = toBlendFactorLvk(src.blendMode.m_equation.m_dstFactor),
            .dstAlphaBlendFactor = toBlendFactorLvk(src.blendMode.m_equation.m_dstFactorAlpha),
        };
    }

    // Stencil state
    if (desc.stencilTest.m_enabled)
    {
        const auto buildStencilState = [&](const pdl::StencilTest& st) -> lvk::StencilState {
            return {
                .stencilFailureOp   = toStencilOpLvk(st.m_failOp),
                .depthFailureOp     = toStencilOpLvk(st.m_depthFailOp),
                .depthStencilPassOp = toStencilOpLvk(st.m_passOp),
                .stencilCompareOp   = toCompareOpLvk(st.m_compareOp),
                .readMask           = st.m_readMask,
                .writeMask          = st.m_writeMask,
            };
        };
        lvkDesc.backFaceStencil  = buildStencilState(desc.stencilTest);
        lvkDesc.frontFaceStencil = buildStencilState(desc.stencilTest);
    }

    lvk::Result result;
    lvk::Holder<lvk::RenderPipelineHandle> holder = m_lvkCtx->createRenderPipeline(lvkDesc, &result);
    if (!result.isOk())
        return Unexpected<String>(result.message);

    return m_renderPipelines.Create({ holder.release(), desc.depthTest });
}

Expected<ComputePipelineHandle, String> VulkanRHIContext::CreateComputePipeline(const ComputePipelineDesc& desc)
{
    lvk::ComputePipelineDesc lvkDesc;
    lvkDesc.smComp    = ToLvk(desc.computeShader);
    lvkDesc.debugName = desc.debugName ? desc.debugName : "";

    lvk::Result result;
    lvk::Holder<lvk::ComputePipelineHandle> holder = m_lvkCtx->createComputePipeline(lvkDesc, &result);
    if (!result.isOk())
        return Unexpected<String>(result.message);

    return m_computePipelines.Create(holder.release());
}

// ---------------------------------------------------------------------------
// Resource destruction
// ---------------------------------------------------------------------------

void VulkanRHIContext::Destroy(TextureHandle handle)
{
    m_lvkCtx->destroy(ToLvk(handle));
    m_textures.Destroy(handle);
}

void VulkanRHIContext::Destroy(BufferHandle handle)
{
    m_lvkCtx->destroy(ToLvk(handle));
    m_buffers.Destroy(handle);
}

void VulkanRHIContext::Destroy(SamplerHandle handle)
{
    m_lvkCtx->destroy(ToLvk(handle));
    m_samplers.Destroy(handle);
}

void VulkanRHIContext::Destroy(ShaderModuleHandle handle)
{
    m_lvkCtx->destroy(ToLvk(handle));
    m_shaderModules.Destroy(handle);
}

void VulkanRHIContext::Destroy(RenderPipelineHandle handle)
{
    m_lvkCtx->destroy(ToLvk(handle));
    m_renderPipelines.Destroy(handle);
}

void VulkanRHIContext::Destroy(ComputePipelineHandle handle)
{
    m_lvkCtx->destroy(ToLvk(handle));
    m_computePipelines.Destroy(handle);
}

// ---------------------------------------------------------------------------
// Buffer data
// ---------------------------------------------------------------------------

void* VulkanRHIContext::GetMappedPtr(BufferHandle handle)
{
    return m_lvkCtx->getMappedPtr(ToLvk(handle));
}

Expected<void, String> VulkanRHIContext::Upload(BufferHandle handle, const void* data, size_t size, uint64 offset)
{
    return fromLvkResult(m_lvkCtx->upload(ToLvk(handle), data, size, static_cast<size_t>(offset)));
}

Expected<void, String> VulkanRHIContext::Download(BufferHandle handle, void* data, size_t size, uint64 offset)
{
    return fromLvkResult(m_lvkCtx->download(ToLvk(handle), data, size, static_cast<size_t>(offset)));
}

// ---------------------------------------------------------------------------
// Texture info
// ---------------------------------------------------------------------------

IRHIContext::Dimensions VulkanRHIContext::GetDimensions(TextureHandle handle) const
{
    const lvk::Dimensions d = m_lvkCtx->getDimensions(ToLvk(handle));
    return { d.width, d.height, d.depth };
}

Format VulkanRHIContext::GetTextureFormat(TextureHandle handle) const
{
    return fromFormatLvk(m_lvkCtx->getFormat(ToLvk(handle)));
}

// ---------------------------------------------------------------------------
// Swapchain
// ---------------------------------------------------------------------------

TextureHandle VulkanRHIContext::GetCurrentSwapchainTexture()
{
    lvk::TextureHandle lvkSwap = m_lvkCtx->getCurrentSwapchainTexture();

    if (!m_swapchainHandle.IsValid())
    {
        // Allocate a persistent pool slot the first time.
        m_swapchainHandle = m_textures.Create(std::move(lvkSwap));
    }
    else
    {
        // Update the stored lvk handle — the swapchain image rotates each frame.
        *m_textures.Get(m_swapchainHandle) = lvkSwap;
    }

    return m_swapchainHandle;
}

Format VulkanRHIContext::GetSwapchainFormat() const
{
    return fromFormatLvk(m_lvkCtx->getSwapchainFormat());
}

// ---------------------------------------------------------------------------
// Frame
// ---------------------------------------------------------------------------

IRHICommandBuffer& VulkanRHIContext::AcquireCommandBuffer()
{
    m_commandBuffer.m_lvkCmdBuf = &m_lvkCtx->acquireCommandBuffer();
    return m_commandBuffer;
}

void VulkanRHIContext::Submit(IRHICommandBuffer& cmdBuf, TextureHandle presentTexture)
{
    auto& vkCmdBuf = static_cast<VulkanRHICommandBuffer&>(cmdBuf);
    // presentTexture is optional — do not attempt a pool lookup on an empty handle.
    const lvk::TextureHandle lvkPresent = presentTexture.IsValid() ? ToLvk(presentTexture) : lvk::TextureHandle{};
    m_lvkCtx->submit(*vkCmdBuf.m_lvkCmdBuf, lvkPresent);
    vkCmdBuf.m_lvkCmdBuf = nullptr;
}

// ---------------------------------------------------------------------------
// Sync + info
// ---------------------------------------------------------------------------

void VulkanRHIContext::WaitIdle()
{
    m_lvkCtx->wait(lvk::SubmitHandle{});
}

const RenderDeviceInfo& VulkanRHIContext::GetDeviceInfo() const
{
    return m_deviceInfo;
}

// ===========================================================================
// VulkanRHICommandBuffer
// ===========================================================================

void VulkanRHIContext::VulkanRHICommandBuffer::CmdBeginRendering(const RenderPass& renderPass, const Framebuffer& framebuffer)
{
    m_lvkCmdBuf->cmdBeginRendering(toRenderPassLvk(renderPass), m_owner->ToFramebufferLvk(framebuffer));
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdEndRendering()
{
    m_lvkCmdBuf->cmdEndRendering();
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdBindRenderPipeline(RenderPipelineHandle handle)
{
    const RenderPipelineEntry* entry = m_owner->m_renderPipelines.Get(handle);
    pdlAssert(entry);
    if (!entry)
        return;

    m_lvkCmdBuf->cmdBindRenderPipeline(entry->lvkHandle);

    // Apply the depth test state that was baked into the pipeline descriptor.
    const DepthTest& dt = entry->depthTest;
    m_lvkCmdBuf->cmdBindDepthState({
        .compareOp           = dt.m_enabled ? toCompareOpLvk(dt.m_compareOp) : lvk::CompareOp_AlwaysPass,
        .isDepthWriteEnabled = dt.m_enabled && dt.m_writeEnabled,
    });
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdBindComputePipeline(ComputePipelineHandle handle)
{
    m_lvkCmdBuf->cmdBindComputePipeline(m_owner->ToLvk(handle));
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdBindIndexBuffer(BufferHandle buffer, IndexFormat format, uint64 offset)
{
    m_lvkCmdBuf->cmdBindIndexBuffer(m_owner->ToLvk(buffer), toIndexFormatLvk(format), offset);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdBindVertexBuffer(uint32 index, BufferHandle buffer, uint64 offset)
{
    m_lvkCmdBuf->cmdBindVertexBuffer(index, m_owner->ToLvk(buffer), offset);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdPushConstants(const void* data, size_t size, uint32 offset)
{
    m_lvkCmdBuf->cmdPushConstants(data, size, offset);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdDraw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance)
{
    m_lvkCmdBuf->cmdDraw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdDrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance)
{
    m_lvkCmdBuf->cmdDrawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdDrawIndirect(BufferHandle buffer, uint64 offset, uint32 drawCount, uint32 stride)
{
    m_lvkCmdBuf->cmdDrawIndirect(m_owner->ToLvk(buffer), static_cast<size_t>(offset), drawCount, stride);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdDrawIndexedIndirect(BufferHandle buffer, uint64 offset, uint32 drawCount, uint32 stride)
{
    m_lvkCmdBuf->cmdDrawIndexedIndirect(m_owner->ToLvk(buffer), static_cast<size_t>(offset), drawCount, stride);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdDispatch(uint32 x, uint32 y, uint32 z)
{
    m_lvkCmdBuf->cmdDispatchThreadGroups({ x, y, z });
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdSetViewport(const Viewport& viewport)
{
    m_lvkCmdBuf->cmdBindViewport({
        .x        = viewport.x,
        .y        = viewport.y,
        .width    = viewport.width,
        .height   = viewport.height,
        .minDepth = viewport.minDepth,
        .maxDepth = viewport.maxDepth,
    });
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdSetScissorRect(const ScissorRect& rect)
{
    m_lvkCmdBuf->cmdBindScissorRect({ rect.x, rect.y, rect.width, rect.height });
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdSetDepthBias(const DepthBias& bias)
{
    m_lvkCmdBuf->cmdSetDepthBiasEnable(bias.m_enabled);
    if (bias.m_enabled)
        m_lvkCmdBuf->cmdSetDepthBias(bias.m_constantFactor, bias.m_slopeFactor, bias.m_clamp);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdTransitionImageLayout(
    TextureHandle texture, ResourceState /*src*/, ResourceState dst, const SubresourceRange& /*range*/)
{
    const lvk::TextureHandle lvkTex = m_owner->ToLvk(texture);
    if (dst == ResourceState::ShaderResource || dst == ResourceState::DepthRead)
        m_lvkCmdBuf->transitionToShaderReadOnly(lvkTex);
    else if (dst == ResourceState::RenderTarget || dst == ResourceState::DepthWrite)
        m_lvkCmdBuf->transitionToRenderingLocalRead(lvkTex);
    // Other transitions are handled implicitly by LVK's automatic tracking
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdCopyImage(
    TextureHandle src, ResourceState /*srcState*/, TextureHandle dst, ResourceState /*dstState*/)
{
    const lvk::TextureHandle lvkSrc = m_owner->ToLvk(src);
    const lvk::Dimensions extent = m_owner->m_lvkCtx->getDimensions(lvkSrc);
    m_lvkCmdBuf->cmdCopyImage(lvkSrc, m_owner->ToLvk(dst), extent);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdClearColorImage(
    TextureHandle texture, const Math::Vector4& clearColor, ResourceState /*state*/)
{
    const lvk::ClearColorValue value = { .float32 = { clearColor.x, clearColor.y, clearColor.z, clearColor.w } };
    m_lvkCmdBuf->cmdClearColorImage(m_owner->ToLvk(texture), value);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdGenerateMipmap(TextureHandle texture)
{
    m_lvkCmdBuf->cmdGenerateMipmap(m_owner->ToLvk(texture));
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdCopyBuffer(
    BufferHandle src, uint64 srcOffset, BufferHandle dst, uint64 dstOffset, uint64 size)
{
    m_lvkCmdBuf->cmdCopyBuffer(
        m_owner->ToLvk(src),
        m_owner->ToLvk(dst),
        static_cast<size_t>(srcOffset),
        static_cast<size_t>(dstOffset),
        static_cast<size_t>(size));
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdUpdateBuffer(
    BufferHandle buffer, uint64 offset, const void* data, uint64 size)
{
    m_lvkCmdBuf->cmdUpdateBuffer(m_owner->ToLvk(buffer),
        static_cast<size_t>(offset), static_cast<size_t>(size), data);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdPushDebugGroupLabel(const char* label)
{
    m_lvkCmdBuf->cmdPushDebugGroupLabel(label);
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdPopDebugGroupLabel()
{
    m_lvkCmdBuf->cmdPopDebugGroupLabel();
}

void VulkanRHIContext::VulkanRHICommandBuffer::CmdInsertDebugEventLabel(const char* label)
{
    m_lvkCmdBuf->cmdInsertDebugEventLabel(label);
}

} // namespace pdl
