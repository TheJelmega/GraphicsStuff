#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "../General/TypesAndMacros.h"
#include "../RHI/Queue.h"

namespace RHI {
	class RHIContext;
}

namespace Vulkan {
	
	class VulkanQueue final : public RHI::Queue
	{
	public:
		VulkanQueue();
		~VulkanQueue();

		/**
		 * Initialize the queue
		 * @param[in] pContext	RHI context
		 * @param[in] type		Queue type
		 * @param[in] index		Queue index
		 * @param[in] priority	Queue priority
		 * @return				True if the queue was initialize successfully, false otherwise
		 */
		b8 Init(RHI::RHIContext* pContext, RHI::QueueType type, u32 index, RHI::QueuePriority priority) override final;

		/**
		* Wait for the queue to be idle
		* @return	True if queue is idle, false otherwise
		*/
		b8 WaitIdle() override final;

		/**
		 * Get the vulkan queue family index
		 * @return	Vulkan queue family index
		 */
		u32 GetQueueFamily() const { return m_Family; }
		/**
		 * Get the vulkan queue
		 * @return	Vulkan queue
		 */
		VkQueue GetQueue() const { return m_Queue; }

		/**
		 * Submit to a vk queue
		 * @param[in] submitInfo	Vk submit info
		 * @param[in] fence			Vk fence
		 * @return					Vulkan result
		 */
		VkResult vkSubmit(const VkSubmitInfo& submitInfo, VkFence fence);
		/**
		 * Submit to a vk queue
		 * @param[in] submitInfo	Vk submit info
		 * @param[in] fence			Vk fence
		 * @return					Vulkan result
		 */
		VkResult vkSubmit(const std::vector<VkSubmitInfo>& submitInfo, VkFence fence);
		/**
		 * Present a vk swapchain
		 * @param[in] presentInfo	Vk present info
		 * @return					Vulkan result
		 */
		VkResult vkPresent(const VkPresentInfoKHR& presentInfo);

	private:
		VkQueue m_Queue;
		u32 m_Family;
	};

}
