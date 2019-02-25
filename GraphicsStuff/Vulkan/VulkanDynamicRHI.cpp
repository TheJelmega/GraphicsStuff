
#include "VulkanDynamicRHI.h"
#include "VulkanContext.h"
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanSampler.h"
#include "VulkanTexture.h"
#include "VulkanRenderTarget.h"

namespace Vulkan {

	VulkanDynamicRHI::VulkanDynamicRHI()
		: IDynamicRHI()
	{
	}

	VulkanDynamicRHI::~VulkanDynamicRHI()
	{
	}

	b8 VulkanDynamicRHI::Init(const RHI::RHIDesc& desc, sf::Window* pMainWindow)
	{
		m_Desc = desc;

		m_pContext = new VulkanContext();
		b8 res = m_pContext->Init(desc, pMainWindow);
		if (!res)
		{
			//g_Logger.LogFatal("Failed to initialize vulkan context");
			Destroy();
			return false;
		}

		return true;
	}

	b8 VulkanDynamicRHI::Destroy()
	{
		if (m_pContext)
		{
			b8 res = m_pContext->Destroy();
			delete m_pContext; // Delete if destroy failed
			if (!res)
			{
				//g_Logger.LogFatal(LogVulkanRHI(), "Failed to desteoy context");
				return false;
			}
		}
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Renderview																  //
	////////////////////////////////////////////////////////////////////////////////
	RHI::SwapChain* VulkanDynamicRHI::CreateSwapChain(sf::Window* pWindow, RHI::VSyncMode vsync)
	{
		VulkanSwapChain* pView = new VulkanSwapChain();

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		RHI::Queue* pQueue = pDevice->GetPresentQueue();
		b8 res = pView->Init(m_pContext, pWindow, vsync, pQueue);
		if (!res)
		{
			pView->Destroy();
			delete pView;
			return nullptr;
		}
		return pView;
	}

	b8 VulkanDynamicRHI::DestroySwapChain(RHI::SwapChain* pRenderView)
	{
		b8 res = pRenderView->Destroy();
		delete pRenderView;
		return res;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Shaders																	  //
	////////////////////////////////////////////////////////////////////////////////
	RHI::Shader* VulkanDynamicRHI::CreateShader(const RHI::ShaderDesc& desc)
	{
		VulkanShader* pShader = new VulkanShader();
		b8 res = pShader->Create(m_pContext, desc);
		if (!res)
		{
			pShader->Destroy();
			delete pShader;
			return nullptr;
		}
		return pShader;
	}

	b8 VulkanDynamicRHI::DestroyShader(RHI::Shader* pShader)
	{
		b8 res = pShader->Destroy();
		delete pShader;
		return res;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Render pass																  //
	////////////////////////////////////////////////////////////////////////////////
	RHI::RenderPass* VulkanDynamicRHI::CreateRenderPass(
		const std::vector<RHI::RenderPassAttachment>& attachments,
		const std::vector<RHI::SubRenderPass>& subpasses)
	{
		VulkanRenderPass* pRenderPass = new VulkanRenderPass();
		b8 res = pRenderPass->Create(m_pContext, attachments, subpasses);
		if (!res)
		{
			pRenderPass->Destroy();
			delete pRenderPass;
			return nullptr;
		}
		return pRenderPass;
	}

	b8 VulkanDynamicRHI::DestroyRenderPass(RHI::RenderPass* pRenderPass)
	{
		b8 res = pRenderPass->Destroy();
		delete pRenderPass;
		return res;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Pipelines																  //
	////////////////////////////////////////////////////////////////////////////////
	RHI::Pipeline* VulkanDynamicRHI::CreatePipeline(const RHI::GraphicsPipelineDesc& desc)
	{
		VulkanPipeline* pPipeline = new VulkanPipeline();
		b8 res = pPipeline->Create(m_pContext, desc);
		if (!res)
		{
			pPipeline->Destroy();
			delete pPipeline;
			return nullptr;
		}
		return pPipeline;
	}

	RHI::Pipeline* VulkanDynamicRHI::CreatePipeline(const RHI::ComputePipelineDesc& desc)
	{
		VulkanPipeline* pPipeline = new VulkanPipeline();
		b8 res = pPipeline->Create(m_pContext, desc);
		if (!res)
		{
			pPipeline->Destroy();
			delete pPipeline;
			return nullptr;
		}
		return pPipeline;
	}

	b8 VulkanDynamicRHI::DestroyPipeline(RHI::Pipeline* pPipeline)
	{
		b8 res = pPipeline->Destroy();
		delete pPipeline;
		return res;
	}

	RHI::Framebuffer* VulkanDynamicRHI::CreateFramebuffer(
		const std::vector<RHI::RenderTarget*>& renderTargets, RHI::RenderPass* pRenderPass)
	{
		VulkanFramebuffer* pFramebuffer = new VulkanFramebuffer();
		b8 res = pFramebuffer->Create(m_pContext, renderTargets, pRenderPass);
		if (!res)
		{
			pFramebuffer->Destroy();
			delete pFramebuffer;
			return nullptr;
		}
		return pFramebuffer;
	}

	b8 VulkanDynamicRHI::DestroyFramebuffer(RHI::Framebuffer* pFramebuffer)
	{
		b8 res = pFramebuffer->Destroy();
		delete pFramebuffer;
		return res;
	}

	RHI::Sampler* VulkanDynamicRHI::CreateSampler(const RHI::SamplerDesc& desc)
	{
		VulkanSampler* pSampler = new VulkanSampler();
		b8 res = pSampler->Create(m_pContext, desc);
		if (!res)
		{
			pSampler->Destroy();
			delete pSampler;
			return nullptr;
		}
		return pSampler;
	}

	b8 VulkanDynamicRHI::DestroySampler(RHI::Sampler* pSampler)
	{
		b8 res = pSampler->Destroy();
		delete pSampler;
		return res;
	}

	RHI::Texture* VulkanDynamicRHI::CreateTexture(const RHI::TextureDesc& desc, RHI::CommandList* pCommandList)
	{
		VulkanTexture* pTexture = new VulkanTexture();
		b8 res = pTexture->Create(m_pContext, desc, pCommandList);
		if (!res)
		{
			pTexture->Destroy();
			delete pTexture;
			return nullptr;
		}
		return pTexture;
	}

	b8 VulkanDynamicRHI::DestroyTexture(RHI::Texture* pTexture)
	{
		b8 res = pTexture->Destroy();
		delete pTexture;
		return res;
	}

	RHI::RenderTarget* VulkanDynamicRHI::CreateRenderTarget(const RHI::RenderTargetDesc& desc)
	{
		VulkanRenderTarget* pRT = new VulkanRenderTarget();
		b8 res = pRT->Create(m_pContext, desc);
		if (!res)
		{
			pRT->Destroy();
			delete pRT;
			return nullptr;
		}
		return pRT;
	}

	b8 VulkanDynamicRHI::DestroyRenderTarget(RHI::RenderTarget* pRenderTarget)
	{
		b8 res = pRenderTarget->Destroy();
		delete pRenderTarget;
		return res;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Buffers																	  //
	////////////////////////////////////////////////////////////////////////////////
	RHI::Buffer* VulkanDynamicRHI::CreateBuffer(RHI::BufferType type, u64 size, RHI::BufferFlags flags)
	{
		VulkanBuffer* pBuffer = new VulkanBuffer();
		b8 res = pBuffer->Create(m_pContext, type, size, flags);
		if (!res)
		{
			delete pBuffer;
			//g_Logger.LogError("Failed to create buffer");
			return nullptr;
		}
		return pBuffer;
	}

	RHI::Buffer* VulkanDynamicRHI::CreateVertexBuffer(u32 vertexCount, u16 vertexSize, RHI::BufferFlags flags)
	{
		VulkanBuffer* pBuffer = new VulkanBuffer();
		b8 res = pBuffer->CreateVertexBuffer(m_pContext, vertexCount, vertexSize, flags);
		if (!res)
		{
			delete pBuffer;
			//g_Logger.LogError("Failed to create buffer");
			return nullptr;
		}
		return pBuffer;
	}

	RHI::Buffer* VulkanDynamicRHI::CreateIndexBuffer(u32 indexCount, RHI::IndexType indexType, RHI::BufferFlags flags)
	{
		VulkanBuffer* pBuffer = new VulkanBuffer();
		b8 res = pBuffer->CreateIndexBuffer(m_pContext, indexCount, indexType, flags);
		if (!res)
		{
			delete pBuffer;
			//g_Logger.LogError("Failed to create buffer");
			return nullptr;
		}
		return pBuffer;
	}

	b8 VulkanDynamicRHI::DestroyBuffer(RHI::Buffer* pBuffer)
	{
		if (!pBuffer)
		{
			//g_Logger.LogError("Buffer can't be a nullptr!");
			return false;
		}
		b8 res = pBuffer->Destroy();
		if (!res)
		{
			//g_Logger.LogError("Failed to destroy buffer!");
			return false;
		}
		delete pBuffer;
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Other																	  //
	////////////////////////////////////////////////////////////////////////////////
	b8 VulkanDynamicRHI::WaitIdle()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		return pDevice->WaitIdle();
	}
}
