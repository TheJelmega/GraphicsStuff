#pragma once
#include "../RHI/Framebuffer.h"

namespace Vulkan {
	
	class VulkanFramebuffer final : public RHI::Framebuffer
	{
	public:
		VulkanFramebuffer();
		~VulkanFramebuffer();

		/**
		* Create the frame buffer
		* @param[in] pContext			RHI context
		* @param[in] renderTargets		Render targets
		* @param[in] pRenderPass		Associated render pass
		* @return						True if the framebuffer was created successfully, false otherwise
		*/
		b8 Create(RHI::RHIContext* pContext, const std::vector<RHI::RenderTarget*>& renderTargets, RHI::RenderPass* pRenderPass) override final;

		/**
		* Destroy the frame buffer
		* @return	True if the framebuffer was destroyed successfully, false otherwise
		*/
		b8 Destroy() override final;

		/**<
		 * Get the vulkan framebuffer
		 * @return	Vulkan framebuffer
		 */
		VkFramebuffer GetFrameBuffer() { return m_FrameBuffer; }

	private:
		VkFramebuffer m_FrameBuffer;
	};

}
