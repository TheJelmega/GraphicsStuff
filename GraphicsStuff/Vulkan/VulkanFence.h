#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/Fence.h"

namespace Vulkan {
	
	class VulkanFence final : public RHI::Fence
	{
	public:
		VulkanFence();
		~VulkanFence();

		/**
		 * Create the fence
		 * @param[in] pContext	RHI context
		 * @param[in] signaled	If the fence is set to signalled on creation
		 * @return				True if the semaphore was fence successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext, b8 signaled) override final;
		/**
		 * Destroy the fence
		 * @return	True if the fence was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;
		/**
		 * Update the fence state
		 */
		void Tick() override final;
		/**
		 * Wait for the fence
		 * @param[in] timeout	Timeout
		 * @return	True if the fence was waited for successfully
		 */
		b8 Wait(u64 timeout = u64(-1)) override final;
		/**
		 * Reset the fence state
		 * @return	True if the fence was reset successfully, false otherwise
		 */
		b8 Reset() override final;

		/**
		 * Get the vulkan fence
		 * @return	Vulkan fence
		 */
		VkFence GetFence() { return m_Fence; }

	private:
		VkFence m_Fence;
	};

}
