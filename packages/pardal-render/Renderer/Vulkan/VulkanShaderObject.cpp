
#include <Renderer/Vulkan/VulkanShaderObject.h>
#include <Renderer/Vulkan/IVulkanDescriptorSet.h>
#include <Base/DebugHelpers.h>
#include <Renderer/Vulkan/VulkanUtils.h>

#include "VulkanBindlessDescriptors.h"

// Created on 2025-05-24 by sisco

namespace pdl
{
    VulkanShaderObject::VulkanShaderObject(ShaderObjectDescriptor& desc, vk::Device device) : m_device(device), m_shaderType(desc.m_shaderType)
    {
        pdlAssert(desc.m_descriptorSet);
        pdlAssert(desc.m_shaderData);

        auto descLayout = static_cast<VulkanBindlessDescriptors*>(desc.m_descriptorSet)->GetVkDescriptorSetLayout();
        
        vk::ShaderCreateInfoEXT shaderCreateInfo;
        shaderCreateInfo.codeSize = desc.m_shaderData->size();
        shaderCreateInfo.pCode = desc.m_shaderData->data();
        shaderCreateInfo.codeType = vk::ShaderCodeTypeEXT::eSpirv;
        shaderCreateInfo.setLayoutCount = 1;
        shaderCreateInfo.pSetLayouts = &descLayout;
        shaderCreateInfo.pName = desc.m_entryPoint.data();
        shaderCreateInfo.stage = VulkanUtils::GetShaderStageBits(m_shaderType);
        shaderCreateInfo.pNext = nullptr;
        shaderCreateInfo.flags = vk::ShaderCreateFlagBitsEXT::eLinkStage;

        auto shaderCreateResult = device.createShaderEXT(shaderCreateInfo);
        CHECK_VK_RESULTVALUE(shaderCreateResult);
        m_shader = shaderCreateResult.value;
    }

    VulkanShaderObject::~VulkanShaderObject()
    {
    }
}

