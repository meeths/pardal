
#pragma once
#include <Renderer/RenderDeviceType.h>
#include <Math/Matrix44.h>
// Created on 2025-03-23 by sisco

namespace pdl
{
    struct DeviceLimits
    {
        uint32 maxTextureDimension1D;
        uint32 maxTextureDimension2D;
        uint32 maxTextureDimension3D;
        uint32 maxTextureDimensionCube;
        uint32 maxTextureArrayLayers;

        uint32 maxVertexInputElements;
        uint32 maxVertexInputElementOffset;
        uint32 maxVertexStreams;
        uint32 maxVertexStreamStride;

        uint32 maxComputeThreadsPerGroup;
        uint32 maxComputeThreadGroupSize[3];
        uint32 maxComputeDispatchThreadGroups[3];

        uint32 maxViewports;
        uint32 maxViewportDimensions[2];
        uint32 maxFramebufferDimensions[3];

        uint32 maxShaderVisibleSamplers;
    };

    struct RenderDeviceInfo
    {
        RenderDeviceType deviceType;

        String name;
        String adapterName;

        DeviceLimits limits;

        Math::Matrix44 identityProjection;

    };

}

