#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/Semaphore.h"

namespace Vulkan {
	
	class VulkanSemaphore final : public RHI::Semaphore
	{
	public:
		VulkanSemaphore();
		~VulkanSemaphore();

		/**
		 * Create the semaphore
		 * @return	True if the semaphore was created successfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext) override final;
		/**
		 * Destroy the semaphore
		 * @return	True if the semaphore was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		/**
		 * Get the vulkan semaphore
		 * @return	Vulkan semaphore
		 */
		VkSemaphore GetSemaphore() { return m_Semaphore; }

	private:
		VkSemaphore m_Semaphore;
	};

}
