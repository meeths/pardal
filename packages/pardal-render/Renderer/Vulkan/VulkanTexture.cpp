#include <Renderer/Vulkan/VulkanTexture.h>

#include "VulkanUtils.h"

// Created on 2025-03-23 by sisco

namespace pdl
{
	VulkanTexture::VulkanTexture(const TextureDescriptor& desc, vk::Device* device)
		: m_textureDescriptor(desc), m_vkDevice(device)
	{
	}

	VulkanTexture::~VulkanTexture()
	{
		if (m_ownsMemory)
		{
			Destroy();
		}
	}

	bool VulkanTexture::Initialize(const vk::PhysicalDevice* physicalDevice)
	{
		auto vkFormat = VulkanUtils::TranslateToVkFormat(m_textureDescriptor.m_format);
		m_textureDescriptor = VulkanUtils::SanitizeTextureDescriptor(m_textureDescriptor);

		if (vkFormat == vk::Format::eUndefined)
		{
			pdlLogError("Failed to translate format to Vulkan format");
			return false;
		}
		m_vkFormat = vkFormat;

		vk::ImageCreateInfo imageInfo;
		switch (m_textureDescriptor.m_textureType)
		{
		case TextureType::Texture1D:
			imageInfo.imageType = vk::ImageType::e1D;
			imageInfo.extent.width = m_textureDescriptor.m_extents.x;
			imageInfo.extent.height = 1;
			imageInfo.extent.depth = 1;
			break;
		case TextureType::Texture2D:
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.extent.width = m_textureDescriptor.m_extents.x;
			imageInfo.extent.height = m_textureDescriptor.m_extents.y;
			imageInfo.extent.depth = 1;
			break;
		case TextureType::Texture3D:
			imageInfo.imageType = vk::ImageType::e3D;
			imageInfo.extent.width = m_textureDescriptor.m_extents.x;
			imageInfo.extent.height = m_textureDescriptor.m_extents.y;
			imageInfo.extent.depth = m_textureDescriptor.m_extents.z;
			break;
		case TextureType::TextureCube:
			pdlNotImplemented();
			return false;
		default:
			pdlLogError("Unsupported texture type");
			return false;
		}

		imageInfo.format = vkFormat;
		imageInfo.mipLevels = m_textureDescriptor.m_mipLevels;
		imageInfo.arrayLayers = m_textureDescriptor.m_arraySize;
		imageInfo.samples = static_cast<vk::SampleCountFlagBits>(m_textureDescriptor.m_sampleDesc.numSamples);

		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.usage = VulkanUtils::GetImageUsageFlags(m_textureDescriptor.m_textureUsage);
		imageInfo.sharingMode = vk::SharingMode::eExclusive;

		auto createImageResult = m_vkDevice->createImage(imageInfo);
		if (createImageResult.result != vk::Result::eSuccess)
		{
			pdlLogError("Error creating image: %s", to_string(createImageResult.result).c_str());
			return false;
		}

		m_vkImage = createImageResult.value;

		auto memoryRequirements = m_vkDevice->getImageMemoryRequirements(m_vkImage);
		if (memoryRequirements.size == 0)
		{
			pdlLogError("Failed to get image memory requirements");
			return false;
		}

		vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice->getMemoryProperties();
		uint32 typeBits = memoryRequirements.memoryTypeBits;
		uint32 typeIndex = static_cast<uint32>(~0);
		for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal))
			{
				typeIndex = i;
				break;
			}
			typeBits >>= 1;
		}
		assert(typeIndex != static_cast<uint32_t>(~0));
		auto textureMemoryResults = m_vkDevice->allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, typeIndex));
		CHECK_VK_RESULTVALUE(textureMemoryResults);

		m_vkDeviceMemory = textureMemoryResults.value;
		m_ownsMemory = true;

		auto bindMemoryResults = m_vkDevice->bindImageMemory(m_vkImage, m_vkDeviceMemory, 0);
		CHECK_VK_RESULT(bindMemoryResults);

		return true;
	}

	bool VulkanTexture::Initialize(vk::Image image)
	{
		auto vkFormat = VulkanUtils::TranslateToVkFormat(m_textureDescriptor.m_format);
		m_textureDescriptor = VulkanUtils::SanitizeTextureDescriptor(m_textureDescriptor);

		if (vkFormat == vk::Format::eUndefined)
		{
			pdlLogError("Failed to translate format to Vulkan format");
			return false;
		}

		m_vkImage = image;
		m_ownsMemory = false;
		m_vkFormat = vkFormat;

		return true;
	}

	void VulkanTexture::Destroy()
	{
		m_vkDevice->freeMemory(m_vkDeviceMemory);
		m_vkDevice->destroyImage(m_vkImage, nullptr);
		m_vkImage = nullptr;
	}
}
