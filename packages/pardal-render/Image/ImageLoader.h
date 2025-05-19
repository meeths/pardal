
#pragma once
#include "Memory/SharedPointer.h"
#include "String/String.h"
#include "Base/Expected.h"

// Created on 2025-05-19 by franciscom

namespace pdl
{
class IFileLocator;
class Image;

class ImageLoader
{
public:
	static Expected<SharedPointer<Image>, StringView> LoadImage(StringView filename, IFileLocator* fileLocator = nullptr);
};

}

