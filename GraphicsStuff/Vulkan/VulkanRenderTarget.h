#pragma once
#include <vulkan/vulkan.h>
#include "../General/TypesAndMacros.h"
#include "../RHI/RenderTarget.h"

namespace RHI {
	class RHIContext;
}

namespace Vulkan {

	// TODO: complete class
	class VulkanRenderTarget final : public RHI::RenderTarget
	{
	public:

		VulkanRenderTarget();
		~VulkanRenderTarget();

		/**
		 * Create a render target
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Render target description
		 * @return				True if the texture was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, const RHI::RenderTargetDesc& desc) override final;
		/**
		 * Create a render target from a vulkan image
		 * @param[in] pContext	RHI context
		 * @param[in] image		Vulkan image
		 * @param[in] layout	Vulkan image layout
		 * @param[in] width		Width
		 * @param[in] height	Height
		 * @param[in] format	Format
		 * @param[in] samples	Sample count
		 * @param[in] type		Render target type
		 * @return				True if the texture was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, VkImage image, VkImageLayout layout, u32 width, u32 height, PixelFormat format, RHI::SampleCount samples, RHI::RenderTargetType type);

		/**
		 * Destroy the texture
		 */
		b8 Destroy() override final;

	private:

	};

}
