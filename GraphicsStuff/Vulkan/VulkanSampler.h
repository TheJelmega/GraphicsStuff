#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/Sampler.h"

namespace Vulkan {
	
	class VulkanSampler : public RHI::Sampler
	{
	public:
		VulkanSampler();
		~VulkanSampler();

		/**
		 * Create the sampler
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Sampler description
		 * @return				True if the sampler was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, const RHI::SamplerDesc& desc) override final;
		/**
		 * Create the sampler
		 * @return	True if the sampler was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		/**
		* Get the vulkan sampler
		* @return	Vulkan sampler
		*/
		VkSampler GetSampler() { return m_Sampler; }

	private:
		VkSampler m_Sampler;
	};

}

