#pragma once
#include <Base/BaseTypes.h>

// Created on 2025-03-23 by sisco

namespace pdl
{
   enum class Format : uint8
    {
        Unknown,

        R32G32B32A32_TYPELESS,
        R32G32B32_TYPELESS,
        R32G32_TYPELESS,
        R32_TYPELESS,

        R16G16B16A16_TYPELESS,
        R16G16_TYPELESS,
        R16_TYPELESS,

        R8G8B8A8_TYPELESS,
        R8G8_TYPELESS,
        R8_TYPELESS,
        B8G8R8A8_TYPELESS,

        R32G32B32A32_FLOAT,
        R32G32B32_FLOAT,
        R32G32_FLOAT,
        R32_FLOAT,

        R16G16B16A16_FLOAT,
        R16G16_FLOAT,
        R16_FLOAT,

        R32G32B32A32_UINT,
        R32G32B32_UINT,
        R32G32_UINT,
        R32_UINT,

        R16G16B16A16_UINT,
        R16G16_UINT,
        R16_UINT,

        R8G8B8A8_UINT,
        R8G8_UINT,
        R8_UINT,

        R32G32B32A32_SINT,
        R32G32B32_SINT,
        R32G32_SINT,
        R32_SINT,

        R16G16B16A16_SINT,
        R16G16_SINT,
        R16_SINT,

        R8G8B8A8_SINT,
        R8G8_SINT,
        R8_SINT,

        R16G16B16A16_UNORM,
        R16G16_UNORM,
        R16_UNORM,

        R8G8B8A8_UNORM,
        R8G8B8A8_UNORM_SRGB,
        R8G8_UNORM,
        R8_UNORM,
        B8G8R8A8_UNORM,
        B8G8R8A8_UNORM_SRGB,
        B8G8R8X8_UNORM,
        B8G8R8X8_UNORM_SRGB,

        R16G16B16A16_SNORM,
        R16G16_SNORM,
        R16_SNORM,

        R8G8B8A8_SNORM,
        R8G8_SNORM,
        R8_SNORM,

        D32_FLOAT,
        D16_UNORM,
        D32_FLOAT_S8_UINT,
        R32_FLOAT_X32_TYPELESS,

        B4G4R4A4_UNORM,
        B5G6R5_UNORM,
        B5G5R5A1_UNORM,

        R9G9B9E5_SHAREDEXP,
        R10G10B10A2_TYPELESS,
        R10G10B10A2_UNORM,
        R10G10B10A2_UINT,
        R11G11B10_FLOAT,

        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC2_UNORM,
        BC2_UNORM_SRGB,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_UNORM,
        BC4_SNORM,
        BC5_UNORM,
        BC5_SNORM,
        BC6H_UF16,
        BC6H_SF16,
        BC7_UNORM,
        BC7_UNORM_SRGB,

        R64_UINT,
        R64_SINT,

        FormatCount,
    };

    constexpr const char* to_string(Format e)
    {
        switch (e)
        {
        case Format::Unknown: return "Unknown";
        case Format::R32G32B32A32_TYPELESS: return "R32G32B32A32_TYPELESS";
        case Format::R32G32B32_TYPELESS: return "R32G32B32_TYPELESS";
        case Format::R32G32_TYPELESS: return "R32G32_TYPELESS";
        case Format::R32_TYPELESS: return "R32_TYPELESS";
        case Format::R16G16B16A16_TYPELESS: return "R16G16B16A16_TYPELESS";
        case Format::R16G16_TYPELESS: return "R16G16_TYPELESS";
        case Format::R16_TYPELESS: return "R16_TYPELESS";
        case Format::R8G8B8A8_TYPELESS: return "R8G8B8A8_TYPELESS";
        case Format::R8G8_TYPELESS: return "R8G8_TYPELESS";
        case Format::R8_TYPELESS: return "R8_TYPELESS";
        case Format::B8G8R8A8_TYPELESS: return "B8G8R8A8_TYPELESS";
        case Format::R32G32B32A32_FLOAT: return "R32G32B32A32_FLOAT";
        case Format::R32G32B32_FLOAT: return "R32G32B32_FLOAT";
        case Format::R32G32_FLOAT: return "R32G32_FLOAT";
        case Format::R32_FLOAT: return "R32_FLOAT";
        case Format::R16G16B16A16_FLOAT: return "R16G16B16A16_FLOAT";
        case Format::R16G16_FLOAT: return "R16G16_FLOAT";
        case Format::R16_FLOAT: return "R16_FLOAT";
        case Format::R32G32B32A32_UINT: return "R32G32B32A32_UINT";
        case Format::R32G32B32_UINT: return "R32G32B32_UINT";
        case Format::R32G32_UINT: return "R32G32_UINT";
        case Format::R32_UINT: return "R32_UINT";
        case Format::R16G16B16A16_UINT: return "R16G16B16A16_UINT";
        case Format::R16G16_UINT: return "R16G16_UINT";
        case Format::R16_UINT: return "R16_UINT";
        case Format::R8G8B8A8_UINT: return "R8G8B8A8_UINT";
        case Format::R8G8_UINT: return "R8G8_UINT";
        case Format::R8_UINT: return "R8_UINT";
        case Format::R32G32B32A32_SINT: return "R32G32B32A32_SINT";
        case Format::R32G32B32_SINT: return "R32G32B32_SINT";
        case Format::R32G32_SINT: return "R32G32_SINT";
        case Format::R32_SINT: return "R32_SINT";
        case Format::R16G16B16A16_SINT: return "R16G16B16A16_SINT";
        case Format::R16G16_SINT: return "R16G16_SINT";
        case Format::R16_SINT: return "R16_SINT";
        case Format::R8G8B8A8_SINT: return "R8G8B8A8_SINT";
        case Format::R8G8_SINT: return "R8G8_SINT";
        case Format::R8_SINT: return "R8_SINT";
        case Format::R16G16B16A16_UNORM: return "R16G16B16A16_UNORM";
        case Format::R16G16_UNORM: return "R16G16_UNORM";
        case Format::R16_UNORM: return "R16_UNORM";
        case Format::R8G8B8A8_UNORM: return "R8G8B8A8_UNORM";
        case Format::R8G8B8A8_UNORM_SRGB: return "R8G8B8A8_UNORM_SRGB";
        case Format::R8G8_UNORM: return "R8G8_UNORM";
        case Format::R8_UNORM: return "R8_UNORM";
        case Format::B8G8R8A8_UNORM: return "B8G8R8A8_UNORM";
        case Format::B8G8R8A8_UNORM_SRGB: return "B8G8R8A8_UNORM_SRGB";
        case Format::B8G8R8X8_UNORM: return "B8G8R8X8_UNORM";
        case Format::B8G8R8X8_UNORM_SRGB: return "B8G8R8X8_UNORM_SRGB";
        case Format::R16G16B16A16_SNORM: return "R16G16B16A16_SNORM";
        case Format::R16G16_SNORM: return "R16G16_SNORM";
        case Format::R16_SNORM: return "R16_SNORM";
        case Format::R8G8B8A8_SNORM: return "R8G8B8A8_SNORM";
        case Format::R8G8_SNORM: return "R8G8_SNORM";
        case Format::R8_SNORM: return "R8_SNORM";
        case Format::D32_FLOAT: return "D32_FLOAT";
        case Format::D16_UNORM: return "D16_UNORM";
        case Format::D32_FLOAT_S8_UINT: return "D32_FLOAT_S8_UINT";
        case Format::R32_FLOAT_X32_TYPELESS: return "R32_FLOAT_X32_TYPELESS";
        case Format::B4G4R4A4_UNORM: return "B4G4R4A4_UNORM";
        case Format::B5G6R5_UNORM: return "B5G6R5_UNORM";
        case Format::B5G5R5A1_UNORM: return "B5G5R5A1_UNORM";
        case Format::R9G9B9E5_SHAREDEXP: return "R9G9B9E5_SHAREDEXP";
        case Format::R10G10B10A2_TYPELESS: return "R10G10B10A2_TYPELESS";
        case Format::R10G10B10A2_UNORM: return "R10G10B10A2_UNORM";
        case Format::R10G10B10A2_UINT: return "R10G10B10A2_UINT";
        case Format::R11G11B10_FLOAT: return "R11G11B10_FLOAT";
        case Format::BC1_UNORM: return "BC1_UNORM";
        case Format::BC1_UNORM_SRGB: return "BC1_UNORM_SRGB";
        case Format::BC2_UNORM: return "BC2_UNORM";
        case Format::BC2_UNORM_SRGB: return "BC2_UNORM_SRGB";
        case Format::BC3_UNORM: return "BC3_UNORM";
        case Format::BC3_UNORM_SRGB: return "BC3_UNORM_SRGB";
        case Format::BC4_UNORM: return "BC4_UNORM";
        case Format::BC4_SNORM: return "BC4_SNORM";
        case Format::BC5_UNORM: return "BC5_UNORM";
        case Format::BC5_SNORM: return "BC5_SNORM";
        case Format::BC6H_UF16: return "BC6H_UF16";
        case Format::BC6H_SF16: return "BC6H_SF16";
        case Format::BC7_UNORM: return "BC7_UNORM";
        case Format::BC7_UNORM_SRGB: return "BC7_UNORM_SRGB";
        case Format::R64_UINT: return "R64_UINT";
        case Format::R64_SINT: return "R64_SINT";
        case Format::FormatCount: return "FormatCount";
        default: return "unknown";
        }
    }

    enum class IndexFormat : uint8
    {
        UInt16,
        UInt32,
    };

    enum class FormatSupport : uint16
    {
        None                = 0,

        Buffer              = 1 << 0,
        IndexBuffer         = 1 << 1,
        VertexBuffer        = 1 << 2,

        Texture             = 1 << 3,
        DepthStencil        = 1 << 4,
        RenderTarget        = 1 << 5,
        Blendable           = 1 << 6,

        ShaderLoad          = 1 << 7,
        ShaderSample        = 1 << 8,
        ShaderUavLoad       = 1 << 9,
        ShaderUavStore      = 1 << 10,
        ShaderAtomic        = 1 << 11,
    };

    enum class PrimitiveTopology : uint8
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        PatchList,
    };

    enum class ResourceState : uint8
    {
        Undefined,
        General,
        VertexBuffer,
        IndexBuffer,
        ConstantBuffer,
        StreamOutput,
        ShaderResource,
        UnorderedAccess,
        RenderTarget,
        DepthRead,
        DepthWrite,
        Present,
        IndirectArgument,
        CopySource,
        CopyDestination,
        ResolveSource,
        ResolveDestination,
        AccelerationStructure,
        AccelerationStructureBuildInput,
    };

    enum class MemoryType : uint8
    {
        DeviceLocal,
        Upload,
        ReadBack,
    };

    enum class BufferUsage : uint16
    {
        None                                = 0,
        VertexBuffer                        = 1 << 0,
        IndexBuffer                         = 1 << 1,
        ConstantBuffer                      = 1 << 2,
        ShaderResource                      = 1 << 3,
        UnorderedAccess                     = 1 << 4,
        IndirectArgument                    = 1 << 5,
        CopySource                          = 1 << 6,
        CopyDestination                     = 1 << 7,
        AccelerationStructure               = 1 << 8,
        AccelerationStructureBuildInput     = 1 << 9,
        ShaderTable                         = 1 << 10,
    };
    
    inline BufferUsage operator|(BufferUsage lhs, BufferUsage rhs) 
    {
        return static_cast<BufferUsage>(static_cast<char>(lhs) | static_cast<char>(rhs));
    }
    inline BufferUsage operator&(BufferUsage lhs, BufferUsage rhs) 
    {
        return static_cast<BufferUsage>(static_cast<char>(lhs) & static_cast<char>(rhs));
    }

    enum class TextureUsage : uint16
    {
        None = 0,
        ShaderResource                      = 1 << 0,
        UnorderedAccess                     = 1 << 1,
        RenderTarget                        = 1 << 2,
        DepthRead                           = 1 << 3,
        DepthWrite                          = 1 << 4,
        Present                             = 1 << 5,
        CopySource                          = 1 << 6,
        CopyDestination                     = 1 << 7,
        ResolveSource                       = 1 << 8,
        ResolveDestination                  = 1 << 9,
    };
    inline TextureUsage operator|(TextureUsage lhs, TextureUsage rhs) 
    {
        return static_cast<TextureUsage>(static_cast<char>(lhs) | static_cast<char>(rhs));
    }
    inline TextureUsage operator&(TextureUsage lhs, TextureUsage rhs) 
    {
        return static_cast<TextureUsage>(static_cast<char>(lhs) & static_cast<char>(rhs));
    }

    enum class TextureType : uint8
    {
        Texture1D,
        Texture2D,
        Texture3D,
        TextureCube
    };

    
    enum class ComparisonFunc : uint8
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,
    };

    enum class TextureFilteringMode : uint8
    {
        Point,
        Linear,
    };

    enum class TextureAddressingMode : uint8
    {
        Wrap,
        ClampToEdge,
        ClampToBorder,
        MirrorRepeat,
        MirrorOnce,
    };

    enum class TextureReductionOp : uint8
    {
        Average,
        Comparison,
        Minimum,
        Maximum,
    };

    enum class TextureAspect : uint32
    {
        Default = 0,
        Color = 0x00000001,
        Depth = 0x00000002,
        Stencil = 0x00000004,
        MetaData = 0x00000008,
        Plane0 = 0x00000010,
        Plane1 = 0x00000020,
        Plane2 = 0x00000040,

        DepthStencil = Depth | Stencil,
        All = Color | Depth | Stencil | MetaData | Plane0 | Plane1 | Plane2
    };
    
    struct SubresourceRange
    {
        TextureAspect m_aspectMask;
        uint32 m_mipLevel;
        size_t m_mipLevelCount;
        uint32 m_baseArrayLayer;
        size_t m_layerCount;
    };

    struct BufferRange
    {
        size_t m_offset;
        size_t m_size;
    };

}

