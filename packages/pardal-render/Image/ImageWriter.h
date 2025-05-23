
#pragma once
#include <String/String.h>

// Created on 2025-05-19 by franciscom

namespace pdl
{
class IFileLocator;
class Image;

class ImageWriter
{
public:
	static bool WriteImage(StringView filename, Image* image, IFileLocator* fileLocator = nullptr);
};
}

