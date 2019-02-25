
#define MAX_DESC_SETS 1024 // Enough for now
#include "VulkanDescriptorSetManager.h"
#include "VulkanContext.h"
#include "../RHI/DescriptorSet.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"

namespace Vulkan {


	VulkanDescriptorSetManager::VulkanDescriptorSetManager()
		: m_DescriptorPool(VK_NULL_HANDLE)
	{
	}

	VulkanDescriptorSetManager::~VulkanDescriptorSetManager()
	{
	}

	b8 VulkanDescriptorSetManager::Create(RHI::RHIContext* pContext)
	{
		m_pContext = pContext;

		// for now, create 1 large descriptor pool
		// TODO: Improve this
		std::vector<VkDescriptorPoolSize> poolSizes;
		poolSizes.reserve(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1);

		VkDescriptorPoolSize poolSize;
		poolSize.descriptorCount = MAX_DESC_SETS;
		poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		poolSizes.push_back(poolSize);
		poolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		poolSizes.push_back(poolSize);

		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		createInfo.maxSets = MAX_DESC_SETS;
		createInfo.poolSizeCount = u32(poolSizes.size());
		createInfo.pPoolSizes = poolSizes.data();

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres = pDevice->vkCreateDescriptorPool(createInfo, m_DescriptorPool);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the vulkan descriptor pool (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		return true;
	}

	b8 VulkanDescriptorSetManager::Destroy()
	{
		for (RHI::DescriptorSet* set : m_DescriptorSets)
		{
			set->Destroy();
			delete set;
		}
		m_DescriptorSets.clear();

		if (m_DescriptorPool)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			pDevice->vkDestroyDescriptorPool(m_DescriptorPool);
		}

		return true;
	}

	RHI::DescriptorSetLayout* VulkanDescriptorSetManager::CreateDescriptorSetLayout(
		const std::vector<RHI::DescriptorSetBinding>& bindings)
	{
		// Try to reuse layouts, if it already exists
		for (RHI::DescriptorSetLayout* pLayout : m_Layouts)
		{
			if (pLayout->Matches(bindings))
				return pLayout;
		}

		// Else create a new layout
		VulkanDescriptorSetLayout* pLayout = new VulkanDescriptorSetLayout();

		b8 res = pLayout->Create(m_pContext, this, bindings);
		if (!res)
		{
			//g_Logger.LogError("Failed to create descriptor set!");
			delete pLayout;
			return nullptr;
		}

		m_Layouts.push_back(pLayout);
		return pLayout;
	}

	RHI::DescriptorSet* VulkanDescriptorSetManager::CreateDescriptorSet(RHI::DescriptorSetLayout* pLayout)
	{
		if (m_DescriptorSets.size() == MAX_DESC_SETS)
		{
			//g_Logger.LogError("Failed to create descriptor set, reach the max amount of allowed sets!");
			return nullptr;
		}

		VulkanDescriptorSet* pDescriptorSet = new VulkanDescriptorSet();

		b8 res = pDescriptorSet->Create(m_pContext, this, pLayout);
		if (!res)
		{
			//g_Logger.LogError("Failed to create descriptor set!");
			delete pDescriptorSet;
			return nullptr;
		}

		m_DescriptorSets.push_back(pDescriptorSet);
		return pDescriptorSet;
	}

	b8 VulkanDescriptorSetManager::DestroyDescriptorSetLayout(RHI::DescriptorSetLayout* pLayout)
	{
		if (pLayout->GetReferenceCount() > 0)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Can't destroy a descriptor set layout that is still being used!");
			return false;
		}

		auto it = std::find(m_Layouts.begin(), m_Layouts.end(), pLayout);
		if (it == m_Layouts.end())
		{
			//g_Logger.LogWarning("Failed to remove a descriptor set from manager!");
			return false;
		}

		b8 res = pLayout->Destroy();
		if (!res)
		{
			//g_Logger.LogError("Failed to destroy a descriptor set layout!");
		}

		delete pLayout;

		m_Layouts.erase(it);
		return true;
	}

	b8 VulkanDescriptorSetManager::DestroyDescriptorSet(RHI::DescriptorSet* pDescriptorSet)
	{
		auto it = std::find(m_DescriptorSets.begin(), m_DescriptorSets.end(), pDescriptorSet);
		if (it == m_DescriptorSets.end())
		{
			//g_Logger.LogWarning("Failed to remove a descriptor set from manager");
			return false;
		}

		b8 res = pDescriptorSet->Destroy();
		if (!res)
		{
			//g_Logger.LogError("Failed to destroy a descriptor set!");
		}

		delete pDescriptorSet;

		m_DescriptorSets.erase(it);
		return true;
	}
}

#undef MAX_DESC_SETS