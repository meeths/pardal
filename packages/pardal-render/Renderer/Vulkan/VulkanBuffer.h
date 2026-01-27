
#pragma once
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

// Created on 2026-01-27 by sisco

namespace pdl
{

struct VulkanBuffer
{
    vk::Buffer m_buffer;
    vk::DeviceMemory m_memory;
    vk::DeviceAddress m_deviceAddress;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
    vk::DeviceSize m_size = 0;
    vk::BufferUsageFlags m_usage = {};
    vk::MemoryPropertyFlags m_memoryPropertyFlags = {};
    void* m_mappedPtr = nullptr;
};

}

