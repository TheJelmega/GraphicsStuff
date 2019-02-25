#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "../RHI/DescriptorSetManager.h"

namespace Vulkan {
	
	class VulkanDescriptorSetManager final : public RHI::DescriptorSetManager
	{
	public:
		VulkanDescriptorSetManager();
		~VulkanDescriptorSetManager();

		/**
		 * Create the descriptor set manager
		 * @param[in] pContext	RHI context
		 * @return				True if the descriptor set manager was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext) override final;
		/**
		 * Create the descriptor set manager
		 * @return	True if the descriptor set manager was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		/**
		* Create a descriptor set
		* @param[in] bindings	Descriptor set bindings
		* @return				Pointer to a descriptor set, nullptr if the creation failed
		*/
		RHI::DescriptorSetLayout* CreateDescriptorSetLayout(const std::vector<RHI::DescriptorSetBinding>& bindings) override final;
		/**
		* Create a descriptor set
		* @param[in] pLayout	Descriptor set layout
		* @return				Pointer to a descriptor set, nullptr if the creation failed
		*/
		RHI::DescriptorSet* CreateDescriptorSet(RHI::DescriptorSetLayout* pLayout) override final;
		/**
		* Destroy a descriptor set
		* @param[in] pLayout	Descriptor set layout to destroy
		* @return				True if the descriptor set layout was destroyed successfully, false otherwise
		*/
		b8 DestroyDescriptorSetLayout(RHI::DescriptorSetLayout* pLayout) override final;
		/**
		* Destroy a descriptor set
		* @param[in] pDescriptorSet	Descriptor set to destroy
		* @return						True if the descriptor set was destroyed successfully, false otherwise
		*/
		b8 DestroyDescriptorSet(RHI::DescriptorSet* pDescriptorSet) override final;

		/**
		 * Get the vulkan descriptor pool
		 * @return	Vulkan descriptor pool
		 */
		VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }

	private:
		VkDescriptorPool m_DescriptorPool;
	};

}
