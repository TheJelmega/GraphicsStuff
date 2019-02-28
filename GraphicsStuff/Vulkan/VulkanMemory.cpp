#pragma once
#include <vulkan/vulkan.h>

#include "VulkanMemory.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanPhysicalDevice.h"

namespace Vulkan {

	VulkanAllocation::VulkanAllocation()
		: m_pContext(nullptr)
		, m_Memory(VK_NULL_HANDLE)
		, m_Size(0)
		, m_Offset(0)
		, m_HeapIndex()
		, m_IsMapped(false)
		, m_MapMode(VulkanAllocationMapMode::Read)
		, m_MapOffset(0)
		, m_MapSize(0)
	{
	}

	VulkanAllocation::~VulkanAllocation()
	{
	}

	void* VulkanAllocation::Map(VkDeviceSize offset, VkDeviceSize size, VulkanAllocationMapMode mapMode)
	{
		if (m_IsMapped)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Can't map an allocation that is already mapped");
			return nullptr;
		}
		m_IsMapped = true;

		if (offset > m_Size)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Can't map with offset that is out of range (map offset: %u, memory size: %u)", offset, m_Size);
			m_IsMapped = false;
			return nullptr;
		}
		if (size > m_Size)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Can't map a range of memory that is larger that the total size (map range: %u, memory size: %u)", offset, m_Size);
			m_IsMapped = false;
			return nullptr;
		}
		if (offset + size > m_Size)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Can't map with a combined offset and range that is larger that the total size (map offset: %u, map range: %u, memory size: %u)", offset, size, m_Size);
			m_IsMapped = false;
			return nullptr;
		}

		m_MapOffset = offset;
		m_MapSize = size;
		m_MapMode = mapMode;

		VulkanDevice* pDevice = m_pContext->GetDevice();

		if (m_MapMode == VulkanAllocationMapMode::Read)
		{
			// Invlidate memory
			VkMappedMemoryRange range = {};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = m_Memory;
			range.offset = m_Offset + m_MapOffset;
			range.size = m_Size;
			pDevice->vkInvalidateMappedMemoryRanges(range);
		}

		void* data;
		VkResult vkres = pDevice->VkMapMemory(m_Memory, m_Offset + m_MapOffset, m_MapSize, &data);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to map vulkan memory (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return nullptr;
		}

		return data;
	}

	b8 VulkanAllocation::Unmap()
	{
		VulkanDevice* pDevice = m_pContext->GetDevice();

		VkResult vkres = VK_SUCCESS;
		if (m_MapMode == VulkanAllocationMapMode::Write)
		{
			// Flush memory
			VkMappedMemoryRange range = {};
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.memory = m_Memory;
			range.offset = m_Offset + m_MapOffset;
			range.size = m_MapSize;

			VkDeviceSize atomSize = m_pContext->GetSelectedPhysicalDevice()->GetLimits().nonCoherentAtomSize;
			VkDeviceSize atomMask = atomSize - 1;

			VkDeviceSize frontPad = range.offset & atomMask;
			if (frontPad > 0)
			{
				range.offset -= frontPad;
				range.size += frontPad;
			}
			VkDeviceSize backPad = (range.offset + range.size) & atomMask;
			if (backPad > 0)
			{
				VkDeviceSize pad = atomSize - backPad;
				range.size += pad;
			}

			if (range.offset + range.size >= m_MapSize)
				range.size = VK_WHOLE_SIZE;

			vkres = pDevice->vkFlushMappedMemoryRanges(range);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to flush vulkan memory (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			}
		}

		pDevice->VkUnmapMemory(m_Memory);

		m_IsMapped = false;

		return vkres == VK_SUCCESS;
	}

	VulkanAllocator::VulkanAllocator()
		: m_pContext(nullptr)
		, m_MemProperties()
	{
	}

	VulkanAllocator::~VulkanAllocator()
	{
	}

	b8 VulkanAllocator::Create(RHI::RHIContext* pContext)
	{
		m_pContext = (VulkanContext*)pContext;
		VulkanDevice* pDevice = m_pContext->GetDevice();
		VulkanPhysicalDevice* pPhysicalDevice = pDevice->GetPhysicalDevice();
		m_MemProperties = pPhysicalDevice->GetMemoryProperties();

		// Setup heap info
		for (u8 i = 0; i < m_MemProperties.memoryHeapCount; ++i)
		{
			HeapInfo info = {};
			const VkMemoryHeap& heap = m_MemProperties.memoryHeaps[i];
			info.totalSize = heap.size;
			info.flags = heap.flags;
			m_HeapInfo.push_back(info);
		}

		return true;
	}

	b8 VulkanAllocator::Destroy()
	{
		if (m_Allocations.size() > 0)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Destroying allocator with %u allocation still in use!", m_Allocations.size());
			return false;
		}

		return true;
	}

	VulkanAllocation* VulkanAllocator::Allocate(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags memProps)
	{
		VulkanDevice* pDevice = m_pContext->GetDevice();
		VulkanAllocation* pAllocation = new VulkanAllocation();

		auto type = GetMemoryType(requirements, memProps);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = type.first;
		VkResult vkres = pDevice->vkAllocateMemory(allocInfo, pAllocation->m_Memory);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to allocate vulkan memory (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		pAllocation->m_pContext = m_pContext;
		pAllocation->m_Offset = 0;
		pAllocation->m_Size = requirements.size;

		m_Allocations.push_back(pAllocation);

		// Update heap info
		pAllocation->m_HeapIndex = type.second.heapIndex;
		HeapInfo& heap = m_HeapInfo[pAllocation->m_HeapIndex];
		heap.usedSize += pAllocation->m_Size;
		
		return pAllocation;
	}

	void VulkanAllocator::Free(VulkanAllocation* pAllocation)
	{
		VulkanDevice* pDevice = m_pContext->GetDevice();

		auto it = std::find(m_Allocations.begin(), m_Allocations.end(), pAllocation);
		if (it == m_Allocations.end())
		{
			//g_Logger.LogWarning(LogVulkanRHI(), "Failed to free a vulkan allocation, allocation does not exist or is already freed!");
			return;
		}

		pDevice->vkFreeMemory(pAllocation->m_Memory);

		HeapInfo& heap = m_HeapInfo[pAllocation->m_HeapIndex];
		heap.usedSize += pAllocation->m_Size;

		delete pAllocation;

		m_Allocations.erase(it);
	}

	std::pair<u32, VkMemoryType> VulkanAllocator::GetMemoryType(const VkMemoryRequirements& requirements,
		VkMemoryPropertyFlags memProps)
	{
		for (u32 i = 0; i < m_MemProperties.memoryTypeCount; ++i)
		{
			if (requirements.memoryTypeBits & (1 << i) && (m_MemProperties.memoryTypes[i].propertyFlags & memProps) == memProps)
				return std::pair<u32, VkMemoryType>(i, m_MemProperties.memoryTypes[i]);
		}
		return std::pair<u32, VkMemoryType>(u32(-1), VkMemoryType());
	}


}
