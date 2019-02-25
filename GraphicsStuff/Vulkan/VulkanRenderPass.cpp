
#include "VulkanRenderPass.h"
#include "VulkanHelpers.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

namespace Vulkan {


	VulkanRenderPass::VulkanRenderPass()
		: m_RenderPass(VK_NULL_HANDLE)
	{
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
	}

	b8 VulkanRenderPass::Create(RHI::RHIContext* pContext,
		const std::vector<RHI::RenderPassAttachment>& attachments,
		const std::vector<RHI::SubRenderPass>& subpasses)
	{
		m_pContext = pContext;
		m_Attachments = attachments;
		m_SubPasses = subpasses;

		// Create vulkan render pass attachments
		std::vector<VkAttachmentDescription> vulkanAttachments;
		for (const RHI::RenderPassAttachment& rpa : m_Attachments)
		{
			VkAttachmentDescription attachment = {};
			attachment.loadOp = Helpers::GetLoadOp(rpa.loadOp);
			attachment.storeOp = Helpers::GetStoreOp(rpa.storeOp);
			attachment.stencilLoadOp = Helpers::GetLoadOp(rpa.stencilLoadOp);
			attachment.stencilStoreOp = Helpers::GetStoreOp(rpa.stencilStoreOp);
			attachment.format = Helpers::GetFormat(rpa.format);
			attachment.samples = Helpers::GetSampleCount(rpa.samples);
			if (rpa.type == RHI::RenderTargetType::Presentable)
			{
				attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachment.finalLayout = Helpers::GetSubpassAttachmentLayout(rpa.type);
			}
			else
			{
				attachment.initialLayout = attachment.finalLayout = Helpers::GetSubpassAttachmentLayout(rpa.type);
			}

			vulkanAttachments.push_back(attachment);
		}

		std::vector<SubpassDescription> subpassDescs;
		subpassDescs.resize(m_SubPasses.size());
		std::vector<VkSubpassDescription> vulkanSubpasses;
		vulkanSubpasses.reserve(m_SubPasses.size());
		for (sizeT i = 0; i < m_SubPasses.size(); ++i)
		{
			const RHI::SubRenderPass& subpass = m_SubPasses[i];
			SubpassDescription& desc = subpassDescs[i];
			desc = {};
			desc.depthAttachment.attachment = VK_ATTACHMENT_UNUSED;

			for (const RHI::RenderPassAttachmentRef& attachmentRef : subpass.attachments)
			{
				if (attachmentRef.type == RHI::RenderTargetType::Preserve)
				{
					desc.preserveAttachments.push_back(attachmentRef.index);
					continue;
				}

				u32 index = attachmentRef.index;
				const RHI::RenderPassAttachment& attachment = m_Attachments[index];
				const VkAttachmentDescription& attachDesc = vulkanAttachments[index];

				VkAttachmentReference ref = {};
				ref.attachment = index;
				ref.layout = Helpers::GetSubpassAttachmentLayout(attachmentRef.type);
				if (ref.layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
					ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				switch (attachmentRef.type)
				{
				default:
				case RHI::RenderTargetType::None: 
					break;
				case RHI::RenderTargetType::Color:
				case RHI::RenderTargetType::Presentable:
				{
					desc.colorAttachments.push_back(ref);
					ref.attachment = VK_ATTACHMENT_UNUSED;
					desc.resolveAttachments.push_back(ref);
					break;
				}
				case RHI::RenderTargetType::DepthStencil:
				{
					desc.depthAttachment = ref;
					break;
				}
				case RHI::RenderTargetType::MultisampleResolve:
				{
					desc.resolveAttachments.push_back(ref);
					ref.attachment = VK_ATTACHMENT_UNUSED;
					desc.colorAttachments.push_back(ref);
					break;
				}
				case RHI::RenderTargetType::Input:
				{
					desc.inputAttachments.push_back(ref);
					break;
				}
				}
			}

			desc.subpassDesc.colorAttachmentCount = u32(desc.colorAttachments.size());
			desc.subpassDesc.pColorAttachments = desc.colorAttachments.data();
			desc.subpassDesc.pResolveAttachments = desc.resolveAttachments.data();
			desc.subpassDesc.pDepthStencilAttachment = &desc.depthAttachment;
			desc.subpassDesc.inputAttachmentCount = u32(desc.inputAttachments.size());
			desc.subpassDesc.pInputAttachments = desc.inputAttachments.data();
			desc.subpassDesc.preserveAttachmentCount = u32(desc.preserveAttachments.size());
			desc.subpassDesc.pPreserveAttachments = desc.preserveAttachments.data();

			desc.subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			vulkanSubpasses.push_back(desc.subpassDesc);
		}
		
		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = u32(vulkanAttachments.size());
		createInfo.pAttachments = vulkanAttachments.data();
		createInfo.subpassCount = u32(vulkanSubpasses.size());
		createInfo.pSubpasses = vulkanSubpasses.data();

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres = pDevice->vkCreateRenderPass(createInfo, m_RenderPass);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create a vulkan render pass!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		return true;
	}

	b8 VulkanRenderPass::Destroy()
	{
		if (m_RenderPass)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			pDevice->vkDestroyRenderPass(m_RenderPass);
			m_RenderPass = VK_NULL_HANDLE;
		}

		return true;
	}
}
