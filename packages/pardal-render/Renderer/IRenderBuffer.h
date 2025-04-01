
#pragma once
#include <Renderer/RendererTypes.h>

// Created on 2025-04-01 by franciscom

namespace pdl
{

class IRenderBuffer
{
public:
	virtual ~IRenderBuffer() = default; 
	struct BufferDescriptor
	{
		uint32 size;
		BufferUsage usage;
		MemoryType memoryType;
	};

	virtual uint8* Map(uint32 size = 0, uint32 offset = 0) = 0;
	virtual void Unmap() = 0;
};

}

