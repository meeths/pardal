
#pragma once
#include "Containers/Vector.h"
#include "Renderer/RendererTypes.h"

// Created on 2025-05-19 by franciscom

namespace pdl
{

class Image
{
public:
	friend class ImageLoader;

	struct ImageInfo
	{
		Format m_format = Format::R8G8B8A8_UINT;
		TextureType m_imageType = TextureType::Texture2D;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_depth = 0;
		uint32_t m_mipLevels = 0;
		uint32_t m_arrayLayers= 1;
	};
	
	Image(ImageInfo info, Vector<uint8> data) : m_info(info), m_data(std::move(data)) {}
	~Image() = default;

	
private:
	ImageInfo m_info {};
	Vector<uint8> m_data;
};

}

