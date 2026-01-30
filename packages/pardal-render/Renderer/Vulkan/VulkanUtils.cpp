
#include <Renderer/Vulkan/VulkanUtils.h>
#include <Containers/UnorderedMap.h>
#include <Math/Functions.h>

// Created on 2025-03-23 by sisco

namespace pdl
{
#define IS_SET(val, flag) (((val) & (flag)) != static_cast<decltype(val)>(0))

vk::Format VulkanUtils::TranslateToVkFormat(Format format)
{
    switch (format)
    {
    case Format::R32G32B32A32_TYPELESS:
        return vk::Format::eR32G32B32A32Sfloat;
    case Format::R32G32B32_TYPELESS:
        return vk::Format::eR32G32B32Sfloat;
    case Format::R32G32_TYPELESS:
        return vk::Format::eR32G32Sfloat;
    case Format::R32_TYPELESS:
        return vk::Format::eR32Sfloat;

    case Format::R16G16B16A16_TYPELESS:
        return vk::Format::eR16G16B16A16Sfloat;
    case Format::R16G16_TYPELESS:
        return vk::Format::eR16G16Sfloat;
    case Format::R16_TYPELESS:
        return vk::Format::eR16Sfloat;

    case Format::R8G8B8A8_TYPELESS:
        return vk::Format::eR8G8B8A8Unorm;
    case Format::R8G8_TYPELESS:
        return vk::Format::eR8G8Unorm;
    case Format::R8_TYPELESS:
        return vk::Format::eR8Unorm;
    case Format::B8G8R8A8_TYPELESS:
        return vk::Format::eB8G8R8A8Unorm;

    case Format::R64_UINT:
        return vk::Format::eR64Uint;

    case Format::R32G32B32A32_FLOAT:
        return vk::Format::eR32G32B32A32Sfloat;
    case Format::R32G32B32_FLOAT:
        return vk::Format::eR32G32B32Sfloat;
    case Format::R32G32_FLOAT:
        return vk::Format::eR32G32Sfloat;
    case Format::R32_FLOAT:
        return vk::Format::eR32Sfloat;

    case Format::R16G16B16A16_FLOAT:
        return vk::Format::eR16G16B16A16Sfloat;
    case Format::R16G16_FLOAT:
        return vk::Format::eR16G16Sfloat;
    case Format::R16_FLOAT:
        return vk::Format::eR16Sfloat;

    case Format::R32G32B32A32_UINT:
        return vk::Format::eR32G32B32A32Uint;
    case Format::R32G32B32_UINT:
        return vk::Format::eR32G32B32Uint;
    case Format::R32G32_UINT:
        return vk::Format::eR32G32Uint;
    case Format::R32_UINT:
        return vk::Format::eR32Uint;

    case Format::R16G16B16A16_UINT:
        return vk::Format::eR16G16B16A16Uint;
    case Format::R16G16_UINT:
        return vk::Format::eR16G16Uint;
    case Format::R16_UINT:
        return vk::Format::eR16Uint;

    case Format::R8G8B8A8_UINT:
        return vk::Format::eR8G8B8A8Uint;
    case Format::R8G8_UINT:
        return vk::Format::eR8G8Uint;
    case Format::R8_UINT:
        return vk::Format::eR8Uint;

    case Format::R64_SINT:
        return vk::Format::eR64Sint;

    case Format::R32G32B32A32_SINT:
        return vk::Format::eR32G32B32A32Sint;
    case Format::R32G32B32_SINT:
        return vk::Format::eR32G32B32Sint;
    case Format::R32G32_SINT:
        return vk::Format::eR32G32Sint;
    case Format::R32_SINT:
        return vk::Format::eR32Sint;

    case Format::R16G16B16A16_SINT:
        return vk::Format::eR16G16B16A16Sint;
    case Format::R16G16_SINT:
        return vk::Format::eR16G16Sint;
    case Format::R16_SINT:
        return vk::Format::eR16Sint;

    case Format::R8G8B8A8_SINT:
        return vk::Format::eR8G8B8A8Sint;
    case Format::R8G8_SINT:
        return vk::Format::eR8G8Sint;
    case Format::R8_SINT:
        return vk::Format::eR8Sint;

    case Format::R16G16B16A16_UNORM:
        return vk::Format::eR16G16B16A16Unorm;
    case Format::R16G16_UNORM:
        return vk::Format::eR16G16Unorm;
    case Format::R16_UNORM:
        return vk::Format::eR16Unorm;

    case Format::R8G8B8A8_UNORM:
        return vk::Format::eR8G8B8A8Unorm;
    case Format::R8G8B8A8_UNORM_SRGB:
        return vk::Format::eR8G8B8A8Srgb;
    case Format::R8G8_UNORM:
        return vk::Format::eR8G8Unorm;
    case Format::R8_UNORM:
        return vk::Format::eR8Unorm;
    case Format::B8G8R8A8_UNORM:
        return vk::Format::eB8G8R8A8Unorm;
    case Format::B8G8R8A8_UNORM_SRGB:
        return vk::Format::eB8G8R8A8Srgb;
    case Format::B8G8R8X8_UNORM:
        return vk::Format::eB8G8R8A8Unorm;
    case Format::B8G8R8X8_UNORM_SRGB:
        return vk::Format::eB8G8R8A8Srgb;

    case Format::R16G16B16A16_SNORM:
        return vk::Format::eR16G16B16A16Snorm;
    case Format::R16G16_SNORM:
        return vk::Format::eR16G16Snorm;
    case Format::R16_SNORM:
        return vk::Format::eR16Snorm;

    case Format::R8G8B8A8_SNORM:
        return vk::Format::eR8G8B8A8Snorm;
    case Format::R8G8_SNORM:
        return vk::Format::eR8G8Snorm;
    case Format::R8_SNORM:
        return vk::Format::eR8Snorm;

    case Format::D32_FLOAT:
        return vk::Format::eD32Sfloat;
    case Format::D16_UNORM:
        return vk::Format::eD16Unorm;
    case Format::D32_FLOAT_S8_UINT:
        return vk::Format::eD32SfloatS8Uint;
    case Format::R32_FLOAT_X32_TYPELESS:
        return vk::Format::eR32Sfloat;

    case Format::B4G4R4A4_UNORM:
        return vk::Format::eA4R4G4B4UnormPack16EXT;
    case Format::B5G6R5_UNORM:
        return vk::Format::eR5G6B5UnormPack16;
    case Format::B5G5R5A1_UNORM:
        return vk::Format::eA1R5G5B5UnormPack16;

    case Format::R9G9B9E5_SHAREDEXP:
        return vk::Format::eE5B9G9R9UfloatPack32;
    case Format::R10G10B10A2_TYPELESS:
        return vk::Format::eA2B10G10R10UintPack32;
    case Format::R10G10B10A2_UINT:
        return vk::Format::eA2B10G10R10UintPack32;
    case Format::R10G10B10A2_UNORM:
        return vk::Format::eA2B10G10R10UnormPack32;
    case Format::R11G11B10_FLOAT:
        return vk::Format::eB10G11R11UfloatPack32;

    case Format::BC1_UNORM:
        return vk::Format::eBc1RgbaUnormBlock;
    case Format::BC1_UNORM_SRGB:
        return vk::Format::eBc1RgbaSrgbBlock;
    case Format::BC2_UNORM:
        return vk::Format::eBc2UnormBlock;
    case Format::BC2_UNORM_SRGB:
        return vk::Format::eBc2SrgbBlock;
    case Format::BC3_UNORM:
        return vk::Format::eBc3UnormBlock;
    case Format::BC3_UNORM_SRGB:
        return vk::Format::eBc3SrgbBlock;
    case Format::BC4_UNORM:
        return vk::Format::eBc4UnormBlock;
    case Format::BC4_SNORM:
        return vk::Format::eBc4SnormBlock;
    case Format::BC5_UNORM:
        return vk::Format::eBc5UnormBlock;
    case Format::BC5_SNORM:
        return vk::Format::eBc5SnormBlock;
    case Format::BC6H_UF16:
        return vk::Format::eBc6HUfloatBlock;
    case Format::BC6H_SF16:
        return vk::Format::eBc6HSfloatBlock;
    case Format::BC7_UNORM:
        return vk::Format::eBc7UnormBlock;
    case Format::BC7_UNORM_SRGB:
        return vk::Format::eBc7SrgbBlock;

    default:
        return vk::Format::eUndefined;
    }
}

Format VulkanUtils::TranslateFromVkFormat(vk::Format format)
{
    return ReverseMap<Format, vk::Format>(VulkanUtils::TranslateToVkFormat, Format::Unknown, Format::FormatCount)(format);
}

vk::ImageUsageFlags VulkanUtils::TranslateToVkImageUsageFlags(TextureUsage usage)
{
    vk::ImageUsageFlags flags{};
    if (IS_SET(usage, TextureUsage::ShaderResource))
        flags |= vk::ImageUsageFlagBits::eSampled;
    if (IS_SET(usage, TextureUsage::UnorderedAccess))
        flags |= vk::ImageUsageFlagBits::eStorage;
    if (IS_SET(usage, TextureUsage::RenderTarget))
        flags |= vk::ImageUsageFlagBits::eColorAttachment;
    if (IS_SET(usage, TextureUsage::DepthRead))
        flags |= vk::ImageUsageFlagBits::eInputAttachment;
    if (IS_SET(usage, TextureUsage::DepthWrite))
        flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    if (IS_SET(usage, TextureUsage::Present))
        flags |= vk::ImageUsageFlagBits::eTransferSrc;
    if (IS_SET(usage, TextureUsage::CopySource))
        flags |= vk::ImageUsageFlagBits::eTransferSrc;
    if (IS_SET(usage, TextureUsage::CopyDestination))
        flags |= vk::ImageUsageFlagBits::eTransferDst;
    if (IS_SET(usage, TextureUsage::ResolveSource))
        flags |= vk::ImageUsageFlagBits::eTransferSrc;
    if (IS_SET(usage, TextureUsage::ResolveDestination))
        flags |= vk::ImageUsageFlagBits::eTransferDst;
    return flags;
}

int VulkanUtils::CalculateMipLevels(const Math::Vector3i& extents)
{
    int32 maxSize = Math::Max(Math::Max(extents.x, extents.y), extents.z);
    return Math::Log2(maxSize);
}

vk::ImageAspectFlags VulkanUtils::GetVkAspectFlagsFromFormat(vk::Format format)
{
    switch (format)
    {
    case vk::Format::eD16UnormS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    case vk::Format::eD16Unorm:
    case vk::Format::eD32Sfloat:
    case vk::Format::eX8D24UnormPack32:
        return vk::ImageAspectFlagBits::eDepth;
    case vk::Format::eS8Uint:
        return vk::ImageAspectFlagBits::eStencil;
    default:
        return vk::ImageAspectFlagBits::eColor;
    }
}

vk::ImageLayout VulkanUtils::GetImageLayoutFromState(ResourceState state)
{
    switch (state)
    {
    case ResourceState::ShaderResource:
        return vk::ImageLayout::eShaderReadOnlyOptimal;
    case ResourceState::UnorderedAccess:
    case ResourceState::General:
        return vk::ImageLayout::eGeneral;
    case ResourceState::Present:
        return vk::ImageLayout::ePresentSrcKHR;
    case ResourceState::CopySource:
        return vk::ImageLayout::eTransferSrcOptimal;
    case ResourceState::CopyDestination:
        return vk::ImageLayout::eTransferDstOptimal;
    case ResourceState::RenderTarget:
        return vk::ImageLayout::eColorAttachmentOptimal;
    case ResourceState::DepthWrite:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case ResourceState::DepthRead:
        return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
    case ResourceState::ResolveSource:
        return vk::ImageLayout::eTransferSrcOptimal;
    case ResourceState::ResolveDestination:
        return vk::ImageLayout::eTransferDstOptimal;
    default:
        return vk::ImageLayout::eUndefined;
    }
}

vk::AccessFlags VulkanUtils::GetAccessFlagsFromImageLayout(vk::ImageLayout layout)
{
    switch (layout)
    {
    case vk::ImageLayout::eUndefined:
    case vk::ImageLayout::eGeneral:
    case vk::ImageLayout::ePreinitialized:
    case vk::ImageLayout::ePresentSrcKHR:
        return (vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    case vk::ImageLayout::eColorAttachmentOptimal:
        return (vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthAttachmentOptimal:
    case vk::ImageLayout::eStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
        return (vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead);
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    case vk::ImageLayout::eDepthReadOnlyOptimalKHR:
    case vk::ImageLayout::eStencilReadOnlyOptimal:
        return vk::AccessFlagBits::eDepthStencilAttachmentRead;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return vk::AccessFlagBits::eShaderRead;
    case vk::ImageLayout::eTransferSrcOptimal:
        return vk::AccessFlagBits::eTransferRead;
    case vk::ImageLayout::eTransferDstOptimal:
        return vk::AccessFlagBits::eTransferWrite;
    default:
        pdlAssert(0 && "Unsupported VkImageLayout");
        return (vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite);
    }
}

vk::PipelineStageFlags VulkanUtils::GetPipelineStageFlagsFromImageLayout(vk::ImageLayout layout)
{
    switch (layout)
    {
    case vk::ImageLayout::eUndefined:
    case vk::ImageLayout::ePreinitialized:
    case vk::ImageLayout::ePresentSrcKHR:
    case vk::ImageLayout::eGeneral:
        return vk::PipelineStageFlagBits::eAllCommands;
    case vk::ImageLayout::eColorAttachmentOptimal:
        return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return (vk::PipelineStageFlagBits::eAllGraphics | vk::PipelineStageFlagBits::eComputeShader);
    case vk::ImageLayout::eTransferSrcOptimal:
        return vk::PipelineStageFlagBits::eTransfer;
    case vk::ImageLayout::eTransferDstOptimal:
        return vk::PipelineStageFlagBits::eTransfer;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
    case vk::ImageLayout::eDepthAttachmentOptimal:
    case vk::ImageLayout::eDepthReadOnlyOptimal:
    case vk::ImageLayout::eStencilAttachmentOptimal:
    case vk::ImageLayout::eStencilReadOnlyOptimal:
        return (vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests);
    default:
        pdlAssert(0 && "Unsupported VkImageLayout");
        return vk::PipelineStageFlagBits::eAllCommands;
    }    
}

vk::ImageAspectFlags VulkanUtils::GetAspectMaskFromFormat(vk::Format format, TextureAspect aspect)
{
    switch (aspect)
    {
    case TextureAspect::All:
        switch (format)
        {
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
            return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
        case vk::Format::eD16Unorm:
        case vk::Format::eD32Sfloat:
        case vk::Format::eX8D24UnormPack32:
            return vk::ImageAspectFlagBits::eDepth;
        case vk::Format::eS8Uint:
            return vk::ImageAspectFlagBits::eStencil;
        default:
            return vk::ImageAspectFlagBits::eColor;
        }
    case TextureAspect::Depth:
        return vk::ImageAspectFlagBits::eDepth;
    case TextureAspect::Stencil:
        return vk::ImageAspectFlagBits::eStencil;
    default:
        return vk::ImageAspectFlagBits::eColor;
    }
}

vk::ImageUsageFlagBits VulkanUtils::GetImageUsageFlags(TextureUsage usage)
{
    int flags = 0;
    if (IS_SET(usage, TextureUsage::ShaderResource))
        flags |= (int)vk::ImageUsageFlagBits::eSampled;
    if (IS_SET(usage, TextureUsage::UnorderedAccess))
        flags |= (int)vk::ImageUsageFlagBits::eStorage;
    if (IS_SET(usage, TextureUsage::RenderTarget))
        flags |= (int)vk::ImageUsageFlagBits::eColorAttachment;
    if (IS_SET(usage, TextureUsage::DepthRead))
        flags |= (int)vk::ImageUsageFlagBits::eInputAttachment;
    if (IS_SET(usage, TextureUsage::DepthWrite))
        flags |= (int)vk::ImageUsageFlagBits::eDepthStencilAttachment;
    if (IS_SET(usage, TextureUsage::Present))
        flags |= (int)vk::ImageUsageFlagBits::eTransferSrc;
    if (IS_SET(usage, TextureUsage::CopySource))
        flags |= (int)vk::ImageUsageFlagBits::eTransferSrc;
    if (IS_SET(usage, TextureUsage::CopyDestination))
        flags |= (int)vk::ImageUsageFlagBits::eTransferDst;
    if (IS_SET(usage, TextureUsage::ShaderResource))
    if (IS_SET(usage, TextureUsage::ResolveSource))
        flags |= (int)vk::ImageUsageFlagBits::eTransferSrc;
    if (IS_SET(usage, TextureUsage::ResolveDestination))
        flags |= (int)vk::ImageUsageFlagBits::eTransferDst;
    return static_cast<vk::ImageUsageFlagBits>(flags);
}

vk::ImageUsageFlags VulkanUtils::GetImageUsageFlags(TextureUsage usage, MemoryType memoryType, const void* initData)
{
    int imageUsageFlags = static_cast<int>(GetImageUsageFlags(usage));
    if (memoryType == MemoryType::HostVisible || initData)
    {
        imageUsageFlags |= static_cast<int>(vk::ImageUsageFlagBits::eTransferDst);
    }

    return static_cast<vk::ImageUsageFlags>(imageUsageFlags);
}

vk::ImageUsageFlagBits VulkanUtils::GetImageUsageFlags(ResourceState state)
{
    switch (state)
    {
    case ResourceState::RenderTarget:
        return vk::ImageUsageFlagBits::eColorAttachment;
    case ResourceState::DepthWrite:
        return vk::ImageUsageFlagBits::eDepthStencilAttachment;
    case ResourceState::DepthRead:
        return vk::ImageUsageFlagBits::eInputAttachment;
    case ResourceState::ShaderResource:
        return vk::ImageUsageFlagBits::eSampled;
    case ResourceState::UnorderedAccess:
        return vk::ImageUsageFlagBits::eStorage;
    case ResourceState::CopySource:
    case ResourceState::ResolveSource:
    case ResourceState::Present:
        return vk::ImageUsageFlagBits::eTransferSrc;
    case ResourceState::CopyDestination:
    case ResourceState::ResolveDestination:
        return vk::ImageUsageFlagBits::eTransferDst;
    case ResourceState::Undefined:
    case ResourceState::General:
        return static_cast<vk::ImageUsageFlagBits>(0);
    default:
        {
            pdlAssert(0 && "Unsupported Resource state");
            return static_cast<vk::ImageUsageFlagBits>(0);
        }
    }
}

vk::BufferUsageFlags VulkanUtils::GetBufferUsageFlags(BufferUsage usage)
{
    vk::BufferUsageFlags flags = {};
    if (IS_SET(usage, BufferUsage::VertexBuffer))
        flags |= vk::BufferUsageFlagBits::eVertexBuffer;
    if (IS_SET(usage, BufferUsage::IndexBuffer))
        flags |= vk::BufferUsageFlagBits::eIndexBuffer;
    if (IS_SET(usage, BufferUsage::ConstantBuffer))
        flags |= vk::BufferUsageFlagBits::eUniformBuffer;
    if (IS_SET(usage, BufferUsage::ShaderResource))
        flags |= vk::BufferUsageFlagBits::eUniformBuffer;
    if (IS_SET(usage, BufferUsage::UnorderedAccess))
        flags |= vk::BufferUsageFlagBits::eUniformBuffer;
    if (IS_SET(usage, BufferUsage::IndirectArgument))
        flags |= vk::BufferUsageFlagBits::eIndirectBuffer;
    if (IS_SET(usage, BufferUsage::CopySource))
        flags |= vk::BufferUsageFlagBits::eTransferSrc;
    if (IS_SET(usage, BufferUsage::CopyDestination))
        flags |= vk::BufferUsageFlagBits::eTransferDst;
    if (IS_SET(usage, BufferUsage::AccelerationStructure))
        flags |= vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
    if (IS_SET(usage, BufferUsage::AccelerationStructureBuildInput))
        flags |= vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
    if (IS_SET(usage, BufferUsage::ShaderTable))
        flags |= vk::BufferUsageFlagBits::eShaderBindingTableKHR;

    return flags;
}

uint32 VulkanUtils::FindMemoryType( vk::PhysicalDeviceMemoryProperties const & memoryProperties, uint32 typeBits, vk::MemoryPropertyFlags requirementsMask )
{
    uint32 typeIndex = uint32( ~0 );
    for ( uint32 i = 0; i < memoryProperties.memoryTypeCount; i++ )
    {
        if ( ( typeBits & 1 ) && ( ( memoryProperties.memoryTypes[i].propertyFlags & requirementsMask ) == requirementsMask ) )
        {
            typeIndex = i;
            break;
        }
        typeBits >>= 1;
    }
    assert( typeIndex != uint32( ~0 ) );
    return typeIndex;
}

vk::MemoryPropertyFlags VulkanUtils::GetMemoryPropertyFlags(MemoryType memoryType)
{
    switch (memoryType)
    {
    case MemoryType::HostVisible:
        return vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    case MemoryType::DeviceLocal:
        return vk::MemoryPropertyFlagBits::eDeviceLocal;
    case MemoryType::MemoryLess:
        return vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eLazilyAllocated;
    }
    pdlAssert(0 && "Unsupported MemoryType");
    return {};
}

vk::DescriptorType VulkanUtils::GetDescriptorType(DescriptorTypes descriptorType)
{
    switch (descriptorType)
    {
    case Sampler:
        return vk::DescriptorType::eSampler;
    case CombinedSampler:
        return vk::DescriptorType::eCombinedImageSampler;
    case UniformBuffer:
        return vk::DescriptorType::eUniformBuffer;
    case StorageBuffer:
        return vk::DescriptorType::eStorageBuffer;
    case AccelerationStructure:
        return vk::DescriptorType::eAccelerationStructureKHR;
    default:
        pdlNotImplemented();
        return vk::DescriptorType::eSampler;
    }
}

vk::BlendOp VulkanUtils::GetBlendOp(BlendMode::BlendOp op)
{
    switch (op)
    {
    case BlendMode::BlendOp::Add:
        return vk::BlendOp::eAdd;
    case BlendMode::BlendOp::Subtract:
        return vk::BlendOp::eSubtract;
    case BlendMode::BlendOp::ReverseSubtract:
        return vk::BlendOp::eReverseSubtract;
    case BlendMode::BlendOp::Min:
        return vk::BlendOp::eMin;
    case BlendMode::BlendOp::Max:
        return vk::BlendOp::eMax;
    }
    pdlAssert(0 && "Unsupported BlendOp");
    return vk::BlendOp::eAdd;
}

vk::BlendFactor VulkanUtils::GetBlendFactor(BlendMode::BlendFactor factor)
{
    switch (factor)
    {
        case BlendMode::BlendFactor::Zero:
            return vk::BlendFactor::eZero;
        case BlendMode::BlendFactor::One:
            return vk::BlendFactor::eOne;
        case BlendMode::BlendFactor::SrcColor:
            return vk::BlendFactor::eSrcColor;
        case BlendMode::BlendFactor::OneMinusSrcColor:
            return vk::BlendFactor::eOneMinusSrcColor;
        case BlendMode::BlendFactor::OneMinusDstColor:
            return vk::BlendFactor::eOneMinusDstColor;
        case BlendMode::BlendFactor::DstColor:
            return vk::BlendFactor::eDstColor;
        case BlendMode::BlendFactor::SrcAlpha:
            return vk::BlendFactor::eSrcAlpha;
    }
    pdlAssert(0 && "Unsupported BlendFactor");   
    return vk::BlendFactor::eOne;
}

vk::ColorBlendEquationEXT VulkanUtils::GetBlendEquation(BlendMode::BlendEquation equation)
{
    vk::ColorBlendEquationEXT vkEquation;
    vkEquation.colorBlendOp = GetBlendOp(equation.m_op);
    vkEquation.srcColorBlendFactor = GetBlendFactor(equation.m_srcFactor);
    vkEquation.dstColorBlendFactor = GetBlendFactor(equation.m_dstFactor);
    vkEquation.alphaBlendOp = GetBlendOp(equation.m_opAlpha);
    vkEquation.srcAlphaBlendFactor = GetBlendFactor(equation.m_srcFactorAlpha);
    vkEquation.dstAlphaBlendFactor = GetBlendFactor(equation.m_dstFactorAlpha);
    return vkEquation;
}

vk::CullModeFlags VulkanUtils::GetCullMode(CullMode cullMode)
{
    switch (cullMode)
    {
        case CullMode::None:
            return vk::CullModeFlagBits::eNone;
        case CullMode::Front:
            return vk::CullModeFlagBits::eFront;
        case CullMode::Back:
            return vk::CullModeFlagBits::eBack;
        default:
            pdlAssert(0 && "Unsupported CullMode");
            return vk::CullModeFlagBits::eNone;
    }
}
    
vk::FrontFace VulkanUtils::GetFrontFace(FrontFace frontFace)
{
    switch (frontFace)
    {
        case FrontFace::Clockwise:
            return vk::FrontFace::eClockwise;
        case FrontFace::CounterClockwise:
            return vk::FrontFace::eCounterClockwise;
        default:
            pdlAssert(0 && "Unsupported FrontFace");
            return vk::FrontFace::eClockwise;
    }
}

vk::PolygonMode VulkanUtils::GetPolygonMode(PolygonMode polygonMode)
{
    switch (polygonMode)
    {
        case PolygonMode::Fill:
            return vk::PolygonMode::eFill;
        case PolygonMode::Line:
            return vk::PolygonMode::eLine;
        case PolygonMode::Point:
            return vk::PolygonMode::ePoint;
        default:
            pdlAssert(0 && "Unsupported PolygonMode");
            return vk::PolygonMode::eFill;
       
    }
}

vk::StencilFaceFlags VulkanUtils::GetStencilFaceFlags(StencilTest::StencilFace stencilFace)
{
    switch (stencilFace)
    {
        case StencilTest::StencilFace::Front:
            return vk::StencilFaceFlagBits::eFront;
        case StencilTest::StencilFace::Back:
            return vk::StencilFaceFlagBits::eBack;
        case StencilTest::StencilFace::FrontAndBack:
            return vk::StencilFaceFlagBits::eFrontAndBack;
        default:
            pdlAssert(0 && "Unsupported StencilFace");
            return vk::StencilFaceFlagBits::eFront;
    }
}

vk::StencilOp VulkanUtils::GetStencilOp(StencilTest::StencilOp op)
{
    switch (op)
    {
        case StencilTest::StencilOp::Keep:
            return vk::StencilOp::eKeep;
        case StencilTest::StencilOp::Zero:
            return vk::StencilOp::eZero;
        case StencilTest::StencilOp::Replace:
            return vk::StencilOp::eReplace;
        case StencilTest::StencilOp::IncrementClamp:
            return vk::StencilOp::eIncrementAndClamp;
        case StencilTest::StencilOp::DecrementClamp:
            return vk::StencilOp::eDecrementAndClamp;
        case StencilTest::StencilOp::Invert:
            return vk::StencilOp::eInvert;
        case StencilTest::StencilOp::IncrementWrap:
            return vk::StencilOp::eIncrementAndWrap;
        case StencilTest::StencilOp::DecrementWrap:
            return vk::StencilOp::eDecrementAndWrap;
        default:
            pdlAssert(0 && "Unsupported StencilOp");
            return vk::StencilOp::eKeep;
    }
}

vk::CompareOp VulkanUtils::GetCompareOp(CompareOp op)
{
    switch (op)
    {
        case CompareOp::Never:
            return vk::CompareOp::eNever;
        case CompareOp::Less:
            return vk::CompareOp::eLess;
        case CompareOp::Equal:
            return vk::CompareOp::eEqual;
        case CompareOp::LessOrEqual:
            return vk::CompareOp::eLessOrEqual;
        case CompareOp::Greater:
            return vk::CompareOp::eGreater;
        case CompareOp::NotEqual:
            return vk::CompareOp::eNotEqual;
        case CompareOp::GreaterOrEqual:
            return vk::CompareOp::eGreaterOrEqual;
        case CompareOp::Always:
            return vk::CompareOp::eAlways;
        default:
            pdlAssert(0 && "Unsupported CompareOp");
            return vk::CompareOp::eNever;
    }
}

vk::ShaderStageFlags VulkanUtils::GetShaderStageFlags(ShaderType shaderType)
{
    vk::ShaderStageFlags shaderStage {};
    if (IS_SET(shaderType, ShaderType::Vertex))
        shaderStage |= vk::ShaderStageFlagBits::eVertex;
    if (IS_SET(shaderType, ShaderType::Fragment))
        shaderStage |= vk::ShaderStageFlagBits::eFragment;
    if (IS_SET(shaderType, ShaderType::Compute))
        shaderStage |= vk::ShaderStageFlagBits::eCompute;
    if (IS_SET(shaderType, ShaderType::Geometry))
        shaderStage |= vk::ShaderStageFlagBits::eGeometry;
    if (IS_SET(shaderType, ShaderType::Hull))
        shaderStage |= vk::ShaderStageFlagBits::eTessellationControl;
    if (IS_SET(shaderType, ShaderType::Domain))
        shaderStage |= vk::ShaderStageFlagBits::eTessellationEvaluation;
    if (IS_SET(shaderType, ShaderType::RayGeneration))
        shaderStage |= vk::ShaderStageFlagBits::eRaygenKHR;
    if (IS_SET(shaderType, ShaderType::Miss))
        shaderStage |= vk::ShaderStageFlagBits::eMissKHR;
    return shaderStage;
        
    
}

vk::ShaderStageFlagBits VulkanUtils::GetShaderStageBits(ShaderType shaderType)
{
    switch (shaderType)
    {
        case ShaderType::Vertex:
            return vk::ShaderStageFlagBits::eVertex;
        case ShaderType::Fragment:
            return vk::ShaderStageFlagBits::eFragment;
        case ShaderType::Compute:
            return vk::ShaderStageFlagBits::eCompute;
        case ShaderType::Geometry:
            return vk::ShaderStageFlagBits::eGeometry;
        case ShaderType::Hull:
            return vk::ShaderStageFlagBits::eTessellationControl;
        case ShaderType::Domain:
            return vk::ShaderStageFlagBits::eTessellationEvaluation;
        case ShaderType::RayGeneration:
            return vk::ShaderStageFlagBits::eRaygenKHR;
        case ShaderType::Miss:
            return vk::ShaderStageFlagBits::eMissKHR;
        default:
            pdlAssert(0 && "Unsupported or combined ShaderType");
            return vk::ShaderStageFlagBits::eVertex;
    }
}

vk::PrimitiveTopology VulkanUtils::GetPrimitiveTopology(PrimitiveTopology topology)
{
    switch (topology)
    {
        case PrimitiveTopology::TriangleList:
            return vk::PrimitiveTopology::eTriangleList;
        case PrimitiveTopology::TriangleStrip:
            return vk::PrimitiveTopology::eTriangleStrip;
        case PrimitiveTopology::LineList:
            return vk::PrimitiveTopology::eLineList;
        case PrimitiveTopology::LineStrip:
            return vk::PrimitiveTopology::eLineStrip;
        case PrimitiveTopology::PointList:
            return vk::PrimitiveTopology::ePointList;
        case PrimitiveTopology::TriangleFan:
            return vk::PrimitiveTopology::eTriangleFan;
        case PrimitiveTopology::LineListWithAdjacency:
            return vk::PrimitiveTopology::eLineListWithAdjacency;
        case PrimitiveTopology::LineStripWithAdjacency:
            return vk::PrimitiveTopology::eLineStripWithAdjacency;
        case PrimitiveTopology::TriangleListWithAdjacency:
            return vk::PrimitiveTopology::eTriangleListWithAdjacency;
        case PrimitiveTopology::TriangleStripWithAdjacency:
            return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
        case PrimitiveTopology::PatchList:
            return vk::PrimitiveTopology::ePatchList;
        default:
            pdlAssert(0 && "Unsupported primitive topology");
            return vk::PrimitiveTopology::eTriangleList;
    }
}

vk::SampleCountFlagBits VulkanUtils::GetMultisampleFlagBits(MultiSampleCount multisampleCount)
{
    switch (multisampleCount)
    {
        case MultiSampleCount::None:
            return {};
        case MultiSampleCount::One:
            return vk::SampleCountFlagBits::e1;
        case MultiSampleCount::Two:
            return vk::SampleCountFlagBits::e2;
        case MultiSampleCount::Four:
            return vk::SampleCountFlagBits::e4;
        case MultiSampleCount::Eight:
            return vk::SampleCountFlagBits::e8;
        case MultiSampleCount::Sixteen:
            return vk::SampleCountFlagBits::e16;
        case MultiSampleCount::ThirtyTwo:
            return vk::SampleCountFlagBits::e32;
        case MultiSampleCount::SixtyFour:
            return vk::SampleCountFlagBits::e64;
        default:
            pdlAssert(0 && "Unsupported multisample count");
            return vk::SampleCountFlagBits::e1;
    }
}

vk::ConservativeRasterizationModeEXT VulkanUtils::GetConservativeRasterizationMode(ConservativeRasterization conservativeMode)
{
    switch (conservativeMode)
    {
    case ConservativeRasterization::Off:
        return vk::ConservativeRasterizationModeEXT::eDisabled;
    case ConservativeRasterization::Overestimate:
        return vk::ConservativeRasterizationModeEXT::eOverestimate;
    case ConservativeRasterization::Underestimate:
        return vk::ConservativeRasterizationModeEXT::eUnderestimate;
    default:
        pdlAssert(0 && "Unsupported conservative rasterization mode");
        return vk::ConservativeRasterizationModeEXT::eDisabled;
    }
}

vk::ColorComponentFlags VulkanUtils::GetColorComponentMasks(ColorChannelMask colorMask)
{
    vk::ColorComponentFlags colorComponentMask {};
    if (IS_SET(colorMask, ColorChannelMask::Red))
        colorComponentMask |= vk::ColorComponentFlagBits::eR;
    if (IS_SET(colorMask, ColorChannelMask::Green))
        colorComponentMask |= vk::ColorComponentFlagBits::eG;
    if (IS_SET(colorMask, ColorChannelMask::Blue))
        colorComponentMask |= vk::ColorComponentFlagBits::eB;
    if (IS_SET(colorMask, ColorChannelMask::Alpha))
        colorComponentMask |= vk::ColorComponentFlagBits::eA;
    return colorComponentMask;
}

vk::ColorSpaceKHR VulkanUtils::GetColorSpace(ColorSpace colorSpace)
{
    switch (colorSpace) {
    case ColorSpace::Srgb:
        return vk::ColorSpaceKHR::eSrgbNonlinear;
    case ColorSpace::Linear:
        return vk::ColorSpaceKHR::eExtendedSrgbLinearEXT;
    case ColorSpace::HDR10:
        return vk::ColorSpaceKHR::eHdr10St2084EXT;
    case ColorSpace::BT709:
        return vk::ColorSpaceKHR::eBt709LinearEXT;
    }
    pdlAssert(0 && "Unsupported ColorSpace");
    return vk::ColorSpaceKHR::eSrgbNonlinear;
}
}

