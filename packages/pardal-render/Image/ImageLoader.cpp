
#include <Image/ImageLoader.h>

#include <Base/BaseTypes.h>
#include <Base/DebugHelpers.h>
#include <Filesystem/IFileLocator.h>
#include <Image/Image.h>
#include <Renderer/RendererTypes.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// Created on 2025-05-19 by franciscom

namespace details
{
	pdl::Expected<pdl::SharedPointer<pdl::Image>, pdl::StringView> LoadImageSTB(pdl::StringView filename, pdl::IFileLocator* fileLocator)
	{
		pdlAssert(!fileLocator && "File locator not supported yet");
		
		int w, h, comp;
		uint8 bytesPerComp;

		unsigned char* image_data;

		if (stbi_is_16_bit(filename.data()))
		{
			image_data = reinterpret_cast<unsigned char*>(stbi_load_16(filename.data(), &w, &h, &comp, STBI_default));
			bytesPerComp = 2;
		}
		else if (stbi_is_hdr(filename.data()))
		{
			image_data = reinterpret_cast<unsigned char*>(stbi_loadf(filename.data(), &w, &h, &comp, STBI_default));
			bytesPerComp = 4;
		}
		else
		{
			image_data = stbi_load(filename.data(), &w, &h, &comp, STBI_default);
			bytesPerComp = 1;
		}
		
		if (image_data == nullptr)
		{
			return pdl::Unexpected<pdl::StringView>(stbi_failure_reason());
		}
			

		int needsExtraAlphaChannel = 0;
		if (comp == 3 && bytesPerComp != 4) needsExtraAlphaChannel = 1;		// R8G8B8 or R16G16B16 3 components is unsupported, add alpha channel
		
		uint32 imageSize = w * h * (comp + needsExtraAlphaChannel) * (bytesPerComp);
		
		pdl::Format imageFormat = pdl::Format::Unknown;
		switch (bytesPerComp)
		{
		case 1:
			switch (comp)
			{
				case 1: imageFormat = pdl::Format::R8_UINT; break;
				case 2: imageFormat = pdl::Format::R8G8_UINT; break;
				case 3: 
				case 4: imageFormat = pdl::Format::R8G8B8A8_UINT; break;
				default: return pdl::Unexpected<pdl::StringView>("Unsupported image component count");
			}
			break;
		case 2:
			switch (comp)
			{
				case 1: imageFormat = pdl::Format::R16_UINT; break;
				case 2: imageFormat = pdl::Format::R16G16_UINT; break;
				case 3: 
				case 4: imageFormat = pdl::Format::R16G16B16A16_UINT; break;
				default: return pdl::Unexpected<pdl::StringView>("Unsupported image component count");
			}
			break;
		case 4:
			switch (comp)
			{
				case 1: imageFormat = pdl::Format::R32_FLOAT; break;
				case 2: imageFormat = pdl::Format::R32G32_FLOAT; break;
				case 3: imageFormat = pdl::Format::R32G32B32_FLOAT; break;
				case 4: imageFormat = pdl::Format::R32G32B32A32_FLOAT; break;
				default: return pdl::Unexpected<pdl::StringView>("Unsupported image component count");
			}
			break;
		}

		pdl::Image::ImageInfo info;
		info.m_format = imageFormat;
		info.m_width = w;
		info.m_height = h;
		info.m_arrayLayers = 1;
		info.m_mipLevels = 1;
		pdl::Vector<uint8> data(imageSize);
		
		if (!needsExtraAlphaChannel)
		{
			memcpy(data.data(), image_data, imageSize);
		}
		else
		{
			if (bytesPerComp == 1)
			{
				// Add extra A8 channel
				for (uint32 i = 0; i < (w * h); ++i)
				{
					data[i * 4 + 0] = image_data[ i * 3 + 0];
					data[i * 4 + 1] = image_data[ i * 3 + 1];
					data[i * 4 + 2] = image_data[ i * 3 + 2];
					data[i * 4 + 3] = 0xFF;
				}
			}
			else
			{
				// Add extra A16 channel
				uint16* image_data16 = reinterpret_cast<uint16*>(image_data);
				uint16* data16 = reinterpret_cast<uint16*>(data.data());
				
				for (uint32 i = 0; i < (w * h); ++i)
				{
					data16[i * 4 + 0] = image_data16[ i * 3 + 0];
					data16[i * 4 + 1] = image_data16[ i * 3 + 1];
					data16[i * 4 + 2] = image_data16[ i * 3 + 2];
					data16[i * 4 + 3] = 0xFFFF;
				}
	
			}
		}
		
		pdl::SharedPointer<pdl::Image> image = MakeSharedPointer<pdl::Image>(info, data);
		stbi_image_free(image_data);
		return image;
	}	
}

namespace pdl
{
	Expected<SharedPointer<Image>, StringView> ImageLoader::LoadImage(StringView filename, IFileLocator* fileLocator)
	{
		// TODO: Check for stb unsupported file types
		return details::LoadImageSTB(filename, fileLocator);
	}
}

