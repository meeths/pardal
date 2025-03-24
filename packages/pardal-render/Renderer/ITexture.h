
#pragma once
#include <Renderer/RendererTypes.h>
#include <Math/Vector3i.h>

// Created on 2025-03-24 by franciscom

namespace pdl
{

class ITexture
{
public:
	struct SampleDesc
	{
		int32 numSamples = 1;
		int32 quality = 0;
	};

	struct TextureDescriptor
	{
		Format m_format = Format::Unknown;
		Math::Vector3i m_extents = Math::Vector3i(0);
		SampleDesc m_sampleDesc;
		int32 m_mipLevels = 0;
		int32 m_arraySize = 0;
		TextureType m_textureType = TextureType::Texture2D;

	};

	virtual ~ITexture() = default;
	virtual const TextureDescriptor& GetDescriptor() const = 0;
	virtual TextureType GetTextureType() const = 0;
	
};

}

