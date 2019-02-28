#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/SwapChain.h"

namespace Vulkan {
	
	class VulkanSwapChain final : public RHI::SwapChain
	{
	public:
		VulkanSwapChain();
		~VulkanSwapChain();

		/**
		 * Create a render view
		 * @param[in] pContext	RHI context
		 * @param[in] pWindow	Window
		 * @param[in] vsync		Vsync mode
		 * @param[in] pQueue	Present queue
		 * @return				True if the render view was created successfully, false otherwise
		 */
		b8 Init(RHI::RHIContext* pContext, GLFWwindow* pWindow, RHI::VSyncMode vsync, RHI::Queue* pQueue) override final;

		/**
		* Destroy a render view
		* @return	True if the render view was destroyed successfully, false otherwise
		*/
		b8 Destroy() override final;

		/**
		* Present the backbuffer to the screen
		* @return	True if the backbuffer was presented successfully, false otherwise
		*/
		b8 Present() override final;

	private:

		/**
		 * Window resize callback
		 * @param[in] width		New width
		 * @param[in] height	New height
		 */
		void ResizeCallback(u32 width, u32 height);

		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_Swapchain;
	};

}
