
#include "VulkanFence.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

namespace Vulkan {


	VulkanFence::VulkanFence()
		: m_Fence(VK_NULL_HANDLE)
	{
	}

	VulkanFence::~VulkanFence()
	{
	}

	b8 VulkanFence::Create(RHI::RHIContext* pContext, b8 signaled)
	{
		m_pContext = pContext;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		if (signaled)
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkResult vkres = pDevice->vkCreateFence(createInfo, m_Fence);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create a vulkan fence (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		return true;
	}

	b8 VulkanFence::Destroy()
	{
		if (m_Fence)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			pDevice->vkDestroyFence(m_Fence);
			m_Fence = VK_NULL_HANDLE;
		}

		return true;
	}

	void VulkanFence::Tick()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		if (m_Status == RHI::FenceStatus::Submitted && pDevice->vkGetFenceStatus(m_Fence) == VK_SUCCESS)
		{
			m_Status = RHI::FenceStatus::Signaled;
			++m_FenceValue;
		}
	}

	b8 VulkanFence::Wait(u64 timeout)
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres = pDevice->vkWaitForFence(m_Fence, timeout);

		if (vkres == VK_SUCCESS)
		{
			m_Status = RHI::FenceStatus::Signaled;
			++m_FenceValue;
			return true;
		}

		return false;
	}

	b8 VulkanFence::Reset()
	{
		if (m_Status == RHI::FenceStatus::Signaled)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			VkResult vkres = pDevice->vkResetFence(m_Fence);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to reset a vulkan fence (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return false;
			}
			m_Status = RHI::FenceStatus::Unsignaled;
		}
		return true;
	}
}
