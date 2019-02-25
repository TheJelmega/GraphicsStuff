
#include "VulkanSemaphore.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

namespace Vulkan {


	VulkanSemaphore::VulkanSemaphore()
		: m_Semaphore(VK_NULL_HANDLE)
	{
	}

	VulkanSemaphore::~VulkanSemaphore()
	{
	}

	b8 VulkanSemaphore::Create(RHI::RHIContext* pContext)
	{
		m_pContext = pContext;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		VkSemaphoreCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult vkres = pDevice->vkCreateSemapore(createInfo, m_Semaphore);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the vulkan semaphore (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}
		return true;
	}

	b8 VulkanSemaphore::Destroy()
	{
		if (m_Semaphore)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			pDevice->vkDestroySemaphore(m_Semaphore);
			m_Semaphore = VK_NULL_HANDLE;
		}

		return true;
	}
}
