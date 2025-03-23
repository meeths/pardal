
#pragma once
#include <vulkan/vulkan.hpp>
#include <Base/DebugHelpers.h>
#include <Log/Log.h>

// Created on 2025-03-23 by sisco
#ifndef NDEBUG
    #define CHECK_VK_RESULTVALUE(x) do{if ((x).result != vk::Result::eSuccess) { pdlLogError("Vulkan Error: %s", to_string((x).result).c_str()); pdlLogFlush(); pdlAssert(0); }} while(0)
    #define CHECK_VK_RESULT(x) do{if ((x) != vk::Result::eSuccess) { pdlLogError("Vulkan Error: %s", to_string(x).c_str()); pdlLogFlush(); pdlAssert(0); }} while(0)
#else
    #define CHECK_VK_RESULTVALUE(x) do{ sizeof(x); } while(0)
    #define CHECK_VK_RESULT(x) do{ sizeof(x); } while(0)
#endif

namespace pdl
{
class VulkanUtils
{
public:

};

}

