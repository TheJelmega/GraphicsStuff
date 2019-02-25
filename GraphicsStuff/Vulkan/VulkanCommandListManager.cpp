#include "VulkanCommandListManager.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanQueue.h"
#include "VulkanCommandList.h"

namespace Vulkan {


	VulkanCommandListManager::VulkanCommandListManager()
	{
	}

	VulkanCommandListManager::~VulkanCommandListManager()
	{
	}

	b8 VulkanCommandListManager::Create(RHI::RHIContext* pContext)
	{
		m_pContext = pContext;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VulkanPhysicalDevice* pPhysDevice = pDevice->GetPhysicalDevice();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		const std::vector<RHI::Queue*>& queues = pDevice->GetQueues();
		for (RHI::Queue* pQueue : queues)
		{
			u32 queueFamily = ((VulkanQueue*)pQueue)->GetQueueFamily();
			poolInfo.queueFamilyIndex = queueFamily;

			if (m_CommandPools.size() <= queueFamily)
				m_CommandPools.resize(queueFamily + 1);

			VkResult vkres = pDevice->vkCreateCommandPool(poolInfo, m_CommandPools[queueFamily]);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create a vulkan command pool (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return vkres;
			}
		}

		return true;
	}

	b8 VulkanCommandListManager::Destroy()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		for (RHI::CommandList* pCommandList : m_CommandLists)
		{
			((VulkanCommandList*)pCommandList)->Destroy();
			delete pCommandList;
		}

		for (VkCommandPool commandPool : m_CommandPools)
		{
			if (commandPool)
			{
				pDevice->vkDestroyCommandPool(commandPool);
			}
		}
		m_CommandPools.clear();

		return true;
	}

	RHI::CommandList* VulkanCommandListManager::CreateCommandList(RHI::Queue* pQueue)
	{
		VulkanCommandList* pCommandList = new VulkanCommandList();
		b8 res = pCommandList->Create(m_pContext, this, pQueue);
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to create command list!");
			delete pCommandList;
			return nullptr;
		}
		m_CommandLists.push_back(pCommandList);
		return pCommandList;
	}

	b8 VulkanCommandListManager::DestroyCommandList(RHI::CommandList* pCommandList)
	{
		auto it = std::find(m_CommandLists.begin(), m_CommandLists.end(), pCommandList);
		if (it == m_CommandLists.end())
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to remove a command list, manager does not contain a command list!");
			return false;
		}

		b8 res = ((VulkanCommandList*)pCommandList)->Destroy();
		delete pCommandList;

		m_CommandLists.erase(it);

		return res;
	}

	RHI::CommandList* VulkanCommandListManager::CreateSingleTimeCommandList(RHI::Queue* pQueue)
	{
		VulkanCommandList* pCommandList = new VulkanCommandList();
		b8 res = pCommandList->Create(m_pContext, this, pQueue);
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to create single time command list!");
			delete pCommandList;
			return nullptr;
		}

		res = pCommandList->Begin();
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to begin single time command list!");
			pCommandList->Destroy();
			delete pCommandList;
			return nullptr;
		}
		m_STCommandLists.push_back(pCommandList);

		return pCommandList;
	}

	b8 VulkanCommandListManager::EndSingleTimeCommandList(RHI::CommandList* pCommandList)
	{
		b8 res = true;
		if (pCommandList->GetState() == RHI::CommandListState::Recording)
		{
			res = pCommandList->End();
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to end single time command list!");
			}
		}
		if (res && pCommandList->GetState() == RHI::CommandListState::Recorded)
		{
			res = pCommandList->Submit();
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to submit single time command list!");
			}
		}
		if (res && pCommandList->GetState() == RHI::CommandListState::Submited)
		{
			res = pCommandList->Wait();
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to wait for single time command list!");
			}
		}

		auto it = std::find(m_STCommandLists.begin(), m_STCommandLists.end(), pCommandList);;
		if (it == m_STCommandLists.end())
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to remove a single time command list, manager does not contain a command list!");
			return false;
		}

		((VulkanCommandList*)pCommandList)->Destroy();
		delete pCommandList;

		m_STCommandLists.erase(it);

		return res;
	}

	VkCommandPool VulkanCommandListManager::GetCommandPool(RHI::Queue* pQueue)
	{
		u32 index = ((VulkanQueue*)pQueue)->GetQueueFamily();
		return m_CommandPools[index];
	}
}
