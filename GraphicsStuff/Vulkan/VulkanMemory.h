// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// VulkanMemory.h: Vulkan memory functions
#pragma once
#include "../General/TypesAndMacros.h"
#include <utility>
#include <vector>

namespace RHI {
	class RHIContext;
}

namespace Vulkan {

	class VulkanContext;

	enum class VulkanAllocationMapMode : u8
	{
		Write,			/**< Write to memory */
		Read,			/**< Read from memory */
	};
	
	class VulkanAllocation
	{
	public:
		VulkanAllocation();
		~VulkanAllocation();

		/**
		 * Map the memory
		 * @param [in] offset	Offset in allocation
		 * @param [in] size		Size/range to map
		 * @param [in] mapMode	Map mode
		 * @return	Pointer to mapped data, nullptr if mapping failed
		 */
		void* Map(VkDeviceSize offset, VkDeviceSize size, VulkanAllocationMapMode mapMode);
		/**
		 * Unmap the memory
		 * @return	True if the memory was unmapped successfully, false otherwise
		 */
		b8 Unmap();

		/**
		 * Get the vulkan memory
		 * @return	Vulkan memory
		 */
		VkDeviceMemory GetMemory() { return m_Memory; }
		/**
		 * Get the allocation sise
		 * @return	Allocation size
		 */
		VkDeviceSize GetSize() { return m_Size; }
		/**
		 * Get the memory size
		 * @return	Memory size
		 */
		VkDeviceSize GetOffset() { return m_Offset; }

	private:
		friend class VulkanAllocator;

		VulkanContext* m_pContext;			/**< Vulkan context */
		VkDeviceMemory m_Memory;			/**< Device memory */
		VkDeviceSize m_Size;				/**< Allocated size */
		VkDeviceSize m_Offset;				/**< Memory offset */
		u32 m_HeapIndex;					/**< Heap index */

		b8 m_IsMapped;						/**< Is the memory mapped */
		VulkanAllocationMapMode m_MapMode;	/**< Allocation map mode */
		u64 m_MapOffset;					/**< Offset of mapped data */
		u64 m_MapSize;						/**< Size of mapped data */
	};

	struct HeapInfo
	{
		VkDeviceSize totalSize;
		VkDeviceSize usedSize;

		VkMemoryHeapFlags flags;
	};

	// TODO: Improve allocation (not just pass through)
	class VulkanAllocator
	{
	public:
		VulkanAllocator();
		~VulkanAllocator();

		/**
		 * Create the vulkan memory allocator
		 * @param[in] pContext	RHI context
		 * @return				True if the allocator was created sucessfully, false otherwise
		 */
		b8 Create(RHI::RHIContext* pContext);
		/**
		 * Destroy the vulkan memory allocator
		 * @return				True if the allocator was destroyed sucessfully, false otherwise
		 */
		b8 Destroy();

		/**
		 * Allocate vulkan memory
		 * @param[in] requirements	Memory requirements
		 * @param[in] memProps		Memory properties
		 * @return	Pointer to vlkan allocation, nullptr if allocation failed
		 */
		VulkanAllocation* Allocate(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags memProps);
		/**
		 * Free a vulkan allocation
		 * @param[in] pAllocation	Allocation to free
		 */
		void Free(VulkanAllocation* pAllocation);

		std::pair<u32, VkMemoryType> GetMemoryType(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags memProps);

	private:
		VulkanContext* m_pContext;							/**< Vulkan context */

		VkPhysicalDeviceMemoryProperties m_MemProperties;	/**< Vulkan memory properties */
		std::vector<HeapInfo> m_HeapInfo;					/**< Heap info */

		std::vector<VulkanAllocation*> m_Allocations;		/**< Allocations */
	};
}
