
#pragma once
#include <vulkan/vulkan.hpp>
// Created on 2025-05-24 by sisco

namespace pdl
{

class IVulkanDescriptorSet
{
public:
    virtual ~IVulkanDescriptorSet() = default;
    virtual vk::DescriptorSet GetVkDescriptorSet() const = 0;
    virtual vk::DescriptorSetLayout GetVkDescriptorSetLayout() const = 0;
};

}

