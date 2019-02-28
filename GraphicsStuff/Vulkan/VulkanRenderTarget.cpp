
#include "VulkanRenderTarget.h"
#include "VulkanTexture.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanContext.h"
#include "../RHI/CommandListManager.h"
#include "../RHI/RHIHelpers.h"
#include "VulkanDevice.h"

namespace Vulkan {

	VulkanRenderTarget::VulkanRenderTarget()
		: RenderTarget()
	{
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
	}

	b8 VulkanRenderTarget::Create(RHI::RHIContext* pContext, const RHI::RenderTargetDesc& desc)
	{
		m_pContext = pContext;
		m_Type = desc.type;

		m_pTexture = new VulkanTexture();

		RHI::TextureFlags textureFlags = RHI::TextureFlags::RenderTargetable;
		if (desc.type == RHI::RenderTargetType::Color || desc.type == RHI::RenderTargetType::Presentable)
			textureFlags |= RHI::TextureFlags::Color;
		if (desc.type == RHI::RenderTargetType::DepthStencil)
			textureFlags |= RHI::TextureFlags::DepthStencil;

		u32 width = desc.width;
		u32 height = desc.height;

		VulkanPhysicalDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice()->GetPhysicalDevice();
		if (width > pDevice->GetLimits().maxFramebufferWidth)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Warning, "Width to large (currenty: %u, max: %u). Width will be resized!", width, pDevice->GetLimits().maxFramebufferWidth);
			width = pDevice->GetLimits().maxFramebufferWidth;
		}
		if (height > pDevice->GetLimits().maxFramebufferHeight)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Warning, "Height to large (currenty: %u, max: %u). Height will be resized!", height, pDevice->GetLimits().maxFramebufferHeight);
			height = pDevice->GetLimits().maxFramebufferHeight;
		}

		RHI::TextureDesc texDesc = {};
		texDesc.width = width;
		texDesc.height = height;
		texDesc.format = desc.format;
		texDesc.type = RHI::TextureType::Tex2D;
		texDesc.samples = desc.samples;
		texDesc.flags = textureFlags;
		texDesc.layout = RHI::Helpers::GetTextureLayoutFromRTType(desc.type);

		RHI::Queue* pQueue = m_pContext->GetQueue(RHI::QueueType::Graphics);
		RHI::CommandListManager* pCommandListManager = m_pContext->GetCommandListManager();
		RHI::CommandList* pCommandList = pCommandListManager->CreateSingleTimeCommandList(pQueue);

		b8 res = m_pTexture->Create(m_pContext, texDesc, pCommandList);
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to create underlying texture for render target!");
			return false;
		}

		pCommandListManager->EndSingleTimeCommandList(pCommandList);

		return true;
	}

	b8 VulkanRenderTarget::Create(RHI::RHIContext* pContext, VkImage image, VkImageLayout layout, u32 width, u32 height, PixelFormat format, RHI::SampleCount samples,
		RHI::RenderTargetType type)
	{
		m_pContext = pContext;
		m_Type = type;

		m_pTexture = new VulkanTexture();
		VulkanTexture* pTexture = (VulkanTexture*)m_pTexture;

		RHI::TextureFlags textureFlags = RHI::TextureFlags::RenderTargetable;

		b8 res = pTexture->Create(m_pContext, image, layout, width, height, format, samples, textureFlags);
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to create underlying texture for render target!");
			return false;
		}

		return true;
	}

	b8 VulkanRenderTarget::Destroy()
	{
		b8 res = m_pTexture->Destroy();
		delete m_pTexture;

		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to destroy underlying texture for render target!");
			return false;
		}

		return true;
	}
}
