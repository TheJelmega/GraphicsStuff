#include "VulkanInstance.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"

namespace Vulkan {


	VulkanInstance::VulkanInstance()
		: m_pContext(nullptr)
		, m_pAllocCallbacks(nullptr)
		, m_Instance(VK_NULL_HANDLE)
		, m_VulkanApiVersion(VK_API_VERSION_1_0)
		, m_DebugReportCallback(VK_NULL_HANDLE)
	{
	}

	VulkanInstance::~VulkanInstance()
	{
	}

	VkResult VulkanInstance::Init(VulkanContext* pContext, const std::vector<const char*>& requestedExtensions, const std::vector<const char*>& requestedLayers)
	{
		m_pContext = pContext;
		m_pAllocCallbacks = m_pContext->GetAllocationCallbacks();

		// Enumerate available extensions
		u32 numExtensions = 0;
		VkResult vkres = vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, nullptr);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of available instance extensions (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}
		m_AvailableExtensions.resize(numExtensions);
		vkres = vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, m_AvailableExtensions.data());
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the available instance extensions (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		// Enumerate available layers
		u32 numLayers = 0;
		vkres = vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of available instance layers (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}
		m_AvailableLayers.resize(numLayers);
		vkres = vkEnumerateInstanceLayerProperties(&numLayers, m_AvailableLayers.data());
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the available instance extensions (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		// Retrieve the current vulkan version, if function isn't loaded, use default version (1.0)
		if (vkEnumerateInstanceVersion != nullptr)
		{
			vkEnumerateInstanceVersion(&m_VulkanApiVersion);
		}

		for (const char* requested : requestedExtensions)
		{
			if (IsExtensionAvailable(requested))
				m_EnabledExtensions.push_back(requested);
		}

		for (const char* requested : requestedLayers)
		{
			if (IsLayerAvailable(requested))
				m_EnabledLayers.push_back(requested);
		}

		VkApplicationInfo applicationInfo = {};
		applicationInfo.apiVersion = m_VulkanApiVersion;
		// TODO: Add app info ???

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &applicationInfo;
		createInfo.enabledExtensionCount = u32(m_EnabledExtensions.size());
		createInfo.ppEnabledExtensionNames = m_EnabledExtensions.data();
		createInfo.enabledLayerCount = u32(m_EnabledLayers.size());
		createInfo.ppEnabledLayerNames = m_EnabledLayers.data();

		vkres = vkCreateInstance(&createInfo, m_pAllocCallbacks, &m_Instance);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create vulkan instance (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		return VK_SUCCESS;
	}

	void VulkanInstance::Destroy()
	{
		DestroyDebugReportCallback();
		if (m_Instance)
		{
			vkDestroyInstance(m_Instance, m_pAllocCallbacks);
			m_Instance = VK_NULL_HANDLE;
		}
	}

	b8 VulkanInstance::IsExtensionAvailable(const std::string& extension)
	{
		for (const VkExtensionProperties& availableExtension : m_AvailableExtensions)
		{
			if (availableExtension.extensionName == extension)
				return true;
		}
		return false;
	}

	b8 VulkanInstance::IsLayerAvailable(const std::string& layer)
	{
		for (const VkLayerProperties& availableExtension : m_AvailableLayers)
		{
			if (availableExtension.layerName == layer)
				return true;
		}
		return false;
	}

	b8 VulkanInstance::IsExtensionEnabled(const std::string& extension)
	{
		for (const char* name : m_EnabledExtensions)
		{
			if (name == extension)
				return true;
		}
		return false;
	}

	b8 VulkanInstance::IsLayerEnabled(const std::string& layer)
	{
		for (const char* name : m_EnabledLayers)
		{
			if (name == layer)
				return true;
		}
		return false;
	}

	void VulkanInstance::LogAvailableExtensions()
	{
		for (const VkExtensionProperties& extension : m_AvailableExtensions)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "instance extension: %s, version: %u", extension.extensionName, extension.specVersion);
		}
	}

	void VulkanInstance::LogAvailableLayers()
	{
		for (const VkLayerProperties& layer : m_AvailableLayers)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "instance layer: %s, spec version: %u, implementation version: %u, description: %s",
			//					layer.layerName, layer.specVersion, layer.implementationVersion, layer.description);
		}
	}

	VkResult VulkanInstance::EnumeratePhysicalDevices(std::vector<VulkanPhysicalDevice*>& physicalDevices) const
	{
		u32 numPhysicalDevices;
		VkResult vkres = vkEnumeratePhysicalDevices(m_Instance, &numPhysicalDevices, nullptr);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the number of physical devices (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}
		std::vector<VkPhysicalDevice> vulkanDevices(numPhysicalDevices);
		vkres = vkEnumeratePhysicalDevices(m_Instance, &numPhysicalDevices, vulkanDevices.data());
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to retrieve the physical devices (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		physicalDevices.reserve(numPhysicalDevices);
		for (VkPhysicalDevice device : vulkanDevices)
		{
			VulkanPhysicalDevice* pDev = new VulkanPhysicalDevice();
			vkres = pDev->Init(m_pContext, device);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to initialize the physical devices (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return vkres;
			}
			physicalDevices.push_back(pDev);
		}

		return VK_SUCCESS;
	}

	VkResult VulkanInstance::CreateDebugReportCallback(void* pUserData, PFN_vkDebugReportCallbackEXT pfnCallback, RHI::RHIValidationLevel level)
	{
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.pUserData = pUserData;
		createInfo.pfnCallback = pfnCallback;

		switch (level)
		{
		case RHI::RHIValidationLevel::DebugInfo:
			createInfo.flags |= VK_DEBUG_REPORT_DEBUG_BIT_EXT;
		case RHI::RHIValidationLevel::Info:
			createInfo.flags |= VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
		case RHI::RHIValidationLevel::Performance:
			createInfo.flags |= VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		case RHI::RHIValidationLevel::Warning:
			createInfo.flags |= VK_DEBUG_REPORT_WARNING_BIT_EXT;
		case RHI::RHIValidationLevel::Error:
			createInfo.flags |= VK_DEBUG_REPORT_ERROR_BIT_EXT;
		case RHI::RHIValidationLevel::None:
		default:
			break;
		}

		PFN_vkCreateDebugReportCallbackEXT createCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugReportCallbackEXT");
		VkResult vkres = createCallback(m_Instance, &createInfo, m_pAllocCallbacks, &m_DebugReportCallback);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create debug callback (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}
		return vkres;
	}

	void VulkanInstance::DestroyDebugReportCallback()
	{
		if (m_DebugReportCallback)
		{
			PFN_vkDestroyDebugReportCallbackEXT destroyCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");
			destroyCallback(m_Instance, m_DebugReportCallback, m_pAllocCallbacks);
		}
	}

#ifdef VK_USE_PLATFORM_WIN32_KHR
	VkResult VulkanInstance::vkCreateSurface(const VkWin32SurfaceCreateInfoKHR& createInfo, VkSurfaceKHR& surface)
	{
		return vkCreateWin32SurfaceKHR(m_Instance, &createInfo, m_pAllocCallbacks, &surface);
	}
#else
#endif

	void VulkanInstance::vkDestroySurface(VkSurfaceKHR surface)
	{
		vkDestroySurfaceKHR(m_Instance, surface, m_pAllocCallbacks);
	}
}