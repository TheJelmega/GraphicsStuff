
#include "VulkanPhysicalDevice.h"
#include "VulkanHelpers.h"

namespace Vulkan {

	VulkanPhysicalDevice::VulkanPhysicalDevice()
		: m_pContext(nullptr)
		, m_PhysicalDevice(VK_NULL_HANDLE)
		, m_Properties()
		, m_Features()
		, m_MemoryProperties()
		, m_FormatProperties{}
		, m_SurfaceSupport()
		, m_GpuInfo()
	{
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
	}

	VkResult VulkanPhysicalDevice::Init(VulkanContext* pContext, VkPhysicalDevice physicalDevice)
	{
		m_pContext = pContext;
		m_PhysicalDevice = physicalDevice;

		// Retrieve properties and features
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
		vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

		// Get device info
		m_GpuInfo.vendor = Helpers::GetVendor(m_Properties.vendorID);
		m_GpuInfo.type = Helpers::GetGpuType(m_Properties.deviceType);
		m_GpuInfo.driver = Helpers::GetDriverVersion(m_Properties.driverVersion);
		memcpy(m_GpuInfo.name, m_Properties.deviceName, sizeof(m_GpuInfo.name));

		// Retrieve queue family properties
		u32 numQueueFamilies;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &numQueueFamilies, nullptr);
		m_QueueFamilyProperties.resize(numQueueFamilies);
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &numQueueFamilies, m_QueueFamilyProperties.data());

		// Retrieve memory properties
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);

		// Retrieve format properties
		m_FormatProperties[0].supported = true;
		m_FormatProperties[0].properties = {};
		for (u32 i = 1; i < VK_FORMAT_RANGE_SIZE; ++i)
		{
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, VkFormat(i), &m_FormatProperties[i].properties);
			m_FormatProperties[i].supported = m_FormatProperties[i].properties.bufferFeatures || 
											  m_FormatProperties[i].properties.linearTilingFeatures ||
											  m_FormatProperties[i].properties.optimalTilingFeatures;
		}

		// Enumerate available extensions
		u32 numExtensions = 0;
		VkResult vkres = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &numExtensions, nullptr);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of available instance extensions (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}
		m_AvailableExtensions.resize(numExtensions);
		vkres = vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &numExtensions, m_AvailableExtensions.data());
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the available instance extensions (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		// Enumerate available layers
		u32 numLayers = 0;
		vkres = vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &numLayers, nullptr);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of available instance layers (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}
		m_AvailableLayers.resize(numLayers);
		vkres = vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &numLayers, m_AvailableLayers.data());
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the available instance extensions (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		return VK_SUCCESS;
	}

	b8 VulkanPhysicalDevice::IsExtensionAvailable(const std::string& extension)
	{
		for (const VkExtensionProperties& availableExtension : m_AvailableExtensions)
		{
			if (availableExtension.extensionName == extension)
				return true;
		}
		return false;
	}

	b8 VulkanPhysicalDevice::IsLayerAvailable(const std::string& layer)
	{
		for (const VkLayerProperties& availableExtension : m_AvailableLayers)
		{
			if (availableExtension.layerName == layer)
				return true;
		}
		return false;
	}

	b8 VulkanPhysicalDevice::AreExtensionsSupported(const std::vector<const char*>& extensions)
	{
		for (const char* name : extensions)
		{
			if (!IsExtensionAvailable(name))
				return false;
		}
		return true;
	}

	b8 VulkanPhysicalDevice::AreLayersSupported(const std::vector<const char*>& layers)
	{
		for (const char* name : layers)
		{
			if (!IsLayerAvailable(name))
				return false;
		}
		return true;
	}

	void VulkanPhysicalDevice::LogInfo()
	{
		//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "\tVulkan %u.%u", m_ApiVersion.major, m_ApiVersion.minor);
		//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "\tVendor: %s", RHI::Helpers::GetGpuVendorstd::string(m_GpuInfo.vendor));
		//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "\t%s", m_GpuInfo.name);
		//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "\tdriver: %u.%u", m_GpuInfo.driver.major, m_GpuInfo.driver.minor);

		// Log device extensions
		for (VkExtensionProperties& extension : m_AvailableExtensions)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "\tdevice extension: %s, version: %u", extension.extensionName, extension.specVersion);
		}
	}

	u32 VulkanPhysicalDevice::GetQueueFamily(RHI::QueueType type)
	{
		// TODO: figure out vulkan sparse queues, since AMD has support on more queues than the main queue
		// try to find a specialized queue family for each type
		u32 curFamily = 0;

		// Present support should be initialized
		assert(m_SurfaceSupport.support.size() == m_QueueFamilyProperties.size());
		for (u32 i = 0; i < m_QueueFamilyProperties.size(); ++i)
		{
			const VkQueueFamilyProperties& props = m_QueueFamilyProperties[i];
			b8 supportPresent = m_SurfaceSupport.support[i];

			if ((type & RHI::QueueType::Graphics) != RHI::QueueType::Unknown)  // Just make sure graphics is supported
			{
				if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT && supportPresent)
				{
					return i;
				}
			}
			else if ((type & RHI::QueueType::Compute) != RHI::QueueType::Unknown) // make sure compute is supported, but try to avoid graphics
			{
				if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					if (!(props.queueFlags & VK_QUEUE_GRAPHICS_BIT))
						return i;
					curFamily = i;
				}
			}
			else if ((type & RHI::QueueType::Transfer) != RHI::QueueType::Unknown) // Make sure transfer is supported and try to avoid graphics and compute
			{
				if (props.queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					if (!(props.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(props.queueFlags & VK_QUEUE_COMPUTE_BIT))
						return i;
					curFamily = i;
				}
			}
		}

		return curFamily;
	}

	VkResult VulkanPhysicalDevice::UpdateSurfaceSupport(VkSurfaceKHR surface)
	{
		VkResult vkres;
		// Retrieve present support
		// Present support always needs to be updated, if not called vulkan gives the follwing error:
		// "vkQueuePresentKHR: Presenting image without calling vkGetPhysicalDeviceSurfaceSupportKHR!"
		for (u32 i = 0; i < m_QueueFamilyProperties.size(); ++i)
		{
			VkBool32 supported;
			vkres = vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, surface, &supported);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve present support for family %u (VkResult: %s)!", i, Helpers::GetResultstd::string(vkres));
				return vkres;
			}
			if (!m_SurfaceSupport.initialized)
				m_SurfaceSupport.support.push_back(supported == VK_TRUE);
		}

		// only needs to run once, since formats and present modes won't change
		if (!m_SurfaceSupport.initialized)
		{
			// Retrieve surface formats
			u32 numFormats;
			vkres = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, surface, &numFormats, nullptr);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of available surface present modes (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return vkres;
			}
			m_SurfaceSupport.formats.resize(numFormats);
			vkres = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, surface, &numFormats, m_SurfaceSupport.formats.data());
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the available surface present modes (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return vkres;
			}

			// Retrieve surface modes
			u32 numModes;
			vkres = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, surface, &numModes, nullptr);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of available surface present modes (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return vkres;
			}
			m_SurfaceSupport.modes.resize(numModes);
			vkres = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, surface, &numModes, m_SurfaceSupport.modes.data());
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the available surface present modes (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return vkres;
			}

			m_SurfaceSupport.initialized = true;
		}

		return VK_SUCCESS;
	}

	VkResult VulkanPhysicalDevice::QuerySurfaceCapabilities(VkSurfaceKHR surface,
		VkSurfaceCapabilitiesKHR& capabilities)
	{
		VkResult vkres = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, surface, &capabilities);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to query surface capabilities (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}
		return vkres;
	}
}
