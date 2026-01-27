
#pragma once
#include "Base/DebugHelpers.h"
#include "Log/Log.h"
#include "Math/Vector3i.h"
#include "Renderer/RendererTypes.h"

#include <vulkan/vulkan.hpp>

// Created on 2025-03-23 by sisco
#ifndef NDEBUG
    #define CHECK_VK_RESULTVALUE(x) do{if ((x).result != vk::Result::eSuccess) { pdlLogError("Vulkan Error: %s", to_string((x).result).c_str()); pdlLogFlush(); pdlAssert(0); }} while(0)
    #define CHECK_VK_RESULT(x) do{if ((x) != vk::Result::eSuccess) { pdlLogError("Vulkan Error: %s", to_string(x).c_str()); pdlLogFlush(); pdlAssert(0); }} while(0)
#else
    #define CHECK_VK_RESULTVALUE(x) do{ sizeof(x); } while(0)
    #define CHECK_VK_RESULT(x) do{ sizeof(x); } while(0)
#endif

namespace pdl
{
class VulkanUtils
{
public:
    static vk::Format TranslateToVkFormat(Format format);
    static Format TranslateFromVkFormat(vk::Format format);
    static vk::ImageUsageFlags TranslateToVkImageUsageFlags(TextureUsage usage);
    static int CalculateMipLevels(const Math::Vector3i& extents);
    static vk::ImageAspectFlags GetVkAspectFlagsFromFormat(vk::Format format);
    static vk::ImageLayout GetImageLayoutFromState(ResourceState state);
    static vk::AccessFlags GetAccessFlagsFromImageLayout(vk::ImageLayout layout);
    static vk::PipelineStageFlags GetPipelineStageFlagsFromImageLayout(vk::ImageLayout layout);
    static vk::ImageAspectFlags GetAspectMaskFromFormat(vk::Format format, TextureAspect aspect = TextureAspect::All);
    static vk::ImageUsageFlagBits GetImageUsageFlags(TextureUsage usage);
    static vk::ImageUsageFlags GetImageUsageFlags(TextureUsage usage, MemoryType memoryType, const void* initData);
    static vk::ImageUsageFlagBits GetImageUsageFlags(ResourceState state);
    static vk::BufferUsageFlags GetBufferUsageFlags(BufferUsage usage);
    static uint32 FindMemoryType(vk::PhysicalDeviceMemoryProperties const & memoryProperties, uint32 typeBits, vk::MemoryPropertyFlags requirementsMask);
    static vk::MemoryPropertyFlags GetMemoryPropertyFlags(MemoryType memoryType);
    static vk::DescriptorType GetDescriptorType(DescriptorTypes descriptorType);
    static vk::BlendOp GetBlendOp(BlendMode::BlendOp op);
    static vk::BlendFactor GetBlendFactor(BlendMode::BlendFactor factor);
    static vk::ColorBlendEquationEXT GetBlendEquation(BlendMode::BlendEquation equation);
    static vk::CullModeFlags GetCullMode(CullMode cullMode);
    static vk::FrontFace GetFrontFace(FrontFace frontFace);
    static vk::PolygonMode GetPolygonMode(PolygonMode polygonMode);
    static vk::StencilFaceFlags GetStencilFaceFlags(StencilTest::StencilFace stencilFace);
    static vk::StencilOp GetStencilOp(StencilTest::StencilOp op);
    static vk::CompareOp GetCompareOp(CompareOp op);
    static vk::ShaderStageFlags GetShaderStageFlags(ShaderType shaderType);
    static vk::ShaderStageFlagBits GetShaderStageBits(ShaderType shaderType);
    static vk::PrimitiveTopology GetPrimitiveTopology(PrimitiveTopology topology);
    static vk::SampleCountFlagBits GetMultisampleFlagBits(MultiSampleCount multisampleCount);
    static vk::ConservativeRasterizationModeEXT GetConservativeRasterizationMode(ConservativeRasterization conservativeMode);
    static vk::ColorComponentFlags GetColorComponentMasks(ColorChannelMask colorMask);
};

}

