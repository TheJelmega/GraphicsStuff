#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/DescriptorSet.h"

namespace Vulkan {
	
	class VulkanDescriptorSet final : public RHI::DescriptorSet
	{
	public:
		VulkanDescriptorSet();
		~VulkanDescriptorSet();

		/**
		 * Create the descriptor set
		 * @param[in] pContext	RHI context
		 * @param[in] pManager	Descriptor set manager
		 * @param[in] pLayout	Descriptor set layout
		 * @return				True if the descriptor set was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, RHI::DescriptorSetManager* pManager, RHI::DescriptorSetLayout* pLayout) override final;
		/**
		 * Destroy the descriptor set
		 * @return	True if the descriptor set was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		/**
		* Write a buffer to the descriptor set
		* @param[in] binding		Binding to write to
		* @param[in] pBuffer		Buffer to write
		* @param[in] arrayElement	Element of the array to write buffer to
		* @note						This function has limited functionality, could become depricated or removed in the future
		*/
		b8 Write(u32 binding, RHI::Buffer* pBuffer, u64 offset = 0, u64 range = u64(-1), u32 arrayElement = 0) override final;
		/**
		* Write a texture and a sampler to the descriptor set
		* @param[in] binding	Binding to write to
		* @param[in] pTexture	Texture to write
		* @param[in] pSampler	sampler to write
		* @param[in] arrayElement	Element of the array to write buffer to
		*/
		b8 Write(u32 binding, RHI::Texture* pTexture, RHI::Sampler* pSampler, u32 arrayElement = 0) override final;
		/**
		 * Write a texture and a sampler to the descriptor set
		 * @param[in] buffers			Buffer write info
		 * @param[in] texAndSamplers		Texture and sampler write info
		 */
		b8 Write(const std::vector<RHI::DescriptorSetBufferInfo>& buffers, const std::vector<RHI::DescriptorSetTextureSamplerInfo>& texAndSamplers) override final;

		/**
		 * Get the vulkan descriptor set
		 * @return Vulkan descriptor set
		 */
		VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }

	private:
		VkDescriptorSet m_DescriptorSet;	/**< Vulkan descriptor set */
		VkDescriptorPool m_Pool;			/**< Vulkan descriptor pool */
	};

}
