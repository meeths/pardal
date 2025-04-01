
#pragma once
#include <Renderer/IRenderBuffer.h>
#include <vulkan/vulkan.hpp>

// Created on 2025-04-01 by franciscom

namespace pdl
{

class VulkanRenderBuffer : public IRenderBuffer
{
public:
	VulkanRenderBuffer(BufferDescriptor _bufferDescriptor, vk::Device device, vk::PhysicalDevice physicalDevice);
	~VulkanRenderBuffer() override;
	uint8* Map(uint32 size = 0, uint32 offset = 0) override;
	void Unmap() override;

private:
	vk::Buffer m_vkBuffer;
	vk::DeviceMemory m_vkMemory;
	vk::Device m_vkDevice;
	BufferDescriptor m_bufferDescriptor;
};

}

