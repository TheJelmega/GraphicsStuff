#pragma once
#include <vulkan/vulkan.h>
#include "../General/TypesAndMacros.h"
#include "../RHI/Pipeline.h"

namespace RHI {
	class RHIContext;
}

namespace Vulkan {
	
	class VulkanPipeline final : public RHI::Pipeline
	{
	public:
		VulkanPipeline();
		~VulkanPipeline();

		/**
		 * Create a graphics pipeline
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Graphics pipeline descriptor
		 * @return				True if the graphics pipeline was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, const RHI::GraphicsPipelineDesc& desc) override final;
		/**
		 * Create a compute pipeline
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Compute pipeline descriptor
		 * @return				True if the compute pipeline was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, const RHI::ComputePipelineDesc& desc) override final;

		/**
		 * Destroy the pipeline
		 * @return	True if the pipeline was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		/**
		 * Get the vulkan pipeline layout
		 * @return	Vulkan pipeline layout
		 */
		VkPipelineLayout GetLayout() { return m_Layout; }
		/**
		 * Get the vulkan pipeline
		 * @return	Vulkan pipeline
		 */
		VkPipeline GetPipeline() { return m_Pipeline; }

	private:
		VkPipelineLayout m_Layout;	/**< Pipeline layout */
		VkPipeline m_Pipeline;		/**< Pipeline */
	};
	
}
