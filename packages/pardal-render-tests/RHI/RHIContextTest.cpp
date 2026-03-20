#include <gtest/gtest.h>

#include "Memory/UniquePointer.h"
#include "Renderer/Shaders/SlangShaderCompiler.h"
#include "Renderer/Vulkan/VulkanRHIContext.h"

namespace pdl { namespace Tests
{

// ---------------------------------------------------------------------------
// Fixture: headless Vulkan context (no window / swapchain)
// ---------------------------------------------------------------------------

struct RHIContextTest : testing::Test
{
    void SetUp() override
    {
        m_rhi = VulkanRHIContext::CreateHeadless();
        if (!m_rhi)
        {
            GTEST_SKIP() << "Failed to create headless Vulkan context (no device available)";
        }
    }

    UniquePointer<VulkanRHIContext> m_rhi;
};

// ---------------------------------------------------------------------------
// Buffer tests
// ---------------------------------------------------------------------------

TEST_F(RHIContextTest, BufferCreateAndDestroy)
{
    auto result = m_rhi->CreateBuffer({
        .size    = 256,
        .usage   = BufferUsage::ShaderResource | BufferUsage::UnorderedAccess,
        .storage = MemoryType::HostVisible,
    });
    ASSERT_TRUE(result.has_value()) << result.error().data();
    EXPECT_TRUE(result->IsValid());

    m_rhi->Destroy(*result);
}

TEST_F(RHIContextTest, BufferUploadAndDownload)
{
    constexpr size_t kSize = 256;

    auto result = m_rhi->CreateBuffer({
        .size    = kSize,
        .usage   = BufferUsage::ShaderResource | BufferUsage::UnorderedAccess,
        .storage = MemoryType::HostVisible,
        .debugName = "UploadDownloadTest",
    });
    ASSERT_TRUE(result.has_value()) << result.error().data();

    Holder<BufferHandle> buf(m_rhi.get(), *result);

    // Fill with known pattern
    uint8_t writeData[kSize];
    for (size_t i = 0; i < kSize; ++i)
        writeData[i] = static_cast<uint8_t>(i & 0xFF);

    auto uploadRes = m_rhi->Upload(buf, writeData, kSize);
    ASSERT_TRUE(uploadRes.has_value()) << uploadRes.error().data();

    uint8_t readData[kSize] = {};
    auto downloadRes = m_rhi->Download(buf, readData, kSize);
    ASSERT_TRUE(downloadRes.has_value()) << downloadRes.error().data();

    for (size_t i = 0; i < kSize; ++i)
        EXPECT_EQ(readData[i], writeData[i]) << "Mismatch at byte " << i;
}

// ---------------------------------------------------------------------------
// Texture tests
// ---------------------------------------------------------------------------

TEST_F(RHIContextTest, TextureCreate)
{
    auto result = m_rhi->CreateTexture({
        .type      = TextureType::Texture2D,
        .format    = Format::R8G8B8A8_UNORM,
        .width     = 64,
        .height    = 64,
        .usage     = TextureUsage::ShaderResource | TextureUsage::UnorderedAccess,
        .debugName = "TestTexture",
    });
    ASSERT_TRUE(result.has_value()) << result.error().data();
    EXPECT_TRUE(result->IsValid());

    const IRHIContext::Dimensions dims = m_rhi->GetDimensions(*result);
    EXPECT_EQ(dims.width, 64u);
    EXPECT_EQ(dims.height, 64u);

    m_rhi->Destroy(*result);
}

// ---------------------------------------------------------------------------
// Shader module tests
// ---------------------------------------------------------------------------

TEST_F(RHIContextTest, ShaderModuleCreate)
{
    SlangShaderCompiler compiler({
        .m_target          = SlangShaderCompiler::Target::SPIRV,
        .m_compilerOptions = SlangShaderCompiler::CompilerOptions::TargetVulkan,
    });

    const char* source =
        "RWStructuredBuffer<float> result;"
        "[shader(\"compute\")]"
        "[numthreads(64,1,1)]"
        "void computeMain(uint3 tid : SV_DispatchThreadID)"
        "{"
        "    result[tid.x] = float(tid.x);"
        "}";

    auto spirv = compiler.CompileShader({ "TestShader", source, "computeMain" });
    ASSERT_TRUE(spirv.has_value()) << spirv.error().data();

    auto result = m_rhi->CreateShaderModule({
        .spirvData  = spirv->data(),
        .spirvSize  = spirv->size(),
        .debugName  = "TestComputeShader",
    });
    ASSERT_TRUE(result.has_value()) << result.error().data();
    EXPECT_TRUE(result->IsValid());

    m_rhi->Destroy(*result);
}

// ---------------------------------------------------------------------------
// Compute pipeline tests
// ---------------------------------------------------------------------------

TEST_F(RHIContextTest, ComputePipelineCreate)
{
    SlangShaderCompiler compiler({
        .m_target          = SlangShaderCompiler::Target::SPIRV,
        .m_compilerOptions = SlangShaderCompiler::CompilerOptions::TargetVulkan,
    });

    const char* source =
        "RWStructuredBuffer<float> result;"
        "[shader(\"compute\")]"
        "[numthreads(64,1,1)]"
        "void computeMain(uint3 tid : SV_DispatchThreadID)"
        "{"
        "    result[tid.x] = float(tid.x);"
        "}";

    auto spirv = compiler.CompileShader({ "TestShader", source, "computeMain" });
    ASSERT_TRUE(spirv.has_value()) << spirv.error().data();

    auto shaderResult = m_rhi->CreateShaderModule({
        .spirvData = spirv->data(),
        .spirvSize = spirv->size(),
    });
    ASSERT_TRUE(shaderResult.has_value()) << shaderResult.error().data();
    Holder<ShaderModuleHandle> shader(m_rhi.get(), *shaderResult);

    auto pipelineResult = m_rhi->CreateComputePipeline({
        .computeShader = shader,
        .debugName     = "TestComputePipeline",
    });
    ASSERT_TRUE(pipelineResult.has_value()) << pipelineResult.error().data();
    EXPECT_TRUE(pipelineResult->IsValid());

    m_rhi->Destroy(*pipelineResult);
}

// ---------------------------------------------------------------------------
// Compute dispatch + readback
// ---------------------------------------------------------------------------

TEST_F(RHIContextTest, ComputeDispatchAndReadback)
{
    constexpr uint32_t kCount = 64;

    SlangShaderCompiler compiler({
        .m_target          = SlangShaderCompiler::Target::SPIRV,
        .m_compilerOptions = SlangShaderCompiler::CompilerOptions::TargetVulkan,
    });

    // Shader writes threadId.x as float into each slot
    const char* source =
        "RWStructuredBuffer<float> result;"
        "[shader(\"compute\")]"
        "[numthreads(64,1,1)]"
        "void computeMain(uint3 tid : SV_DispatchThreadID)"
        "{"
        "    result[tid.x] = float(tid.x);"
        "}";

    auto spirv = compiler.CompileShader({ "DispatchShader", source, "computeMain" });
    ASSERT_TRUE(spirv.has_value()) << spirv.error().data();

    auto shaderResult = m_rhi->CreateShaderModule({ .spirvData = spirv->data(), .spirvSize = spirv->size() });
    ASSERT_TRUE(shaderResult.has_value());
    Holder<ShaderModuleHandle> shader(m_rhi.get(), *shaderResult);

    auto pipelineResult = m_rhi->CreateComputePipeline({ .computeShader = shader });
    ASSERT_TRUE(pipelineResult.has_value());
    Holder<ComputePipelineHandle> pipeline(m_rhi.get(), *pipelineResult);

    // Output buffer (host-visible so we can read back)
    auto bufResult = m_rhi->CreateBuffer({
        .size      = kCount * sizeof(float),
        .usage     = BufferUsage::ShaderResource | BufferUsage::UnorderedAccess,
        .storage   = MemoryType::HostVisible,
        .debugName = "DispatchOutput",
    });
    ASSERT_TRUE(bufResult.has_value());
    Holder<BufferHandle> outputBuf(m_rhi.get(), *bufResult);

    // Record and submit
    IRHICommandBuffer& cmd = m_rhi->AcquireCommandBuffer();
    cmd.CmdBindComputePipeline(pipeline);
    cmd.CmdDispatch(1, 1, 1); // 1 group × 64 threads
    m_rhi->Submit(cmd);

    m_rhi->WaitIdle();

    // Readback
    float readData[kCount] = {};
    auto downloadRes = m_rhi->Download(outputBuf, readData, kCount * sizeof(float));
    ASSERT_TRUE(downloadRes.has_value()) << downloadRes.error().data();

    for (uint32_t i = 0; i < kCount; ++i)
        EXPECT_FLOAT_EQ(readData[i], static_cast<float>(i)) << "Mismatch at slot " << i;
}

}} // namespace pdl::Tests
