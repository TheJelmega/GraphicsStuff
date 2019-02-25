#pragma once

#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>
#include "../General/TypesAndMacros.h"
#include "../RHI/Queue.h"
#include "../RHI/GpuInfo.h"

namespace Vulkan {
	class VulkanContext;

	class VulkanPhysicalDevice
	{
	public:
		struct FormatProperties
		{
			b8 supported;					/**< If the format is supported */
			VkFormatProperties properties;	/**< Format properties */
		};

		struct SurfaceSupport
		{
			std::vector<b8> support;					/**< Present supported, for each queue family */
			std::vector<VkSurfaceFormatKHR> formats;	/**< Available surface formats */
			std::vector<VkPresentModeKHR> modes;		/**< Available surface modes */
			b8 initialized;							/**< Wheter the surface support is initialized */
		};

	public:

		VulkanPhysicalDevice();
		~VulkanPhysicalDevice();

		/**
		 * Initialize the physical device
		 * @param[in] pContext					Vulkan context
		 * @param[in] physicalDevice			Vk physical device
		 * @return								Vulkan result
		 */
		VkResult Init(VulkanContext* pContext, VkPhysicalDevice physicalDevice);

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
		* Check if all instance extensions are available
		* @param[in] extensions		Names of the extensions to check
		* @return					True if all extensions are available, false otherwise
		*/
		b8 AreExtensionsSupported(const std::vector<const char*>& extensions);
		/**
		* Check if all instance layers are available
		* @param[in] layers		Names of the layers to check
		* @return				True if the layers are available, false otherwise
		*/
		b8 AreLayersSupported(const std::vector<const char*>& layers);

		/**
		 * Log physical device info
		 */
		void LogInfo();

		/**
		* Get the queue family from the corresponding type
		* @param[in] type	Queue type
		* @return			Queue family index
		*/
		u32 GetQueueFamily(RHI::QueueType type);

		/**
		 * Update the physical device surface support
		 * @param[in] surface	Vulkan surface
		 */
		VkResult UpdateSurfaceSupport(VkSurfaceKHR surface);
		/**
		 * Query the surface capabilities for a surface
		 * @param[in] surface			Surface to query capabilities for
		 * @param[out] capabilities		Surface capabilities
		 * @return						Vulkan result
		 */
		VkResult QuerySurfaceCapabilities(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR& capabilities);

		/**
		 * Check if a vulkan format is supported by the device
		 * @param[in] format	Format to query support for
		 * @return				True if the device supports the format, false otherwise
		 */
		b8 IsFormatSupported(VkFormat format) const { return m_FormatProperties[format].supported; }
		/**
		 * Check if a vulkan format is supports a feature for optimal image tiling on the device
		 * @param[in] format	Format to query support for
		 * @param[in] feature	Feature to query
		 * @return				True if the device supports the feature for the format, false otherwise
		 */
		b8 IsFormatFeatureSupportedOptimal(VkFormat format, VkFormatFeatureFlags feature) const { return (m_FormatProperties[format].properties.optimalTilingFeatures & feature) == feature; }
		/**
		 * Check if a vulkan format is supports a feature for linear image tiling on the device
		 * @param[in] format	Format to query support for
		 * @param[in] feature	Feature to query
		 * @return				True if the device supports the feature for the format, false otherwise
		 */
		b8 IsFormatFeatureSupportedLinear(VkFormat format, VkFormatFeatureFlags feature) const { return (m_FormatProperties[format].properties.linearTilingFeatures & feature) == feature; }
		/**
		 * Check if a vulkan format is supports a feature for buffers on the device
		 * @param[in] format	Format to query support for
		 * @param[in] feature	Feature to query
		 * @return				True if the device supports the feature for the format, false otherwise
		 */
		b8 IsFormatFeatureSupportedBuffer(VkFormat format, VkFormatFeatureFlags feature) const { return (m_FormatProperties[format].properties.bufferFeatures & feature) == feature; }

		/**
		 * Get the vulkan physical device
		 * @return	Vulkan physical device
		 */
		VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		/**
		 * Get the physical device features
		 * @return	Physical device features
		 */
		const VkPhysicalDeviceFeatures& GetFeatures() const { return m_Features; }
		/**
		* Get the physical device properties
		* @return	Physical device properties
		*/
		const VkPhysicalDeviceProperties& GetProperties() const { return m_Properties; }
		/**
		* Get the physical device limits
		* @return	Physical device limits
		*/
		const VkPhysicalDeviceLimits& GetLimits() const { return m_Properties.limits; }
		/**
		* Get the physical device surface support
		* @return	Physical device surface support
		*/
		const SurfaceSupport& GetSurfaceSupport() const { return m_SurfaceSupport; }
		/**
		* Get the physical device memory properties
		* @return	Physical device memory properties
		*/
		const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return m_MemoryProperties; }

		RHI::GpuInfo GetGpuInfo() const { return m_GpuInfo; }

	private:
		VulkanContext* m_pContext;										/**< Vulkan context */

		VkPhysicalDevice m_PhysicalDevice;								/**< Vk Physical device */
		VkPhysicalDeviceProperties m_Properties;						/**< Physical device properties */
		VkPhysicalDeviceFeatures m_Features;							/**< Physical device features */
		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;	/**< Physical device queue family properties */
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;			/**< Physical device memory properties */
		FormatProperties m_FormatProperties[VK_FORMAT_RANGE_SIZE];		/**< Format properties */
		SurfaceSupport m_SurfaceSupport;								/**< Surface support */

		std::vector<VkExtensionProperties> m_AvailableExtensions;		/**< Available extensions */
		std::vector<VkLayerProperties> m_AvailableLayers;				/**< Available extensions */

		RHI::GpuInfo m_GpuInfo;
	};

}
