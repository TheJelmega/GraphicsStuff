
#include "VulkanSampler.h"
#include "VulkanHelpers.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

namespace Vulkan {


	VulkanSampler::VulkanSampler()
		: Sampler()
		, m_Sampler(VK_NULL_HANDLE)
	{
	}

	VulkanSampler::~VulkanSampler()
	{
	}

	b8 VulkanSampler::Create(RHI::RHIContext* pContext, const RHI::SamplerDesc& desc)
	{
		m_pContext = pContext;
		m_Desc = desc;

		VkSamplerCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter = Helpers::GetFilter(m_Desc.magFilter);
		createInfo.minFilter = Helpers::GetFilter(m_Desc.minFilter);
		createInfo.mipmapMode = Helpers::GetMipmapMode(m_Desc.magFilter, m_Desc.minFilter);
		createInfo.addressModeU = Helpers::GetAddressMode(m_Desc.addressU);
		createInfo.addressModeV = Helpers::GetAddressMode(m_Desc.addressV);
		createInfo.addressModeW = Helpers::GetAddressMode(m_Desc.addressW);
		createInfo.anisotropyEnable = m_Desc.enableAnisotropy;
		createInfo.maxAnisotropy = m_Desc.anisotropy;
		createInfo.mipLodBias = m_Desc.mipLodBias;
		createInfo.minLod = m_Desc.minLod;
		createInfo.maxLod = m_Desc.maxLod;
		createInfo.compareEnable = m_Desc.enableCompare;
		createInfo.compareOp = Helpers::GetCompareOp(m_Desc.compareOp);
		createInfo.unnormalizedCoordinates = m_Desc.unnormalizedCoordinates;
		// TODO: Int or float types?
		createInfo.borderColor = Helpers::GetBorderColor(m_Desc.borderColor, true);
			
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres = pDevice->vkCreateSampler(createInfo, m_Sampler);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create vulkan sampler (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		return true;
	}

	b8 VulkanSampler::Destroy()
	{
		if (m_Sampler)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			pDevice->vkDestroySampler(m_Sampler);
			m_Sampler = VK_NULL_HANDLE;
		}

		return true;
	}
}
