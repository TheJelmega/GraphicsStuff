#include "VulkanDescriptorSetLayout.h"
#include "VulkanHelpers.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"

namespace Vulkan {

	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
		: m_Layout(VK_NULL_HANDLE)
	{
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
	}

	b8 VulkanDescriptorSetLayout::Create(RHI::RHIContext* pContext, RHI::DescriptorSetManager* pManager,
		const std::vector<RHI::DescriptorSetBinding>& bindings)
	{
		m_pContext = pContext;
		m_pManager = pManager;
		m_Bindings = bindings;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		// flags???

		std::vector<VkDescriptorSetLayoutBinding> vulkanBindings;
		vulkanBindings.reserve(bindings.size());

		for (u32 i = 0; i < bindings.size(); ++i)
		{
			const RHI::DescriptorSetBinding& binding = bindings[i];
			VkDescriptorSetLayoutBinding vulkanBinding = {};
			vulkanBinding.binding = i;
			vulkanBinding.descriptorCount = binding.count;
			vulkanBinding.descriptorType = Helpers::GetDescriptorType(binding.type);
			vulkanBinding.stageFlags = Helpers::GetShaderStage(binding.shadertype);
			// TODO: pImmutableSamplers?

			vulkanBindings.push_back(vulkanBinding);
		}
		layoutInfo.bindingCount = u32(vulkanBindings.size());
		layoutInfo.pBindings = vulkanBindings.data();

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres = pDevice->vkCreateDescriptorSetLayout(layoutInfo, m_Layout);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the vulkan descriptor set layout (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		return true;
	}

	b8 VulkanDescriptorSetLayout::Destroy()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		if (m_Layout)
		{
			pDevice->vkDestroyDescriptorSetLayout(m_Layout);
			m_Layout = VK_NULL_HANDLE;
		}

		return true;
	}
}
