
#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

namespace Vulkan {


	VulkanQueue::VulkanQueue()
		: m_Queue(VK_NULL_HANDLE)
		, m_Family(0)
	{
	}

	VulkanQueue::~VulkanQueue()
	{
	}

	b8 VulkanQueue::Init(RHI::RHIContext* pContext, RHI::QueueType type, u32 index, RHI::QueuePriority priority)
	{
		m_pContext = pContext;
		m_Type = type;
		m_Priority = priority;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		m_Family = pDevice->GetQueueFamily(m_Type);
		m_Queue = pDevice->vkGetQueue(m_Family, index);

		return true;
	}

	b8 VulkanQueue::WaitIdle()
	{
		VkResult vkres = vkQueueWaitIdle(m_Queue);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to wait for the queue to be idle (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}
		return true;
	}

	VkResult VulkanQueue::vkSubmit(const VkSubmitInfo& submitInfo, VkFence fence)
	{
		return vkQueueSubmit(m_Queue, 1, &submitInfo, fence);
	}

	VkResult VulkanQueue::vkSubmit(const std::vector<VkSubmitInfo>& submitInfo, VkFence fence)
	{
		return vkQueueSubmit(m_Queue, u32(submitInfo.size()), submitInfo.data(), fence);
	}

	VkResult VulkanQueue::vkPresent(const VkPresentInfoKHR& presentInfo)
	{
		return vkQueuePresentKHR(m_Queue, &presentInfo);
	}
}
