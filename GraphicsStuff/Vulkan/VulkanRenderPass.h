#pragma once
#include <vulkan/vulkan.h>
#include "../General/TypesAndMacros.h"
#include "../RHI/RenderPass.h"

namespace Vulkan {
	
	class VulkanRenderPass final : public RHI::RenderPass
	{
	private:

		struct SubpassDescription
		{
			std::vector<VkAttachmentReference> colorAttachments;
			std::vector<VkAttachmentReference> resolveAttachments;
			VkAttachmentReference depthAttachment;
			std::vector<VkAttachmentReference> inputAttachments;
			std::vector<u32> preserveAttachments;
			VkSubpassDescription subpassDesc;
		};

	public:
		VulkanRenderPass();
		~VulkanRenderPass();

		/**
		 * Create a render pass
		 * @param[in] pContext		RHI context
		 * @param[in] attachments	Attachments
		 * @param[in] subpasses		Sub passes
		 * @return					True if the renderpass was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, const std::vector<RHI::RenderPassAttachment>& attachments, const std::vector<RHI::SubRenderPass>& subpasses) override final;

		/**
		 * Destroy a renderpass
		 * @return	True if the renderpass was desstroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		/**
		 * Get the vulkan render pass
		 * @return	Vulkan render pass
		 */
		VkRenderPass GetRenderPass() { return m_RenderPass; }

	private:
		VkRenderPass m_RenderPass;	/**< Vulkan render pass */
	};

}
