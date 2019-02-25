#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanHelpers.h"

namespace Vulkan {

	VulkanBuffer::VulkanBuffer()
		: Buffer()
		, m_Buffer(VK_NULL_HANDLE)
		, m_pAllocation(nullptr)
		, m_pStagingBuffer(nullptr)
		, m_View(VK_NULL_HANDLE)
	{
	}

	VulkanBuffer::~VulkanBuffer()
	{
	}

	b8 VulkanBuffer::Create(RHI::RHIContext* pContext, RHI::BufferType type, u64 size,
		RHI::BufferFlags flags, PixelFormat format)
	{
		m_pContext = pContext;
		m_Type = type;
		m_Size = size;
		m_Flags = flags;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_Size;
		bufferInfo.usage = Helpers::GetBufferUsage(m_Type, m_Flags);
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Concurent?

		VkResult vkres = pDevice->vkCreateBuffer(bufferInfo, m_Buffer);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create a vulkan buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		VkMemoryRequirements memReqs = {};
		pDevice->vkGetBufferMemoryRequirements(m_Buffer, memReqs);
		m_MemorySize = memReqs.size;

		VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		if ((m_Flags & RHI::BufferFlags::Dynamic) != RHI::BufferFlags::None || m_Type == RHI::BufferType::Staging)
			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		VulkanAllocator* pAllocator = ((VulkanContext*)m_pContext)->GetAllocator();
		m_pAllocation = pAllocator->Allocate(memReqs, memProps);

		vkres = pDevice->vkBindBufferMemory(m_Buffer, m_pAllocation->GetMemory(), m_pAllocation->GetOffset());
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to bind memory to a vulkan buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		if (m_Type == RHI::BufferType::StorageTexel || m_Type == RHI::BufferType::UniformTexel)
		{
			m_TexelBufferFormat = format;
			// TODO Buffer view as seperate class (maybe texture view too)???
			VkBufferViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
			viewInfo.buffer = m_Buffer;
			viewInfo.offset = 0;
			viewInfo.range = m_Size;
			viewInfo.format = Helpers::GetFormat(format);

			vkres = pDevice->vkCreateBufferView(viewInfo, m_View);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to bind memory to a vulkan buffer view (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return vkres;
			}
		}

		if ((m_Flags & (RHI::BufferFlags::Dynamic | RHI::BufferFlags::Static)) == RHI::BufferFlags::None && m_Type != RHI::BufferType::Staging)
		{
			m_pStagingBuffer = new VulkanBuffer();
			b8 res = m_pStagingBuffer->Create(m_pContext, RHI::BufferType::Staging, m_Size, RHI::BufferFlags::None);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to create staging buffer for buffer");
				return false;
			}
		}

		return true;
	}

	b8 VulkanBuffer::CreateVertexBuffer(RHI::RHIContext* pContext, u32 vertexCount, u16 vertexSize,
		RHI::BufferFlags flags)
	{
		m_VertexCount = vertexCount;
		m_VertexSize = vertexSize;
		return Create(pContext, RHI::BufferType::Vertex, m_VertexCount * m_VertexSize, flags);
	}

	b8 VulkanBuffer::CreateIndexBuffer(RHI::RHIContext* pContext, u32 indexCount, RHI::IndexType indexType,
		RHI::BufferFlags flags)
	{
		m_IndexCount = indexCount;
		m_IndexType = indexType;
		u8 indexSize = m_IndexType == RHI::IndexType::UShort ? sizeof(u16) : sizeof(u32);
		return Create(pContext, RHI::BufferType::Index, m_IndexCount * indexSize, flags);
	}

	b8 VulkanBuffer::Destroy()
	{
		if (m_pStagingBuffer)
		{
			m_pStagingBuffer->Destroy();
			delete m_pStagingBuffer;
			m_pStagingBuffer = nullptr;
		}

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		if (m_View)
		{
			pDevice->vkDestroyBufferView(m_View);
			m_View = VK_NULL_HANDLE;
		}

		if (m_pAllocation)
		{
			VulkanAllocator* pAllocator = ((VulkanContext*)m_pContext)->GetAllocator();
			pAllocator->Free(m_pAllocation);
			m_pAllocation = nullptr;
		}

		if (m_Buffer)
		{
			pDevice->vkDestroyBuffer(m_Buffer);
			m_Buffer = VK_NULL_HANDLE;
		}

		return true;
	}

	b8 VulkanBuffer::Write(u64 offset, u64 size, void* pData, RHI::BufferWriteFlags flags,
		RHI::CommandList* pCommandList)
	{
		if ((m_Flags & RHI::BufferFlags::NoWrite) != RHI::BufferFlags::None)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Can't write to a buffer with the 'NoWrite' flag set!");
			return false;
		}

		if ((m_Flags & RHI::BufferFlags::Dynamic) != RHI::BufferFlags::None || m_Type == RHI::BufferType::Staging)
		{
			if (flags == RHI::BufferWriteFlags::Discard)
			{
				void* pMappedData = m_pAllocation->Map(0, m_pAllocation->GetSize(), VulkanAllocationMapMode::Write);
				if (!pMappedData)
				{
					//g_Logger.LogError(LogVulkanRHI(), "Failed to map the vulkan buffer memory!");
					return false;
				}
				memset(pMappedData, 0, m_pAllocation->GetSize());
				memcpy((u8*)pMappedData + offset, pData, size);
				m_pAllocation->Unmap();
			}
			else
			{
				void* pMappedData = m_pAllocation->Map(offset, size, VulkanAllocationMapMode::Write);
				if (!pMappedData)
				{
					//g_Logger.LogError(LogVulkanRHI(), "Failed to map the vulkan buffer memory!");
					return false;
				}
				memcpy(pMappedData, pData, size);
				m_pAllocation->Unmap();
			}
		}
		else if ((m_Flags & RHI::BufferFlags::Static) != RHI::BufferFlags::None)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

			// Create staging buffer
			VulkanBuffer* pStagingBuffer = new VulkanBuffer();
			b8 res = pStagingBuffer->Create(m_pContext, RHI::BufferType::Staging, size, RHI::BufferFlags::None);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to create temporary staging buffer for static buffer!");
				delete pStagingBuffer;
				return false;
			}
			res = pStagingBuffer->Write(0, size, pData);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to write to temporary staging buffer for static buffer!");
				pStagingBuffer->Destroy();
				delete pStagingBuffer;
				return false;
			}

			// Record copy and execute
			res = Copy(pStagingBuffer, 0, offset, size);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to copy temporary staging buffer to static buffer!");
			}

			pStagingBuffer->Destroy();
			delete pStagingBuffer;
			return res;
		}
		else
		{
			b8 res = m_pStagingBuffer->Write(0, size, pData, flags);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to write to staging buffer!");
				return false;
			}
			res = Copy(m_pStagingBuffer, 0, offset, size, pCommandList);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to copy staging buffer to buffer!");
				return false;
			}
		}

		return true;
	}

	b8 VulkanBuffer::Read(u64 offset, u64 size, void* pData, RHI::CommandList* pCommandList)
	{
		if ((m_Flags & RHI::BufferFlags::NoRead) != RHI::BufferFlags::None)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Can't read from a buffer with the 'NoRead' flag set!");
			return false;
		}

		if ((m_Flags & RHI::BufferFlags::Dynamic) != RHI::BufferFlags::None || m_Type == RHI::BufferType::Staging)
		{
			void* pMappedData = m_pAllocation->Map(offset, size, VulkanAllocationMapMode::Write);
			if (!pMappedData)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to map the vulkan buffer memory!");
				return false;
			}
			memcpy(pData, pMappedData, size);
			m_pAllocation->Unmap();
		}
		else if ((m_Flags & RHI::BufferFlags::Static) != RHI::BufferFlags::None)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

			// Create staging buffer
			VulkanBuffer* pStagingBuffer = new VulkanBuffer();
			b8 res = pStagingBuffer->Create(m_pContext, RHI::BufferType::Staging, size, RHI::BufferFlags::None);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to create temporary staging buffer for static buffer!");
				pStagingBuffer->Destroy();
				delete pStagingBuffer;
				return false;
			}

			res = pStagingBuffer->Copy(this, offset, 0, size); //Copy(pStagingBuffer, 0, offset, size, nullptr);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to copy temporary staging buffer to static buffer!");
				pStagingBuffer->Destroy();
				delete pStagingBuffer;
				return false;
			}

			res = pStagingBuffer->Read(0, size, pData);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to write to temporary staging buffer for static buffer!");
			}

			pStagingBuffer->Destroy();
			delete pStagingBuffer;
			return res;
		}
		else
		{
			b8 res = Copy(m_pStagingBuffer, 0, offset, size, pCommandList);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to copy staging buffer to buffer!");
				return false;
			}
			res = m_pStagingBuffer->Read(0, size, pData);
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to write to staging buffer!");
				return false;
			}
		}

		return true;
	}

	b8 VulkanBuffer::Copy(Buffer* pBuffer, u64 srcOffset, u64 dstOffset, u64 size,
		RHI::CommandList* pCommandList)
	{
		if (pCommandList)
		{
			pCommandList->CopyBuffer(pBuffer, srcOffset, this, dstOffset, size);
			return true;
		}

		RHI::CommandListManager* pCommandListManager = ((VulkanContext*)m_pContext)->GetCommandListManager();
		RHI::Queue* pCopyQueue = m_pContext->GetQueue(RHI::QueueType::Transfer);
		RHI::CommandList* pCopyCommandList = pCommandListManager->CreateSingleTimeCommandList(pCopyQueue);

		// Record copy and execute and cleanup
		pCopyCommandList->CopyBuffer(pBuffer, srcOffset, this, dstOffset, size);
		b8 res = pCommandListManager->EndSingleTimeCommandList(pCopyCommandList);

		return res;
	}

	b8 VulkanBuffer::Copy(Buffer* pBuffer, const std::vector<RHI::BufferCopyRegion>& regions,
		RHI::CommandList* pCommandList)
	{
		if (pCommandList)
		{
			pCommandList->CopyBuffer(pBuffer, this, regions);
			return true;
		}

		RHI::CommandListManager* pCommandListManager = ((VulkanContext*)m_pContext)->GetCommandListManager();
		RHI::Queue* pCopyQueue = m_pContext->GetQueue(RHI::QueueType::Transfer);
		RHI::CommandList* pCopyCommandList = pCommandListManager->CreateCommandList(pCopyQueue);

		// Record copy and execute
		b8 res = pCopyCommandList->Begin();
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to start temporary copy command buffer for static buffer!");
		}
		else
		{
			pCopyCommandList->CopyBuffer(pBuffer, this, regions);
			res = pCopyCommandList->End();
			if (!res)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to end temporary copy command buffer for static buffer!");
			}
			else
			{
				res = pCopyCommandList->Submit();
				if (!res)
				{
					//g_Logger.LogError(LogVulkanRHI(), "Failed to submit temporary copy command buffer for static buffer!");
				}
				else
				{
					res = pCopyCommandList->Wait();
					if (!res)
					{
						//g_Logger.LogError(LogVulkanRHI(), "Failed to wait for temporary copy command buffer for static buffer!");
					}
				}
			}
		}
		// Cleanup
		pCommandListManager->DestroyCommandList(pCopyCommandList);
		return res;
	}

	b8 VulkanBuffer::Copy(RHI::Texture* pTexture, const RHI::TextureBufferCopyRegion& region,
		RHI::CommandList* pCommandList)
	{
		b8 isSingleTimeCommands = false;
		RHI::CommandListManager* pCommandListManager = ((VulkanContext*)m_pContext)->GetCommandListManager();

		if (pCommandList == nullptr)
		{
			RHI::Queue* pCopyQueue = m_pContext->GetQueue(RHI::QueueType::Transfer);
			pCommandList = pCommandListManager->CreateCommandList(pCopyQueue);
			isSingleTimeCommands = true;
		}

		if (pTexture->GetLayout() != RHI::TextureLayout::TransferSrc)
		{
			RHI::TextureLayoutTransition transition = {};
			transition.layout = RHI::TextureLayout::TransferDst;
			transition.baseMipLevel = 0;
			transition.mipLevelCount = pTexture->GetMipLevels();
			transition.baseArrayLayer = 0;
			transition.layerCount = pTexture->GetLayerCount();
			pCommandList->TransitionTextureLayout(RHI::PipelineStage::Transfer, RHI::PipelineStage::Transfer, pTexture, transition);
		}

		pCommandList->CopyTextureToBuffer(pTexture, this, region);

		if (isSingleTimeCommands)
		{
			pCommandListManager->EndSingleTimeCommandList(pCommandList);
		}

		return true;
	}

	b8 VulkanBuffer::Copy(RHI::Texture* pTexture, const std::vector<RHI::TextureBufferCopyRegion>& regions,
		RHI::CommandList* pCommandList)
	{
		return false;
	}
}
