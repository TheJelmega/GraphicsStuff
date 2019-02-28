#include "VulkanContext.h"
#include "VulkanHelpers.h"
#include "VulkanInstance.h"
#include "../RHI/IDynamicRHI.h"
#include "VulkanSwapChain.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetManager.h"
#include "../RHI/GpuInfo.h"

#include <iostream>

namespace Vulkan {

	// Vulkan allocation callbacks
	// TODO: _aligned_... is MSVC
	void* VulkanAlloc(void* pUserData, sizeT size, sizeT alignment, VkSystemAllocationScope scope)
	{
		//return g_Alloctor.Allocate(size, u16(alignment), HV_ALLOC_CONTEXT(Memory::AllocCategory::RHI));
		return _aligned_malloc(size, alignment);
	}

	void* VulkanRealloc(void* pUserData, void* pOriginal, sizeT size, sizeT alignment, VkSystemAllocationScope scope)
	{
		//return g_Alloctor.Reallocate(pOriginal, size, u16(alignment), HV_ALLOC_CONTEXT(Memory::AllocCategory::RHI));
		return _aligned_realloc(pOriginal, size, alignment);
	}

	void VulkanFree(void* pUserData, void* pMemory)
	{
		//g_Alloctor.Free(pMemory);
		_aligned_free(pMemory);
	}

	void VulkanInterAlloc(void* pUserdata, sizeT size, VkInternalAllocationType type, VkSystemAllocationScope scope)
	{
		// don't do anything for now, this is just a notification
	}

	void VulkanInternFree(void* pUserdata, sizeT size, VkInternalAllocationType type, VkSystemAllocationScope scope)
	{
		// don't do anything for now, this is just a notification
	}

	VkBool32 VulkanDebugReportCallback(
		VkDebugReportFlagsEXT                       flags,
		VkDebugReportObjectTypeEXT                  objectType,
		uint64_t                                    object,
		size_t                                      location,
		int32_t                                     messageCode,
		const char*                                 pLayerPrefix,
		const char*                                 pMessage,
		void*                                       pUserData)
	{
		//LogLevel level = Helpers::GetDebugReportLogLevel(flags);
		//const char* type = Helpers::GetObjectTypestd::string(objectType);

		//g_Logger.LogFormat(LogVulkanRHI(), level, "Debug report: [%s] Object %x %s at location %u, code %i: %s", pLayerPrefix, object, type, location, messageCode, pMessage);
		std::cout << pMessage << '\n';

		return VK_FALSE;
	}

	VulkanContext::VulkanContext()
		: RHIContext()
		, m_pInstance(nullptr)
		, m_pDevice(nullptr)
		, m_pAllocator(nullptr)
		, m_pSelectedPhysicalDevice(nullptr)
	{
		m_AllocationCallbacks.pUserData = nullptr;
		m_AllocationCallbacks.pfnAllocation = &VulkanAlloc;
		m_AllocationCallbacks.pfnReallocation = &VulkanRealloc;
		m_AllocationCallbacks.pfnFree = &VulkanFree;
		m_AllocationCallbacks.pfnInternalAllocation = &VulkanInterAlloc;
		m_AllocationCallbacks.pfnInternalFree = &VulkanInternFree;
	}

	VulkanContext::~VulkanContext()
	{
	}

	b8 VulkanContext::Init(const RHI::RHIDesc& desc, GLFWwindow* pMainWindow)
	{
		// Create Instance
		m_pInstance = new VulkanInstance();

		// TODO: figure out how to request extensions and layers
		std::vector<const char*> requestedInstanceExtensions;
		std::vector<const char*> requestedInstanceLayers;

		// Add surface extensions
		requestedInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		requestedInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#endif

		if (desc.validationLevel != RHI::RHIValidationLevel::None)
		{
			requestedInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			requestedInstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
		}

		VkResult vkres = m_pInstance->Init(this, requestedInstanceExtensions, requestedInstanceLayers);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFatal(LogVulkanRHI(), "Failed to initialize vulkan instance!");
			Destroy();
			return false;
		}

		if (!m_pInstance->IsExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME))
		{
			//g_Logger.LogFatal(LogVulkanRHI(), "Vulkan surface KHR extension not enabled!");
			Destroy();
			return false;
		}

		if (desc.validationLevel != RHI::RHIValidationLevel::None)
		{
			vkres = m_pInstance->CreateDebugReportCallback(nullptr, &VulkanDebugReportCallback, desc.validationLevel);
		}

		// Log instance info
		m_pInstance->LogAvailableExtensions();
		m_pInstance->LogAvailableLayers();

		// Retrieve physical devices
		vkres = m_pInstance->EnumeratePhysicalDevices(m_PhysicalDevices);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFatal(LogVulkanRHI(), "Failed to retrieve vulkan physical devices!");
			Destroy();
			return false;
		}

		// Create temporary renderview to query present support
		VulkanSwapChain* pTmpRenderView = new VulkanSwapChain();
		pTmpRenderView->Init(this, pMainWindow, RHI::VSyncMode::Off, nullptr);
		// And destroy the temporary renderview
		pTmpRenderView->Destroy();
		delete pTmpRenderView;

		// Log physical device info
		//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "%u physical %s found", m_PhysicalDevices.size(), m_PhysicalDevices.size() > 1 ? "devices" : "device");
		for (u32 i = 0; i < m_PhysicalDevices.size(); ++i)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Detail, "GPU %u:", i);
			m_PhysicalDevices[i]->LogInfo();
		}

		// TODO: figure out how to request extensions, layers and features
		std::vector<const char*> requestedDeviceExtensions;
		std::vector<const char*> requestedDeviceLayers;

		requestedDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		
		// Setup device features
		// TODO: figure out device features
		VkPhysicalDeviceFeatures features = {};

		// TODO: figure out if we need present support
		b8 needsPresentSupport = true;

		VulkanPhysicalDevice* pPhysicalDevice = GetMostSuitablePhysicalDevice(requestedDeviceExtensions, requestedDeviceLayers, features, needsPresentSupport);
		if (!pPhysicalDevice)
		{
			//g_Logger.LogFatal(LogVulkanRHI(), "Failed to find suitable device!");
			Destroy();
			return false;
		}

		// Setup queue info
		std::vector<VkDeviceQueueCreateInfo> deviceQueues;
		u32 queueCount = 0;
		for (const RHI::QueueInfo& info : desc.queueInfo)
		{
			queueCount += info.count;
		}
		std::vector<f32> queuePriorities;
		queuePriorities.reserve(queueCount);
		u32 queueOffset = 0;
		for (const RHI::QueueInfo& info : desc.queueInfo)
		{
			VkDeviceQueueCreateInfo queueInfo = {};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueCount = info.count;
			queueInfo.queueFamilyIndex = pPhysicalDevice->GetQueueFamily(info.type);

			f32 priority = info.priority == RHI::QueuePriority::High ? 1.f : 0.5f;
			for (u32 i = 0; i < info.count; ++i)
			{
				queuePriorities.push_back(priority);
			}
			queueInfo.pQueuePriorities = queuePriorities.data() + queueOffset;
			queueOffset = u32(queuePriorities.size());
			deviceQueues.push_back(queueInfo);
		}

		// Use all available device features for now
		features = pPhysicalDevice->GetFeatures();
		Helpers::DisableUnsupportedDeviceFeatures(pPhysicalDevice->GetFeatures(), features);

		// Create logical device
		m_pDevice = new VulkanDevice();
		vkres = m_pDevice->Init(this, pPhysicalDevice, requestedDeviceExtensions, requestedDeviceLayers, features, deviceQueues);
		if (vkres != VK_SUCCESS)
		{
			Destroy();
			return false;
		}

		// Retrieve queues
		m_Queues = m_pDevice->CreateQueues(desc.queueInfo);

		// Create memory allocator
		m_pAllocator = new VulkanAllocator();
		b8 res = m_pAllocator->Create(this);
		if (!res)
		{
			Destroy();
			return false;
		}

		// Create command list manager
		m_pCommandListManager = new VulkanCommandListManager();
		res = m_pCommandListManager->Create(this);
		if (!res)
		{
			Destroy();
			return false;
		}

		// Create descriptor set manager
		m_pDescriptorSetManager = new VulkanDescriptorSetManager();
		res = m_pDescriptorSetManager->Create(this);
		if (!res)
		{
			Destroy();
			return false;
		}

		return true;
	}

	b8 VulkanContext::Destroy()
	{
		if (m_pDescriptorSetManager)
		{
			m_pDescriptorSetManager->Destroy();
			delete m_pDescriptorSetManager;
			m_pDescriptorSetManager = nullptr;
		}

		if (m_pCommandListManager)
		{
			m_pCommandListManager->Destroy();
			delete m_pCommandListManager;
			m_pCommandListManager = nullptr;
		}

		if (m_pAllocator)
		{
			m_pAllocator->Destroy();
			delete m_pAllocator;
			m_pAllocator = nullptr;
		}

		if (m_Queues.size() > 0)
		{
			for (RHI::Queue* queue : m_Queues)
			{
				delete queue;
			}
		}
		m_Queues.clear();

		if (m_pDevice)
		{
			m_pDevice->Destroy();
			delete m_pDevice;
			m_pDevice = nullptr;
		}

		if (m_PhysicalDevices.size() > 0)
		{
			for (VulkanPhysicalDevice* pDev : m_PhysicalDevices)
			{
				delete pDev;
			}
			m_PhysicalDevices.clear();
		}

		if (m_pInstance)
		{
			m_pInstance->Destroy();
			delete m_pInstance;
			m_pInstance = nullptr;
		}

		return true;
	}

	VkResult VulkanContext::UpdateSurfaceSupport(VkSurfaceKHR surface)
	{
		for (VulkanPhysicalDevice* pDevice : m_PhysicalDevices)
		{
			VkResult vkres = pDevice->UpdateSurfaceSupport(surface);
			if (vkres != VK_SUCCESS)
			{
				return vkres;
			}
		}
		return VK_SUCCESS;
	}

	VulkanPhysicalDevice* VulkanContext::GetMostSuitablePhysicalDevice(const std::vector<const char*>& requestedExtensions,
		const std::vector<const char*>& requestedLayers, const VkPhysicalDeviceFeatures& features, b8 needsPresentSupport)
	{
		// Select the 'best fitting' physical device
		// TODO improve physical device selection
		u32 deviceIndex = u32(-1);
		RHI::GpuType devType = RHI::GpuType::Unknown;
		for (u32 i = 0; i < m_PhysicalDevices.size(); ++i)
		{
			VulkanPhysicalDevice* pPhysicalDevice = m_PhysicalDevices[i];

			// Check for extension and layer support
			b8 extensionAndLayerSupport = pPhysicalDevice->AreExtensionsSupported(requestedExtensions);
			if (!extensionAndLayerSupport)
				continue;

			extensionAndLayerSupport = pPhysicalDevice->AreLayersSupported(requestedLayers);
			if (!extensionAndLayerSupport)
				continue;

			// Check for best GPU type
			const RHI::GpuInfo& info = pPhysicalDevice->GetGpuInfo();
			if (deviceIndex == u32(-1) || info.type != devType)
			{
				if (info.type == RHI::GpuType::Discrete)
				{
					deviceIndex = i;
					devType = RHI::GpuType::Discrete;
				}
				else if (devType != RHI::GpuType::Discrete)
				{
					if (devType == RHI::GpuType::Unknown)
					{
						deviceIndex = i;
						devType = info.type;
					}
					else if (info.type == RHI::GpuType::Integrated)
					{
						deviceIndex = i;
						devType = RHI::GpuType::Integrated;
					}
					// Don't care about CPU and software renderers for now
				}
			}
		}

		if (deviceIndex == u32(-1))
			return nullptr;

		m_pSelectedPhysicalDevice = m_PhysicalDevices[deviceIndex];
		return m_pSelectedPhysicalDevice;
	}
}
