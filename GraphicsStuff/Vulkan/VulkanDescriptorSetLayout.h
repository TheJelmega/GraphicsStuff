#pragma once
#include "VulkanDescriptorSet.h"
#include "../RHI/DescriptorSetLayout.h"

namespace Vulkan {
	
	class VulkanDescriptorSetLayout : public RHI::DescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout();
		~VulkanDescriptorSetLayout();

		/**
		 * Create the descriptor set
		 * @param[in] pContext	RHI context
		 * @param[in] pManager	Descriptor set manager
		 * @param[in] bindings	Descriptor set bindings
		 * @return				True if the descriptor set was created successfully, false otherwise
		 */
		virtual b8 Create(RHI::RHIContext* pContext, RHI::DescriptorSetManager* pManager, const std::vector<RHI::DescriptorSetBinding>& bindings);
		/**
		 * Destroy the descriptor set
		 * @return	True if the descriptor set was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy();

		/**
		 * Get the vulkan descriptor set layout
		 * @return	Vulkan descriptor set layout
		 */
		VkDescriptorSetLayout GetLayout() { return m_Layout; }

	private:
		VkDescriptorSetLayout m_Layout;
	};

}
