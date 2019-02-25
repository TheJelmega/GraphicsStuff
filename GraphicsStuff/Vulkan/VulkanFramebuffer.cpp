
#include <vulkan/vulkan.h>
#undef min
#include <algorithm>
#include "VulkanFramebuffer.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "../RHI/RenderTarget.h"
#include "VulkanContext.h"


namespace Vulkan {

	VulkanFramebuffer::VulkanFramebuffer()
		: m_FrameBuffer(VK_NULL_HANDLE)
	{
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	b8 VulkanFramebuffer::Create(RHI::RHIContext* pContext, const std::vector<RHI::RenderTarget*>& renderTargets, RHI::RenderPass* pRenderPass)
	{
		m_pContext = pContext;
		m_RenderTargets = renderTargets;
		m_pRenderPass = pRenderPass;

		std::vector<VkImageView> views;
		m_Width = 0xFFFF'FFFF;
		m_Height = 0xFFFF'FFFF;
		// TODO: support render targets with multiple layers
		u32 layers = 1;
		for (RHI::RenderTarget* pRT : m_RenderTargets)
		{
			views.push_back(((VulkanTexture*)pRT->GetTexture())->GetImageView());
			m_Width = std::min(m_Width, pRT->GetWidth());
			m_Height = std::min(m_Height, pRT->GetHeight());
		}

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = u8(views.size());
		createInfo.pAttachments = views.data();
		createInfo.renderPass = ((VulkanRenderPass*)m_pRenderPass)->GetRenderPass();
		createInfo.width = m_Width;
		createInfo.height = m_Height;
		createInfo.layers = layers;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres = pDevice->vkCreateFramebuffer(createInfo, m_FrameBuffer);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create a vulkan frame buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		return true;
	}

	b8 VulkanFramebuffer::Destroy()
	{
		if (m_FrameBuffer)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			pDevice->vkDestroyFramebuffer(m_FrameBuffer);
			m_FrameBuffer = VK_NULL_HANDLE;
		}
		return true;
	}
}
