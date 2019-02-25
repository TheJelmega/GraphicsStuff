
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanQueue.h"
#include "VulkanInstance.h"

namespace Vulkan {

	VulkanDevice::VulkanDevice()
		: m_pContext()
		, m_pPhysicalDevice(nullptr)
		, m_pAllocCallbacks(nullptr)
		, m_Device(VK_NULL_HANDLE)
	{
	}

	VulkanDevice::~VulkanDevice()
	{
	}

	VkResult VulkanDevice::Init(VulkanContext* pContext, VulkanPhysicalDevice* pPhysicalDevice, const std::vector<const char*>& requestedExtensions, const std::vector<const char*>& requestedLayers, const VkPhysicalDeviceFeatures& features, const std::vector<VkDeviceQueueCreateInfo>& deviceQueues)
	{
		m_pContext = pContext;
		m_pAllocCallbacks = m_pContext->GetAllocationCallbacks();
		m_pPhysicalDevice = pPhysicalDevice;
		m_EnabledExtensions = requestedExtensions;
		m_EnabledLayers = requestedLayers;

		VkPhysicalDeviceFeatures devFeatures = features;


		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.enabledExtensionCount = u32(m_EnabledExtensions.size());
		createInfo.ppEnabledExtensionNames = m_EnabledExtensions.data();
		createInfo.enabledLayerCount = u32(m_EnabledLayers.size());
		createInfo.ppEnabledLayerNames = m_EnabledLayers.data();
		createInfo.pEnabledFeatures = &devFeatures;
		createInfo.queueCreateInfoCount = u32(deviceQueues.size());
		createInfo.pQueueCreateInfos = deviceQueues.data();

		VkResult vkres =::vkCreateDevice(m_pPhysicalDevice->GetPhysicalDevice(), &createInfo, m_pAllocCallbacks, &m_Device);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the vulkan device (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		return VK_SUCCESS;
	}

	void VulkanDevice::Destroy()
	{
		if (m_Device)
		{
			vkDestroyDevice(m_Device, m_pAllocCallbacks);
		}
	}

	b8 VulkanDevice::WaitIdle()
	{
		VkResult vkres =::vkDeviceWaitIdle(m_Device);
		return vkres == VK_SUCCESS;
	}

	b8 VulkanDevice::IsExtensionAvailable(const std::string& extension)
	{
		return m_pPhysicalDevice->IsExtensionAvailable(extension);
	}

	b8 VulkanDevice::IsLayerAvailable(const std::string& layer)
	{
		return m_pPhysicalDevice->IsLayerAvailable(layer);
	}

	b8 VulkanDevice::IsExtensionEnabled(const std::string& extension)
	{
		for (const char* name : m_EnabledExtensions)
		{
			if (name == extension)
				return true;
		}
		return false;
	}

	b8 VulkanDevice::IsLayerEnabled(const std::string& layer)
	{
		for (const char* name : m_EnabledLayers)
		{
			if (name == layer)
				return true;
		}
		return false;
	}

	std::vector<RHI::Queue*>& VulkanDevice::CreateQueues(const std::vector<RHI::QueueInfo>& deviceQueues)
	{;
		for (const RHI::QueueInfo& info : deviceQueues)
		{
			for (u32 i = 0; i < info.count; ++i)
			{
				VulkanQueue* pQueue = new VulkanQueue();
				b8 res = pQueue->Init(m_pContext, info.type, i, info.priority);
				if (!res)
				{
					//g_Logger.LogError(LogVulkanRHI(), "Failed to initialize queue!");
				}
				m_Queues.push_back(pQueue);
			}
		}
		return m_Queues;
	}

	VkQueue VulkanDevice::vkGetQueue(u32 family, u32 index)
	{
		VkQueue queue;
		vkGetDeviceQueue(m_Device, family, index, &queue);
		return queue;
	}

	u32 VulkanDevice::GetQueueFamily(RHI::QueueType type)
	{
		return m_pPhysicalDevice->GetQueueFamily(type);
	}

	RHI::Queue* VulkanDevice::GetPresentQueue()
	{
		for (RHI::Queue* pQueue : m_Queues)
		{
			if ((pQueue->GetQueueType() & RHI::QueueType::Graphics) != RHI::QueueType::Unknown)
				return pQueue;
		}
		return nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////
	VkResult VulkanDevice::vkCreateSwapchain(const VkSwapchainCreateInfoKHR& createInfo, VkSwapchainKHR& swapchain)
	{
		return ::vkCreateSwapchainKHR(m_Device, &createInfo, m_pAllocCallbacks, &swapchain);
	}

	void VulkanDevice::vkDestroySwapchain(VkSwapchainKHR swapchain)
	{
		::vkDestroySwapchainKHR(m_Device, swapchain, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkGetSwapchainImages(VkSwapchainKHR swapchain, u32& imageCount, VkImage* pImages)
	{
		return ::vkGetSwapchainImagesKHR(m_Device, swapchain, &imageCount, pImages);
	}

	VkResult VulkanDevice::vkAquireNextImage(VkSwapchainKHR swapchain, u32& nextImageIndex, VkSemaphore semaphore,
		VkFence fence, u64 timeout)
	{
		return ::vkAcquireNextImageKHR(m_Device, swapchain, timeout, semaphore, fence, &nextImageIndex);
	}

	VkResult VulkanDevice::vkCreateImage(const VkImageCreateInfo& createInfo, VkImage& image)
	{
		return ::vkCreateImage(m_Device, &createInfo, m_pAllocCallbacks, &image);
	}

	void VulkanDevice::vkDestroyImage(VkImage image)
	{
		::vkDestroyImage(m_Device, image, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateImageView(const VkImageViewCreateInfo& createInfo, VkImageView& imageView)
	{
		return ::vkCreateImageView(m_Device, &createInfo, m_pAllocCallbacks, &imageView);
	}

	void VulkanDevice::vkDestroyImageView(VkImageView imageView)
	{
		::vkDestroyImageView(m_Device, imageView, m_pAllocCallbacks);
	}

	void VulkanDevice::vkGetImageMemoryRequirements(VkImage image, VkMemoryRequirements& requirements)
	{
		::vkGetImageMemoryRequirements(m_Device, image, &requirements);
	}

	VkResult VulkanDevice::vkBindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize offset)
	{
		return ::vkBindImageMemory(m_Device, image, memory, offset);
	}

	VkResult VulkanDevice::vkCreateSampler(const VkSamplerCreateInfo& createInfo, VkSampler& sampler)
	{
		return ::vkCreateSampler(m_Device, &createInfo, m_pAllocCallbacks, &sampler);
	}

	void VulkanDevice::vkDestroySampler(VkSampler sampler)
	{
		::vkDestroySampler(m_Device, sampler, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateShaderModule(const VkShaderModuleCreateInfo& createInfo, VkShaderModule& shaderModule)
	{
		return ::vkCreateShaderModule(m_Device, &createInfo, m_pAllocCallbacks, &shaderModule);
	}

	void VulkanDevice::vkDestroyShaderModule(VkShaderModule shaderModule)
	{
		::vkDestroyShaderModule(m_Device, shaderModule, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreatePipelineLayout(const VkPipelineLayoutCreateInfo& createInfo, VkPipelineLayout& pipelineLayout)
	{
		return ::vkCreatePipelineLayout(m_Device, &createInfo, m_pAllocCallbacks, &pipelineLayout);
	}

	void VulkanDevice::vkDestroyPipelineLayout(VkPipelineLayout pipelineLayout)
	{
		::vkDestroyPipelineLayout(m_Device, pipelineLayout, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreatePipeline(const VkGraphicsPipelineCreateInfo& createInfo, VkPipeline& pipeline, VkPipelineCache cache)
	{
		return ::vkCreateGraphicsPipelines(m_Device, cache, 1, &createInfo, m_pAllocCallbacks, &pipeline);
	}

	VkResult VulkanDevice::vkCreatePipeline(const VkComputePipelineCreateInfo& createInfo, VkPipeline& pipeline, VkPipelineCache cache)
	{
		return ::vkCreateComputePipelines(m_Device, cache, 1, &createInfo, m_pAllocCallbacks, &pipeline);
	}

	void VulkanDevice::vkDestroyPipeline(VkPipeline pipeline)
	{
		::vkDestroyPipeline(m_Device, pipeline, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateRenderPass(const VkRenderPassCreateInfo& createInfo, VkRenderPass& renderPass)
	{
		return ::vkCreateRenderPass(m_Device, &createInfo, m_pAllocCallbacks, &renderPass);
	}

	void VulkanDevice::vkDestroyRenderPass(VkRenderPass renderPass)
	{
		::vkDestroyRenderPass(m_Device, renderPass, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateFramebuffer(const VkFramebufferCreateInfo& createInfo, VkFramebuffer& framebuffer)
	{
		return ::vkCreateFramebuffer(m_Device, &createInfo, m_pAllocCallbacks, &framebuffer);
	}

	void VulkanDevice::vkDestroyFramebuffer(VkFramebuffer framebuffer)
	{
		::vkDestroyFramebuffer(m_Device, framebuffer, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateCommandPool(const VkCommandPoolCreateInfo& createInfo, VkCommandPool& commandPool)
	{
		return ::vkCreateCommandPool(m_Device, &createInfo, m_pAllocCallbacks, &commandPool);
	}

	void VulkanDevice::vkDestroyCommandPool(VkCommandPool commandPool)
	{
		::vkDestroyCommandPool(m_Device, commandPool, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::VkAllocateCommandBuffer(const VkCommandBufferAllocateInfo& allocInfo,
		VkCommandBuffer* pCommandBuffers)
	{
		assert(allocInfo.commandBufferCount == 1);
		return ::vkAllocateCommandBuffers(m_Device, &allocInfo, pCommandBuffers);
	}

	void VulkanDevice::vkFreeCommandBuffers(VkCommandPool commandPool, VkCommandBuffer commandBuffer)
	{
		::vkFreeCommandBuffers(m_Device, commandPool, 1, &commandBuffer);
	}

	void VulkanDevice::vkFreeCommandBuffers(VkCommandPool commandPool, const std::vector<VkCommandBuffer> commandBuffers)
	{
		::vkFreeCommandBuffers(m_Device, commandPool, u32(commandBuffers.size()), commandBuffers.data());
	}

	VkResult VulkanDevice::vkAllocateMemory(const VkMemoryAllocateInfo& allocInfo, VkDeviceMemory& memory)
	{
		return ::vkAllocateMemory(m_Device, &allocInfo, m_pAllocCallbacks, &memory);
	}

	void VulkanDevice::vkFreeMemory(VkDeviceMemory memory)
	{
		::vkFreeMemory(m_Device, memory, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::VkMapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, void** ppData)
	{
		// Flags are 0, since until 1.1.73, flags are reserved
		return ::vkMapMemory(m_Device, memory, offset, size, 0, ppData);
	}

	void VulkanDevice::VkUnmapMemory(VkDeviceMemory memory)
	{
		::vkUnmapMemory(m_Device, memory);
	}

	VkResult VulkanDevice::vkFlushMappedMemoryRanges(const VkMappedMemoryRange& range)
	{
		return ::vkFlushMappedMemoryRanges(m_Device, 1, &range);
	}

	VkResult VulkanDevice::vkFlushMappedMemoryRanges(const std::vector<VkMappedMemoryRange>& ranges)
	{
		return ::vkFlushMappedMemoryRanges(m_Device, u32(ranges.size()), ranges.data());
	}

	VkResult VulkanDevice::vkInvalidateMappedMemoryRanges(const VkMappedMemoryRange& range)
	{
		return ::vkInvalidateMappedMemoryRanges(m_Device, 1, &range);
	}

	VkResult VulkanDevice::vkInvalidateMappedMemoryRanges(const std::vector<VkMappedMemoryRange>& ranges)
	{
		return ::vkInvalidateMappedMemoryRanges(m_Device, u32(ranges.size()), ranges.data());
	}

	VkResult VulkanDevice::vkCreateBuffer(const VkBufferCreateInfo& createInfo, VkBuffer& buffer)
	{
		return ::vkCreateBuffer(m_Device, &createInfo, m_pAllocCallbacks, &buffer);
	}

	void VulkanDevice::vkDestroyBuffer(VkBuffer buffer)
	{
		::vkDestroyBuffer(m_Device, buffer, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateBufferView(const VkBufferViewCreateInfo& createInfo, VkBufferView& bufferView)
	{
		return ::vkCreateBufferView(m_Device, &createInfo, m_pAllocCallbacks, &bufferView);
	}

	void VulkanDevice::vkDestroyBufferView(VkBufferView bufferView)
	{
		::vkDestroyBufferView(m_Device, bufferView, m_pAllocCallbacks);
	}

	void VulkanDevice::vkGetBufferMemoryRequirements(VkBuffer buffer, VkMemoryRequirements& requirements)
	{
		::vkGetBufferMemoryRequirements(m_Device, buffer, &requirements);
	}

	VkResult VulkanDevice::vkBindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize offset)
	{
		return ::vkBindBufferMemory(m_Device, buffer, memory, offset);
	}

	VkResult VulkanDevice::vkCreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo,
		VkDescriptorSetLayout& descriptorSetLayout)
	{
		return ::vkCreateDescriptorSetLayout(m_Device, &createInfo, m_pAllocCallbacks, &descriptorSetLayout);
	}

	void VulkanDevice::vkDestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
	{
		::vkDestroyDescriptorSetLayout(m_Device, descriptorSetLayout, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateDescriptorPool(const VkDescriptorPoolCreateInfo& createInfo,
		VkDescriptorPool& descriptorPool)
	{
		return ::vkCreateDescriptorPool(m_Device, &createInfo, m_pAllocCallbacks, &descriptorPool);
	}

	void VulkanDevice::vkDestroyDescriptorPool(VkDescriptorPool descriptorPool)
	{
		::vkDestroyDescriptorPool(m_Device, descriptorPool, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkAllocateDescriptorSet(const VkDescriptorSetAllocateInfo& allocInfo,
		VkDescriptorSet& descriptorPool)
	{
		assert(allocInfo.descriptorSetCount == 1);
		return ::vkAllocateDescriptorSets(m_Device, &allocInfo, &descriptorPool);
	}

	void VulkanDevice::vkFreeDescriptorSet(VkDescriptorPool pool, VkDescriptorSet set)
	{
		::vkFreeDescriptorSets(m_Device, pool, 1, &set);
	}

	void VulkanDevice::vkUpdateDescriptorSets(u32 numWrites, VkWriteDescriptorSet* writes, u32 numCopies,
		VkCopyDescriptorSet* copies)
	{
		::vkUpdateDescriptorSets(m_Device, numWrites, writes, numCopies, copies);
	}

	VkResult VulkanDevice::vkCreateSemapore(const VkSemaphoreCreateInfo& createInfo, VkSemaphore& semaphore)
	{
		return ::vkCreateSemaphore(m_Device, &createInfo, m_pAllocCallbacks, &semaphore);
	}

	void VulkanDevice::vkDestroySemaphore(VkSemaphore semaphore)
	{
		::vkDestroySemaphore(m_Device, semaphore, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkCreateFence(const VkFenceCreateInfo& createInfo, VkFence& fence)
	{
		return ::vkCreateFence(m_Device, &createInfo, m_pAllocCallbacks, &fence);
	}

	void VulkanDevice::vkDestroyFence(VkFence fence)
	{
		::vkDestroyFence(m_Device, fence, m_pAllocCallbacks);
	}

	VkResult VulkanDevice::vkResetFence(VkFence fence)
	{
		return ::vkResetFences(m_Device, 1, &fence);
	}

	VkResult VulkanDevice::vkResetFences(std::vector<VkFence>& fences)
	{
		return ::vkResetFences(m_Device, u32(fences.size()), fences.data());
	}

	VkResult VulkanDevice::vkWaitForFence(VkFence fence, u64 timeout)
	{
		return ::vkWaitForFences(m_Device, 1, &fence, true, timeout);
	}

	VkResult VulkanDevice::vkWaitForFences(std::vector<VkFence>& fences, b8 waitForAll, u64 timeout)
	{
		return ::vkWaitForFences(m_Device, u32(fences.size()), fences.data(), waitForAll, timeout);
	}

	VkResult VulkanDevice::vkGetFenceStatus(VkFence& fence)
	{
		return ::vkGetFenceStatus(m_Device, fence);
	}
}