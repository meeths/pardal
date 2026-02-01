
#pragma once
#include "Renderer/RendererTypes.h"
#include <vulkan/vulkan.hpp>
// Created on 2026-01-30 by sisco

namespace pdl
{

    struct VulkanCommandBufferObject
    {
        vk::CommandBuffer m_commandBuffer;
        SubmitHandle m_submitHandle;
        vk::Fence m_fence;
        vk::Semaphore m_semaphore;
        bool m_isAvailable = true;
    };

}

