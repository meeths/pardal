
#include <Image/ImageWriter.h>
#include <Base/DebugHelpers.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <Image/Image.h>
#include <Log/Log.h>

#include "String/StringUtils.h"

// Created on 2025-05-19 by franciscom

namespace pdl
{
	bool ImageWriter::WriteImage(StringView filename, Image* image, IFileLocator* fileLocator)
	{
		pdlAssert(!fileLocator && "File locator not supported");

		const auto& imageInfo = image->GetInfo();
		const auto imageDataRaw = image->GetData().data();
		int w = imageInfo.m_width;
		int h= imageInfo.m_height;
		
		int comp = 4;
		bool isHDR = false;
		switch (imageInfo.m_format)
		{
			case Format::R8_UINT:
			case Format::R8_UNORM: comp = 1; break;
			case Format::R8G8B8A8_UINT:
			case Format::R8G8B8A8_UNORM:
			case Format::R8G8B8A8_UNORM_SRGB: comp = 4; break;
			case Format::R32G32B32A32_FLOAT: comp = 4; isHDR = true; break;
			case Format::R32G32B32_FLOAT: comp = 3; isHDR = true; break;
			case Format::R32G32_FLOAT: comp = 2; isHDR = true; break;
			case Format::R32_FLOAT: comp = 1; isHDR = true; break;
			default: pdlLogError("WriteImage: Unsupported image format"); return false;
		}

		if (!isHDR)
		{
			if (StringUtils::EndsWith(filename, ".jpg"))
			{
				static constexpr int jpegQuality = 80;
				return stbi_write_jpg(filename.data(), w, h, comp, imageDataRaw, jpegQuality) != 0;
			}
			else if (StringUtils::EndsWith(filename, ".tga"))
			{
				return stbi_write_tga(filename.data(), w, h, comp, imageDataRaw) != 0;
			}
			else if (StringUtils::EndsWith(filename, ".bmp"))
			{
				return stbi_write_bmp(filename.data(), w, h, comp, imageDataRaw) != 0;
			}
			else
			{
				return stbi_write_png(filename.data(), w, h, comp, imageDataRaw, w * comp) != 0;
			}
		}
		else
		{
			return stbi_write_hdr(filename.data(), w, h, comp, reinterpret_cast<const float*>(imageDataRaw));
		}
	}
}

