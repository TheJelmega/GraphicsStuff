#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanMemory.h"
#include "../RHI/RHIContext.h"

namespace Vulkan {
	class VulkanDevice;
	class VulkanPhysicalDevice;

	class VulkanInstance;
	
	class VulkanContext final : public RHI::RHIContext
	{
	public:
	public:

		VulkanContext();
		~VulkanContext();

		/**
		 * Initialize the RHI context
		 * @return	True if the context was initialized successfully initialized, false otherwise
		 */
		b8 Init(const RHI::RHIDesc& desc, sf::Window* pMainWindow) override;
		/**
		 * Destroy the RHI context
		 * @return	True if the context was destroyed successfully initialized, false otherwise
		 */
		b8 Destroy() override;

		/**
		 * Update the physical device surface support
		 * @param[in] surface	Vulkan surface
		 */
		VkResult UpdateSurfaceSupport(VkSurfaceKHR surface);

		/**
		 * Find the most suitable physical device to create the logical device on
		 * @param[in] requestedExtensions	Requested extensions
		 * @param[in] requestedLayers		Requested layers
		 * @param[in] features				Requested features
		 * @param[in] needsPresentSupport	If the device needs present support
		 * @return							Most suitable physical device
		 */
		VulkanPhysicalDevice* GetMostSuitablePhysicalDevice(const std::vector<const char*>& requestedExtensions, const std::vector<const char*>& requestedLayers, const VkPhysicalDeviceFeatures& features, b8 needsPresentSupport);

		/**
		 * Get the vulkan allocation callbacks
		 * @return	Vulkan allocation callbacks
		 */
		VkAllocationCallbacks* GetAllocationCallbacks() { return &m_AllocationCallbacks; }

		/**
		 * Get the vulkan instance
		 * @return	Vulkan instance
		 */
		VulkanInstance* GetInstance() { return m_pInstance; }
		/**
		 * Get all vulkan physical devices
		 * @return	Vulkan physical devices
		 */
		std::vector<VulkanPhysicalDevice*>& GetPhysicalDevices() { return m_PhysicalDevices; }
		/**
		 * Get the vulkan logical device
		 * @return	Vulkan device
		 */
		VulkanDevice* GetDevice() { return m_pDevice; }
		/**
		 * Get the vulkan memory allocator
		 * @return	Vulkan memory allocator
		 */
		VulkanAllocator* GetAllocator() { return m_pAllocator; }

	private:
		VkAllocationCallbacks m_AllocationCallbacks;		/**< Vulkan allocation callbacks */
		VulkanInstance* m_pInstance;						/**< Vulkan instance */
		std::vector<VulkanPhysicalDevice*> m_PhysicalDevices;	/**< Vulkan physical devices */
		VulkanDevice* m_pDevice;							/**< Vulkan device */
		VulkanAllocator* m_pAllocator;						/**< Vulkan memory allocator */
	};

}
