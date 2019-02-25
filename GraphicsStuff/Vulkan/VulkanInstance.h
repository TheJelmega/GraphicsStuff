// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// VulkanInstance.h: Vulkan Instance
#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "../General/TypesAndMacros.h"
#include "../RHI/RHICommon.h"

namespace Vulkan {

	class VulkanContext;
	class VulkanPhysicalDevice;

	class VulkanInstance
	{
	public:

		VulkanInstance();
		~VulkanInstance();

		/**
		 * Initialize the instance
		 * @param[in] pContext				Vulkan context
		 * @param[in] requestedExtensions	Requested extensions to create the instance with
		 * @param[in] requestedLayers		Requested layers to create the instance with
		 * @return							True if the instance initialized successfully
		 */
		VkResult Init(VulkanContext* pContext, const std::vector<const char*>& requestedExtensions, const std::vector<const char*>& requestedLayers);
		/**
		 * Destroy the instance
		 */
		void Destroy();

		/**
		 * Check if an instance extension is available
		 * @param[in] extension		Name of the extension to check
		 * @return					True if the extension is available, false otherwise
		 */
		b8 IsExtensionAvailable(const std::string& extension);
		/**
		 * Check if an instance layer is available
		 * @param[in] layer		Name of the layer to check
		 * @return				True if the layer is available, false otherwise
		 */
		b8 IsLayerAvailable(const std::string& layer);
		/**
		 * Check if an instance extension is available
		 * @param[in] extension		Name of the extension to check
		 * @return					True if the extension is available, false otherwise
		 */
		b8 IsExtensionEnabled(const std::string& extension);
		/**
		 * Check if an instance layer is available
		 * @param[in] layer		Name of the layer to check
		 * @return				True if the layer is available, false otherwise
		 */
		b8 IsLayerEnabled(const std::string& layer);

		/**
		 * Log the available extensions to the console
		 */
		void LogAvailableExtensions();
		/**
		 * Log the available layers to the console
		 */
		void LogAvailableLayers();

		/**
		 * Enumerate the vulkan devices
		 * @param[out] physicalDevices	Physical devices
		 * @return						Vulkan result
		 */
		VkResult EnumeratePhysicalDevices(std::vector<VulkanPhysicalDevice*>& physicalDevices) const;

		/**
		 * Create a debug report callback
		 * @param[in] pUserData		User data
		 * @param[in] pfnCallback	Debug callback
		 * @param[in] level			Validation level
		 * @return					Vulkan result
		 */
		VkResult CreateDebugReportCallback(void* pUserData, PFN_vkDebugReportCallbackEXT pfnCallback, RHI::RHIValidationLevel level);
		/**
		 * Destroy the debug report callback
		 */
		void DestroyDebugReportCallback();

		/**
		 * Get the vulkan allocation callbacks
		 * @return	Vulkan allocation callbacks
		 */
		VkAllocationCallbacks* GetAllocationCallbacks() { return m_pAllocCallbacks; }

		/**
		 * Get the vulkan instance
		 * @return	Vulkan instance
		 */
		VkInstance GetInstance() { return m_Instance; }

		////////////////////////////////////////////////////////////////////////////////
#ifdef VK_USE_PLATFORM_WIN32_KHR
		/**
		* Create a win32 vk surface
		* @param[in] createInfo	Create info
		* @param[out] surface		Vulkan surface
		* @return					Vulkan result
		*/
		VkResult vkCreateSurface(const VkWin32SurfaceCreateInfoKHR& createInfo, VkSurfaceKHR& surface);
#else
#endif
		/**
		* Destroy a vk surface
		* @param[in] surface	Surface
		*/
		void vkDestroySurface(VkSurfaceKHR surface);

	private:

		VulkanContext* m_pContext;									/**< Vulkan context */

		VkAllocationCallbacks* m_pAllocCallbacks;					/**< Allocation callbacks */
		VkInstance m_Instance;										/**< Vk instance */
		std::vector<VkExtensionProperties> m_AvailableExtensions;	/**< Available vulkan extensions */
		std::vector<const char*> m_EnabledExtensions;				/**< Enabled vulkan extensions */
		std::vector<VkLayerProperties> m_AvailableLayers;			/**< Available vulkan layers */
		std::vector<const char*> m_EnabledLayers;					/**< Enabled vulkan layers */
		u32 m_VulkanApiVersion;										/**< Vulkan version */

		VkDebugReportCallbackEXT m_DebugReportCallback;				/**< Vulkan debug report callback */
	};

}
