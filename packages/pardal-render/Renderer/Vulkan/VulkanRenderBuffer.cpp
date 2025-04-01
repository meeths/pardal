
#include <Renderer/Vulkan/VulkanRenderBuffer.h>
#include <Renderer/Vulkan/VulkanUtils.h>

// Created on 2025-04-01 by franciscom

namespace pdl
{
	VulkanRenderBuffer::VulkanRenderBuffer(BufferDescriptor _bufferDescriptor, vk::Device device, vk::PhysicalDevice physicalDevice) : m_vkDevice(device), m_bufferDescriptor(_bufferDescriptor)
	{
		vk::BufferCreateInfo createInfo;
		createInfo.size = _bufferDescriptor.size;
		createInfo.usage = VulkanUtils::GetBufferUsageFlags(_bufferDescriptor.usage);
		createInfo.sharingMode = vk::SharingMode::eExclusive;
		createInfo.flags = static_cast<vk::BufferCreateFlagBits>(0);

		auto createBufferResults = m_vkDevice.createBuffer(createInfo);
		CHECK_VK_RESULTVALUE(createBufferResults);
		m_vkBuffer = createBufferResults.value;

		vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(m_vkBuffer);
		vk::MemoryPropertyFlags memoryTypeFlags = {};
		switch (_bufferDescriptor.memoryType)
		{
		case MemoryType::DeviceLocal:
			memoryTypeFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
			break;
		case MemoryType::Upload:
			memoryTypeFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
			break;
		case MemoryType::ReadBack:
			memoryTypeFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
			break;
		}
		uint32 memoryTypeIndex = VulkanUtils::FindMemoryType( physicalDevice.getMemoryProperties(), memoryRequirements.memoryTypeBits, memoryTypeFlags);

		auto uniformDataMemoryResults = device.allocateMemory(vk::MemoryAllocateInfo( memoryRequirements.size, memoryTypeIndex));
		CHECK_VK_RESULTVALUE(uniformDataMemoryResults);
		m_vkMemory = uniformDataMemoryResults.value;

		auto bindResults = device.bindBufferMemory(m_vkBuffer, m_vkMemory, 0);
		CHECK_VK_RESULT(bindResults);
	}

	VulkanRenderBuffer::~VulkanRenderBuffer()
	{
		if (m_vkMemory)
		{
			m_vkDevice.freeMemory(m_vkMemory);
		}
		if (m_vkBuffer != VK_NULL_HANDLE)
		{
			m_vkDevice.destroyBuffer(m_vkBuffer);
		}
	}

	uint8* VulkanRenderBuffer::Map(uint32 size, uint32 offset)
	{
		auto mapResults = m_vkDevice.mapMemory(m_vkMemory, offset, size > 0 ? size : VK_WHOLE_SIZE);
		CHECK_VK_RESULTVALUE(mapResults);
		return static_cast<uint8*>(mapResults.value);
	}

	void VulkanRenderBuffer::Unmap()
	{
		m_vkDevice.unmapMemory(m_vkMemory);
	}

}

