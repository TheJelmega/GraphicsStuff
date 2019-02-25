#pragma once
#include <vulkan/vulkan.h>
#include "VulkanPipeline.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandListManager.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorSet.h"
#include "../RHI/Queue.h"

namespace Vulkan {
	class VulkanQueue;
	class VulkanPhysicalDevice;
	class VulkanContext;

	class VulkanDevice
	{
	public:
		VulkanDevice();
		~VulkanDevice();

		/**
		 * Initialize the vulkan logical device
		 * @param[in] pContext				Vulkan context
		 * @param[in] pPhysicalDevice		Physical device
		 * @param[in] requestedExtensions	Requested extensions to create the instance with
		 * @param[in] requestedLayers		Requested layers to create the instance with.
		 * @param[in] features				Physical device features
		 * @param[in] deviceQueues			Device queue info
		 * @return							Vulkan result
		 */
		VkResult Init(VulkanContext* pContext, VulkanPhysicalDevice* pPhysicalDevice, const std::vector<const char*>& requestedExtensions, const std::vector<const char*>& requestedLayers, const VkPhysicalDeviceFeatures& features, const std::vector<VkDeviceQueueCreateInfo>& deviceQueues);
		/**
		 * Destroy the device
		 */
		void Destroy();
		/**
		* Wait for the device to be idle
		* @return	True if te device is idle, false otherwise
		*/
		b8 WaitIdle();

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
		 * Retrieve the queue of a certain type and index
		 * @param[in] deviceQueues	Queue info
		 * @return					Queue or VK_NULL_HANDLE if the device failed to retrieve the queue
		 */
		std::vector<RHI::Queue*>& CreateQueues(const std::vector<RHI::QueueInfo>& deviceQueues);
		/**
		* Retrieve the queue of a certain family and index
		* @param[in] family		Queue family
		* @param[in] index		Queue index
		* @return				Queue or VK_NULL_HANDLE if the device failed to retrieve the queue
		*/
		VkQueue vkGetQueue(u32 family, u32 index);
		/**
		 * Get the queue family from the corresponding type
		 * @param[in] type	Queue type
		 * @return			Queue family index
		 */
		u32 GetQueueFamily(RHI::QueueType type);
		/**
		 * Get the present queue
		 * @return	Present queue
		 */
		RHI::Queue* GetPresentQueue();
		/**
		 * Get the queues
		 * @return	Array with queues
		 */
		std::vector<RHI::Queue*>& GetQueues() { return m_Queues; }

		/**
		 * Get the physical device where the logical device was created on
		 * @return Physical device
		 */
		VulkanPhysicalDevice* GetPhysicalDevice() { return m_pPhysicalDevice; }
		/**
		 * Get the vulkan device
		 * @return Vulkan device
		 */
		VkDevice GetDevice() { return m_Device; }
		/**
		 * Get the vulkan allocation callbacks
		 * @return	Vulkan allocation callbacks
		 */
		VkAllocationCallbacks* GetAllocationCallbacks() { return m_pAllocCallbacks; }

		////////////////////////////////////////////////////////////////////////////////
		/**
		 * Create a vk swapchain
		 * @param[in] createInfo	Create info
		 * @param[out] swapchain	Swapchain
		 * @return					Vulkan result
		 */
		VkResult vkCreateSwapchain(const VkSwapchainCreateInfoKHR& createInfo, VkSwapchainKHR& swapchain);
		/**
		 * Destroy a vk swapchain
		 * @param[in] swapchain	Swapchain
		 */
		void vkDestroySwapchain(VkSwapchainKHR swapchain);
		/**
		 * Get the swapchain images
		 * @param[in] swapchain		Swapchain
		 * @param[out] imageCount	Number of images
		 * @param[out] pImages		Images
		 * @return					Vulkan result
		 */
		VkResult vkGetSwapchainImages(VkSwapchainKHR swapchain, u32& imageCount, VkImage* pImages);
		/**
		* Get the swapchain images
		* @param[in] swapchain			Swapchain
		* @param[out] nextImageIndex	Index of the next image
		* @param[in] semaphore			Semaphore to signal
		* @param[in] fence				Fence to signal
		* @param[in] timeout			Timout to acquire the next image (ns)
		* @return						Vulkan result
		*/
		VkResult vkAquireNextImage(VkSwapchainKHR swapchain, u32& nextImageIndex, VkSemaphore semaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE, u64 timeout = u64(-1));

		/**
		 * Create a vk image
		 * @param[in] createInfo	Create info
		 * @param[out] image		Image
		 * @return					Vulkan result
		 */
		VkResult vkCreateImage(const VkImageCreateInfo& createInfo, VkImage& image);
		/**
		 * Destroy a vk image
		 * @param[in] image		Image
		 */
		void vkDestroyImage(VkImage image);
		/**
		 * Create a vk image view
		 * @param[in] createInfo	Create info
		 * @param[out] imageView	Image view
		 * @return					Vulkan result
		 */
		VkResult vkCreateImageView(const VkImageViewCreateInfo& createInfo, VkImageView& imageView);
		/**
		 * Destroy a vk image view
		 * @param[in] imageView		Image
		 */
		void vkDestroyImageView(VkImageView imageView);
		/**
		 * Get the memory requirements for a vk image
		 * @param[in] image			Image
		 * @param[in] requirements	Memory requirement
		 */
		void vkGetImageMemoryRequirements(VkImage image, VkMemoryRequirements& requirements);
		/**
		* Bind vk memory to a vk buffer
		* @param[in] image		Image
		* @param[in] memory		Memory
		* @param[in] offset		Memory offset
		* @return				Vulkan result
		*/
		VkResult vkBindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize offset);

		/**
		 * Create a vk sampler
		 * @param[in] createInfo	Create info
		 * @param[out] sampler		Sampler
		 * @return					Vulkan result
		 */
		VkResult vkCreateSampler(const VkSamplerCreateInfo& createInfo, VkSampler& sampler);
		/**
		 * Destroy a vk sampler
		 * @param[in] sampler		Sampler
		 */
		void vkDestroySampler(VkSampler sampler);

		/**
		 * Create a vk shader module
		 * @param[in] createInfo		Create info
		 * @param[out] shaderModule		Shader module
		 * @return						Vulkan result
		 */
		VkResult vkCreateShaderModule(const VkShaderModuleCreateInfo& createInfo, VkShaderModule& shaderModule);
		/**
		 * Destroy a vk shader module
		 * @param[in] shaderModule		Shader module
		 */
		void vkDestroyShaderModule(VkShaderModule shaderModule);
		/**
		 * Create a vk pipeline layout
		 * @param[in] createInfo			Create info
		 * @param[out] pipelineLayout	Pipeline layout
		 * @return						Vulkan result
		 */
		VkResult vkCreatePipelineLayout(const VkPipelineLayoutCreateInfo& createInfo, VkPipelineLayout& pipelineLayout);
		/**
		 * Destroy a vk pipeline layout
		 * @param[in] pipeline	Pipeline layout
		 */
		void vkDestroyPipelineLayout(VkPipelineLayout pipeline);
		/**
		 * Create a vk graphics pipeline
		 * @param[in] createInfo		Create info
		 * @param[out] pipeline		Pipeline
		 * @param[in] cache			[OPTIONAL] Pipeline cache
		 * @return					Vulkan result
		 */
		VkResult vkCreatePipeline(const VkGraphicsPipelineCreateInfo& createInfo, VkPipeline& pipeline, VkPipelineCache cache = VK_NULL_HANDLE);
		/**
		 * Create a vk compute pipeline
		 * @param[in] createInfo		Create info
		 * @param[out] pipeline		Pipeline
		 * @param[in] cache			[OPTIONAL] Pipeline cache
		 * @return					Vulkan result
		 */
		VkResult vkCreatePipeline(const VkComputePipelineCreateInfo& createInfo, VkPipeline& pipeline, VkPipelineCache cache = VK_NULL_HANDLE);
		/**
		 * Destroy a vk pipeline
		 * @param[in] pipeline	Pipeline
		 */
		void vkDestroyPipeline(VkPipeline pipeline);

		/**
		 * Create a vk render pass
		 * @param[in] createInfo		Create info
		 * @param[out] renderPass	Render pass
		 * @return					Vulkan result
		 */
		VkResult vkCreateRenderPass(const VkRenderPassCreateInfo& createInfo, VkRenderPass& renderPass);
		/**
		 * Destroy a vk render pass
		 * @param[in] renderPass	Render pass
		 */
		void vkDestroyRenderPass(VkRenderPass renderPass);

		/**
		 * Create a vk framebuffer
		 * @param[in] createInfo		Create info
		 * @param[out] framebuffer	Framebuffer
		 * @return					Vulkan result
		 */
		VkResult vkCreateFramebuffer(const VkFramebufferCreateInfo& createInfo, VkFramebuffer& framebuffer);
		/**
		 * Destroy a vk framebuffer
		 * @param[in] framebuffer	Framebuffer
		 */
		void vkDestroyFramebuffer(VkFramebuffer framebuffer);

		/**
		 * Create a vk command pool
		 * @param[in] createInfo		Create info
		 * @param[out] commandPool	CommandPool
		 * @return					Vulkan result
		 */
		VkResult vkCreateCommandPool(const VkCommandPoolCreateInfo& createInfo, VkCommandPool& commandPool);
		/**
		 * Destroy a vk commandPool
		 * @param[in] commandPool	CommandPool
		 */
		void vkDestroyCommandPool(VkCommandPool commandPool);

		/**
		 * Allocate a command buffer
		 * @param[in] allocInfo			Allocation info
		 * @param[out] pCommandBuffers	Command buffer array
		 * @return						Vulkan result
		 */
		VkResult VkAllocateCommandBuffer(const VkCommandBufferAllocateInfo& allocInfo, VkCommandBuffer* pCommandBuffers);
		/**
		 * Free a command buffer
		 * @param[in] commandPool		Command pool which was used to allocate the buffer
		 * @param[in] commandBuffer		Command buffer
		 */
		void vkFreeCommandBuffers(VkCommandPool commandPool, VkCommandBuffer commandBuffer);
		/**
		 * Free a command buffer
		 * @param[in] commandPool		Command pool which was used to allocate the buffer
		 * @param[in] commandBuffers		Command buffers
		 */
		void vkFreeCommandBuffers(VkCommandPool commandPool, const std::vector<VkCommandBuffer> commandBuffers);

		/**
		 * Allocate vulkan memory
		 * @param[in] allocInfo		Allocation info
		 * @param[out] memory		Memory
		 * @return					Vulkan result
		 */
		VkResult vkAllocateMemory(const VkMemoryAllocateInfo& allocInfo, VkDeviceMemory& memory);
		/**
		 * Free vulkan memory
		 * @param[in] memory	Memory
		 */
		void vkFreeMemory(VkDeviceMemory memory);
		/**
		 * Map vulkan memory
		 * @param[in] memory	Memory
		 * @param[in] offset	Offset of mapped memory
		 * @param[in] size		Size of mapped memory
		 * @param[out] ppData	Mapped data
		 * @return				Vulkan result
		 */
		VkResult VkMapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, void** ppData);
		/**
		 * Unmap vulkan memory
		 * @param[in] memory	Memory
		 */
		void VkUnmapMemory(VkDeviceMemory memory);
		/**
		 * Flush a mapped vulkan memory range
		 * @param[in] range		Range to flush
		 */
		VkResult vkFlushMappedMemoryRanges(const VkMappedMemoryRange& range);
		/**
		 * Flush a mapped vulkan memory ranges
		 * @param[in] ranges	Ranges to flush
		 */
		VkResult vkFlushMappedMemoryRanges(const std::vector<VkMappedMemoryRange>& ranges);
		/**
		 * Flush a mapped vulkan memory range
		 * @param[in] range		Range to flush
		 */
		VkResult vkInvalidateMappedMemoryRanges(const VkMappedMemoryRange& range);
		/**
		 * Flush a mapped vulkan memory ranges
		 * @param[in] ranges	Ranges to flush
		 */
		VkResult vkInvalidateMappedMemoryRanges(const std::vector<VkMappedMemoryRange>& ranges);

		/**
		 * Create a vk buffer
		 * @param[in] createInfo	Create info
		 * @param[out] buffer		Buffer
		 * @return					Vulkan result
		 */
		VkResult vkCreateBuffer(const VkBufferCreateInfo& createInfo, VkBuffer& buffer);
		/**
		 * Destroy a vk buffer
		 * @param[in] buffer	Buffer
		 */
		void vkDestroyBuffer(VkBuffer buffer);
		/**
		 * Create a vk buffer view
		 * @param[in] createInfo	Create info
		 * @param[out] bufferView	Buffer view
		 * @return					Vulkan result
		 */
		VkResult vkCreateBufferView(const VkBufferViewCreateInfo& createInfo, VkBufferView& bufferView);
		/**
		 * Destroy a vk buffer view
		 * @param[in] bufferView	Buffer view
		 */
		void vkDestroyBufferView(VkBufferView bufferView);
		/**
		 * Get the memory requirements for a vk buffer
		 * @param[in] buffer			Buffer
		 * @param[in] requirements	Memory requirement
		 */
		void vkGetBufferMemoryRequirements(VkBuffer buffer, VkMemoryRequirements& requirements);
		/**
		 * Bind vk memory to a vk buffer
		 * @param[in] buffer		Buffer
		 * @param[in] memory		Memory
		 * @param[in] offset		Memory offset
		 * @return				Vulkan result
		 */
		VkResult vkBindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize offset);

		/**
		 * Create a vk descriptor set layout
		 * @param[in] createInfo			Create info
		 * @param[out] descriptorSetLayout	Descriptor set layout
		 * @return							Vulkan result
		 */
		VkResult vkCreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo, VkDescriptorSetLayout& descriptorSetLayout);
		/**
		 * Destroy a vk descriptor set layout
		 * @param[in] descriptorSetLayout		Descriptor set layout
		 */
		void vkDestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);
		/**
		 * Create a vk descriptor pool
		 * @param[in] createInfo		Create info
		 * @param[out] descriptorPool	Descriptor pool
		 * @return						Vulkan result
		 */
		VkResult vkCreateDescriptorPool(const VkDescriptorPoolCreateInfo& createInfo, VkDescriptorPool& descriptorPool);
		/**
		 * Destroy a vk descriptor pool
		 * @param[in] descriptorPool		Descriptor pool
		 */
		void vkDestroyDescriptorPool(VkDescriptorPool descriptorPool);
		/**
		 * Create a vk descriptor pool
		 * @param[in] allocInfo			Allocation info
		 * @param[out] descriptorPool	Descriptor pool
		 * @return						Vulkan result
		 */
		VkResult vkAllocateDescriptorSet(const VkDescriptorSetAllocateInfo& allocInfo, VkDescriptorSet& descriptorPool);
		/**
		 * Destroy a vk descriptor set
		 * @param[in] pool	Descriptor pool
		 * @param[in] set	Descriptor set
		 */
		void vkFreeDescriptorSet(VkDescriptorPool pool, VkDescriptorSet set);
		/**
		* Update vk descriptor sets
		* @param[in] numWrites	Number of write descriptor sets
		* @param[in] writes		Write descriptor sets
		* @param[in] numWrites	Number of copy descriptor sets
		* @param[in] writes		Copy descriptor sets
		*/
		void vkUpdateDescriptorSets(u32 numWrites, VkWriteDescriptorSet* writes, u32 numCopies, VkCopyDescriptorSet* copies);

		/**
		 * Create a vk semaphore
		 * @param[in] createInfo	Create info
		 * @param[out] semaphore	Semaphore
		 * @return					Vulkan result
		 */
		VkResult vkCreateSemapore(const VkSemaphoreCreateInfo& createInfo, VkSemaphore& semaphore);
		/**
		 * Destroy a vk semaphore
		 * @param[in] semaphore		Semaphore
		 */
		void vkDestroySemaphore(VkSemaphore semaphore);

		/**
		 * Create a vk fence
		 * @param[in] createInfo	Create info
		 * @param[out] fence		Fence
		 * @return					Vulkan result
		 */
		VkResult vkCreateFence(const VkFenceCreateInfo& createInfo, VkFence& fence);
		/**
		 * Destroy a vk fence
		 * @param[in] fence		Fence
		 */
		void vkDestroyFence(VkFence fence);
		/**
		* Reset a vk fence
		* @param[in] fence	Fence to reset
		* @return			Vulkan result
		*/
		VkResult vkResetFence(VkFence fence);
		/**
		* reset a vk fence fences
		* @param[in] fences		Fences to reset
		* @return				Vulkan result
		*/
		VkResult vkResetFences(std::vector<VkFence>& fences);
		/**
		 * Wait for a vk fence
		 * @param[in] fence			Fence to wait for
		 * @param[in] timeout		Wait timeout
		 * @return					Vulkan result
		 */
		VkResult vkWaitForFence(VkFence fence, u64 timeout = u64(-1));
		/**
		 * Wait for a vk fences
		 * @param[in] fences			Fences to wait for
		 * @param[in] waitForAll		If all fences need to be waited on
		 * @param[in] timeout		Wait timeout
		 * @return					Vulkan result
		 */
		VkResult vkWaitForFences(std::vector<VkFence>& fences, b8 waitForAll, u64 timeout = u64(-1));
		/**
		 * Get a vk fence status
		 * @param[in] fence			Fence
		 * @return					Vulkan result
		 */
		VkResult vkGetFenceStatus(VkFence& fence);


	private:

		VulkanContext* m_pContext;								/**< Vulkan context */
		VulkanPhysicalDevice* m_pPhysicalDevice;				/**< Vulkan physical device */

		VkAllocationCallbacks* m_pAllocCallbacks;				/**< Allocation calbacks */
		VkDevice m_Device;										/**< Vk device */
		std::vector<const char*> m_EnabledExtensions;			/**< Enabled extensions */
		std::vector<const char*> m_EnabledLayers;				/**< Enabled extensions */
		std::vector<RHI::Queue*> m_Queues;				/**< Device queues */
	};

}
