#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/CommandListManager.h"

namespace Vulkan {
	
	class VulkanCommandListManager final : public RHI::CommandListManager
	{
	public:
		VulkanCommandListManager();
		~VulkanCommandListManager();

		/**
		* Create a command list manager
		* @param[in] pContext	RHI context
		* @return				True if the command list manager was created successfully, false otherwise
		*/
		b8 Create(RHI::RHIContext* pContext) override final;
		/**
		* Destroy the command list manager
		* @return				True if the command list manager was created successfully, false otherwise
		*/
		b8 Destroy() override final;

		/**
		* Create a command list for a certain queue
		* @param[in] pQueue	Queue
		* @return				Pointer to a command buffer, nullptr if creation failed
		*/
		RHI::CommandList* CreateCommandList(RHI::Queue* pQueue) override final;
		/**
		* Destroy a command list
		* @param[in] pCommandList	Command list to destroy
		* @return					True if the command list was successfully destroyed, false otherwise
		*/
		b8 DestroyCommandList(RHI::CommandList* pCommandList) override final;

		/**
		 * Create and begin a single time command list for a certain queue
		 * @param[in] pQueue	Queue
		 * @return				Pointer to a single time command buffer, nullptr if creation failed
		 */
		RHI::CommandList* CreateSingleTimeCommandList(RHI::Queue* pQueue) override final;
		/**
		 * End and estroy a singe time command list
		 * @param[in] pCommandList	Single time command list
		 * @return					True if the single time command list was successfully destroyed, false otherwise
		 */
		b8 EndSingleTimeCommandList(RHI::CommandList* pCommandList) override final;

		/**
		 * Get the command pool associated with a queue
		 * @param[in] pQueue	Queue
		 * @return				Vulkan command pool
		 */
		VkCommandPool GetCommandPool(RHI::Queue* pQueue);

	private:
		std::vector<VkCommandPool> m_CommandPools;
	};

}
