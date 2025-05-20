
#pragma once
#include "Base/BaseTypes.h"
#include "Containers/Vector.h"

// Created on 2025-05-11 by sisco

namespace pdl
{
    enum class CullMode : uint8
    {
        None,
        Front,
        Back
    };

    enum class FrontFace : uint8
    {
        Clockwise,
        CounterClockwise
    };

    enum class CompareOp : uint8
    {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    enum class PolygonMode : uint8
    {
        Fill,
        Line,
        Point
    };
    
    struct DepthTest
    {
        bool m_enabled = true;
        bool m_writeEnabled = true;
        CompareOp m_compareOp = CompareOp::LessOrEqual;

        bool operator == (const DepthTest& other) const
        {
            return m_enabled == other.m_enabled &&
                m_writeEnabled == other.m_writeEnabled &&
                m_compareOp == other.m_compareOp;
        }
        bool operator != (const DepthTest& other) const
        {
            return !(*this == other);       
        }
    };
    
    struct StencilTest
    {
        enum class StencilOp : uint8
        {
            Keep,
            Zero,
            Replace,
            IncrementClamp,
            DecrementClamp,
            Invert,
            IncrementWrap,
            DecrementWrap
        };
        enum class StencilFace : uint8
        {
            Front,
            Back,
            FrontAndBack
        };
        bool m_enabled = false;
        StencilOp m_failOp = StencilOp::Keep;
        StencilOp m_depthFailOp = StencilOp::Keep;
        StencilOp m_passOp = StencilOp::Keep;
        CompareOp m_compareOp = CompareOp::Always;
        StencilFace m_face = StencilFace::FrontAndBack;
        uint8 m_readMask = 0;
        uint8 m_writeMask = 0;
        uint8 m_reference = 0;

        bool operator == (const StencilTest& other) const
        {
            return m_enabled == other.m_enabled &&
                m_failOp == other.m_failOp &&
                m_depthFailOp == other.m_depthFailOp &&
                m_passOp == other.m_passOp &&
                m_compareOp == other.m_compareOp &&
                m_face == other.m_face &&
                m_readMask == other.m_readMask &&
                m_writeMask == other.m_writeMask &&
                m_reference == other.m_reference;
        }
        bool operator != (const StencilTest& other) const
        {
            return !(*this == other);
        }       
    };
    
    struct DepthBias
    {
        bool m_enabled = false;
        float m_constantFactor = 0;
        float m_clamp = 0;
        float m_slopeFactor = 0;

        bool operator == (const DepthBias& other) const
        {
            return m_enabled == other.m_enabled &&
                m_constantFactor == other.m_constantFactor &&
                m_clamp == other.m_clamp &&
                m_slopeFactor == other.m_slopeFactor;
        }
        bool operator != (const DepthBias& other) const
        {
            return !(*this == other);       
        }
    };

    struct BlendMode
    {
        enum class BlendFactor : uint8
        {
            Zero,
            One,
            SrcColor,
            OneMinusSrcColor,
            DstColor,
            OneMinusDstColor,
            SrcAlpha,
        };
        enum class BlendOp : uint8
        {
            Add,
            Subtract,
            ReverseSubtract,
            Min,
            Max
        };
        struct BlendEquation
        {
            BlendFactor m_srcFactor = BlendFactor::One;
            BlendFactor m_dstFactor = BlendFactor::Zero;
            BlendOp m_op = BlendOp::Add;
            BlendFactor m_srcFactorAlpha = BlendFactor::One;
            BlendFactor m_dstFactorAlpha = BlendFactor::Zero;
            BlendOp m_opAlpha = BlendOp::Add;
        };
        bool m_enabled = false;
        BlendEquation m_equation {};
        bool m_colorWriteMask = true;

        bool operator == (const BlendMode& other) const
        {
            return m_enabled != other.m_enabled &&
                m_equation.m_srcFactor == other.m_equation.m_srcFactor &&
                m_equation.m_dstFactor == other.m_equation.m_dstFactor &&
                m_equation.m_op == other.m_equation.m_op &&
                m_equation.m_srcFactorAlpha == other.m_equation.m_srcFactorAlpha &&
                m_equation.m_dstFactorAlpha == other.m_equation.m_dstFactorAlpha &&
                m_equation.m_opAlpha == other.m_equation.m_opAlpha &&
                m_colorWriteMask == other.m_colorWriteMask;           
        }
        
        bool operator != (const BlendMode& other) const
        {
            return !(*this == other);
        }
    };
    
struct PipelineState
{
    PolygonMode m_polygonMode = PolygonMode::Fill;   
    CullMode m_cullMode = CullMode::Back;
    FrontFace m_frontFace = FrontFace::Clockwise;
    DepthTest m_depthTest {};
    DepthBias m_depthBias {};
    StencilTest m_stencilTest {};
    BlendMode m_blendMode {};
    Vector<BlendMode> m_perAttachmentBlendModes {};
};

}

