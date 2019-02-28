
#include "VulkanTexture.h"
#include "VulkanDevice.h"
#include "VulkanHelpers.h"
#include "VulkanContext.h"

namespace Vulkan {


	VulkanTexture::VulkanTexture()
		: Texture()
		, m_Image(VK_NULL_HANDLE)
		, m_View(VK_NULL_HANDLE)
		, m_OwningQueueFamily(u32(-1))
		, m_pAllocation(nullptr)
		, m_pStagingBuffer(nullptr)
		, m_OwnsImage(false)
	{
	}

	VulkanTexture::~VulkanTexture()
	{
	}

	b8 VulkanTexture::Create(RHI::RHIContext* pContext, const RHI::TextureDesc& desc, RHI::CommandList* pCommandList)
	{
		m_pContext = pContext;
		m_Desc = desc;
		m_OwnsImage = true;

		if ((m_Desc.flags & RHI::TextureFlags::Dynamic) != RHI::TextureFlags::None)
		{
			m_Desc.layerCount = 1;
			m_Desc.mipLevels = 1;

			u8 formatSize = m_Desc.format.GetSize();
			if (formatSize == 0 || formatSize == 0xFF)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Can only use uncompressed data for dynamic textures!");
				return false;
			}
		}

		return CreateInternal(pCommandList);
	}

	b8 VulkanTexture::Create(RHI::RHIContext* pContext, VkImage image, VkImageLayout layout, u32 width, u32 height, PixelFormat format, RHI::SampleCount samples,
		RHI::TextureFlags flags)
	{
		m_pContext = pContext;
		m_Desc.width = width;
		m_Desc.height = height;
		m_Desc.format = format;
		m_Desc.samples = samples;
		m_Desc.flags = flags;
		m_Image = image;
		m_Desc.layout = Helpers::GetTextureLayout(layout);
		m_Desc.type = RHI::TextureType::Tex2D;

		return CreateInternal(nullptr);
	}

	b8 VulkanTexture::Destroy()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		if (m_View)
		{
			pDevice->vkDestroyImageView(m_View);
		}

		if (m_pStagingBuffer)
		{
			m_pStagingBuffer->Destroy();
			delete m_pStagingBuffer;
			m_pStagingBuffer = nullptr;
		}

		if (m_OwnsImage && m_Image)
		{
			if (m_pAllocation)
			{
				VulkanAllocator* pAllocator = ((VulkanContext*)m_pContext)->GetAllocator();
				pAllocator->Free(m_pAllocation);
			}

			pDevice->vkDestroyImage(m_Image);
		}

		return true;
	}

	b8 VulkanTexture::Write(const RHI::TextureRegion& region, u64 size, void* pData,
		RHI::CommandList* pCommandList)
	{
		if ((m_Desc.flags & RHI::TextureFlags::Dynamic) != RHI::TextureFlags::None)
		{
			void* pMappedData = m_pAllocation->Map(0, size, VulkanAllocationMapMode::Write);
			if (!pMappedData)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to map the vulkan buffer memory!");
				return false;
			}

			u8 formatSize = m_Desc.format.GetSize();
			u64 lineSize = m_Desc.width * formatSize;
			u64 layerSize = lineSize * m_Desc.height;
			u64 copySize = region.extent.x * formatSize;
			u64 zOffset = region.offset.z * layerSize;
			u64 yOffset = region.offset.y * lineSize;
			u64 xOffset = region.offset.x * formatSize;
			u64 bytesWritten = 0;
			
			for (u32 z = 0; z < region.extent.y && bytesWritten + lineSize < size; ++z)
			{
				u64 offset = zOffset + z * layerSize + yOffset + xOffset;
				for (u32 y = 0; y < region.extent.y && bytesWritten + lineSize > size; ++y)
				{
					if (bytesWritten + lineSize > size)
						copySize = size - bytesWritten;

					memcpy(pMappedData, (u8*)pData + offset, copySize);
					offset += lineSize;
					bytesWritten += lineSize;
				}
			}

			m_pAllocation->Unmap();

			return true;
		}
		else if ((m_Desc.flags & RHI::TextureFlags::Static) != RHI::TextureFlags::None)
		{
			VulkanBuffer* pStagingBuffer = new VulkanBuffer();
			b8 res = pStagingBuffer->Create(m_pContext, RHI::BufferType::Staging, size, RHI::BufferFlags::None);
			if (!res)
			{
				delete pStagingBuffer;
				return false;
			}
			res = pStagingBuffer->Write(0, size, pData);
			if (!res)
			{
				pStagingBuffer->Destroy();
				delete pStagingBuffer;
				return false;
			}
			RHI::TextureBufferCopyRegion copyRegion = {};
			copyRegion.texOffset = region.offset;
			copyRegion.texExtent = region.extent;
			copyRegion.mipLevel = region.mipLevel;
			copyRegion.baseArrayLayer = region.baseArrayLayer;
			copyRegion.layerCount = region.layerCount;
			res = Copy(pStagingBuffer, copyRegion);

			pStagingBuffer->Destroy();
			delete pStagingBuffer;

			return res;
		}
		else
		{
			b8 res = m_pStagingBuffer->Write(0, size, pData);
			if (!res)
				return res;
			RHI::TextureBufferCopyRegion copyRegion = {};
			copyRegion.texOffset = region.offset;
			copyRegion.texExtent = region.extent;
			copyRegion.mipLevel = region.mipLevel;
			copyRegion.baseArrayLayer = region.baseArrayLayer;
			copyRegion.layerCount = region.layerCount;
			return Copy(m_pStagingBuffer, copyRegion, pCommandList);
		}
	}

	b8 VulkanTexture::Read(const RHI::TextureRegion& region, u64 size, void* pData,
		RHI::CommandList* pCommandList)
	{
		if ((m_Desc.flags & RHI::TextureFlags::Dynamic) != RHI::TextureFlags::None)
		{
			void* pMappedData = m_pAllocation->Map(0, size, VulkanAllocationMapMode::Write);
			if (!pMappedData)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to map the vulkan buffer memory!");
				return false;
			}

			u8 formatSize = m_Desc.format.GetSize();
			u64 lineSize = m_Desc.width * formatSize;
			u64 layerSize = lineSize * m_Desc.height;
			u64 copySize = region.extent.x * formatSize;
			u64 zOffset = region.offset.z * layerSize;
			u64 yOffset = region.offset.y * lineSize;
			u64 xOffset = region.offset.x * formatSize;
			u64 bytesWritten = 0;

			for (u32 z = 0; z < region.extent.y && bytesWritten + lineSize < size; ++z)
			{
				u64 offset = zOffset + z * layerSize + yOffset + xOffset;
				for (u32 y = 0; y < region.extent.y && bytesWritten + lineSize > size; ++y)
				{
					if (bytesWritten + lineSize > size)
						copySize = size - bytesWritten;

					memcpy((u8*)pData, (u8*)pMappedData + offset, copySize);
					offset += lineSize;
					bytesWritten += lineSize;
				}
			}

			m_pAllocation->Unmap();
			return true;
		}
		else if ((m_Desc.flags & RHI::TextureFlags::Static) != RHI::TextureFlags::None)
		{
			VulkanBuffer* pStagingBuffer = new VulkanBuffer();
			b8 res = pStagingBuffer->Create(m_pContext, RHI::BufferType::Staging, size, RHI::BufferFlags::None);
			if (!res)
			{
				delete pStagingBuffer;
				return false;
			}
			RHI::TextureBufferCopyRegion copyRegion = {};
			copyRegion.texOffset = region.offset;
			copyRegion.texExtent = region.extent;
			copyRegion.mipLevel = region.mipLevel;
			copyRegion.baseArrayLayer = region.baseArrayLayer;
			copyRegion.layerCount = region.layerCount;
			res = Copy(pStagingBuffer, copyRegion);
			if (!res)
			{
				pStagingBuffer->Destroy();
				delete pStagingBuffer;
				return false;
			}

			res = pStagingBuffer->Read(0, size, pData);
			pStagingBuffer->Destroy();
			delete pStagingBuffer;

			return res;
		}
		else
		{
			RHI::TextureBufferCopyRegion copyRegion = {};
			copyRegion.texOffset = region.offset;
			copyRegion.texExtent = region.extent;
			copyRegion.mipLevel = region.mipLevel;
			copyRegion.baseArrayLayer = region.baseArrayLayer;
			copyRegion.layerCount = region.layerCount;
			b8 res = m_pStagingBuffer->Copy(this, copyRegion, pCommandList);
			if (!res)
				return res;
			
			return m_pStagingBuffer->Read(0, size, pData);
		}

	}

	b8 VulkanTexture::Copy(Texture* pTexture, const RHI::TextureCopyRegion& region,
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

		if (m_Desc.layout != RHI::TextureLayout::TransferDst)
		{
			RHI::TextureLayoutTransition transition = {};
			transition.layout = RHI::TextureLayout::TransferDst;
			transition.baseMipLevel = 0;
			transition.mipLevelCount = m_Desc.mipLevels;
			transition.baseArrayLayer = 0;
			transition.layerCount = m_Desc.layerCount;
			pCommandList->TransitionTextureLayout(RHI::PipelineStage::Transfer, RHI::PipelineStage::Transfer, this, transition);
		}
		if (pTexture->GetLayout() != RHI::TextureLayout::TransferSrc)
		{
			RHI::TextureLayoutTransition transition = {};
			transition.layout = RHI::TextureLayout::TransferDst;
			transition.baseMipLevel = 0;
			transition.mipLevelCount = m_Desc.mipLevels;
			transition.baseArrayLayer = 0;
			transition.layerCount = m_Desc.layerCount;
			pCommandList->TransitionTextureLayout(RHI::PipelineStage::Transfer, RHI::PipelineStage::Transfer, this, transition);
		}

		pCommandList->CopyTexture(pTexture, this, region);

		if (isSingleTimeCommands)
		{
			pCommandListManager->EndSingleTimeCommandList(pCommandList);
		}

		return true;
	}

	b8 VulkanTexture::Copy(RHI::Buffer* pBuffer, const RHI::TextureBufferCopyRegion& region,
		RHI::CommandList* pCommandList)
	{
		b8 isSingleTimeCommands = false;
		RHI::CommandListManager* pCommandListManager = ((VulkanContext*)m_pContext)->GetCommandListManager();

		if (pCommandList == nullptr)
		{
			RHI::Queue* pCopyQueue = m_pContext->GetQueue(RHI::QueueType::Transfer);
			pCommandList = pCommandListManager->CreateSingleTimeCommandList(pCopyQueue);
			isSingleTimeCommands = true;
		}

		if (m_Desc.layout != RHI::TextureLayout::TransferDst)
		{
			RHI::TextureLayoutTransition transition = {};
			transition.layout = RHI::TextureLayout::TransferDst;
			transition.baseMipLevel = 0;
			transition.mipLevelCount = m_Desc.mipLevels;
			transition.baseArrayLayer = 0;
			transition.layerCount = m_Desc.layerCount;
			pCommandList->TransitionTextureLayout(RHI::PipelineStage::Transfer, RHI::PipelineStage::Transfer, this, transition);
		}

		pCommandList->CopyBufferToTexture(pBuffer, this, region);

		// TODO: Should we transition back to the old layout?

		if (isSingleTimeCommands)
		{
			pCommandListManager->EndSingleTimeCommandList(pCommandList);
		}

		return true;
	}

	b8 VulkanTexture::CreateInternal(RHI::CommandList* pCommandList)
	{
		VkResult vkres;
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		if (m_OwnsImage)
		{
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.extent.width = m_Desc.width;
			imageInfo.extent.height = m_Desc.height;
			imageInfo.extent.depth = m_Desc.depth;
			imageInfo.arrayLayers = m_Desc.layerCount;
			imageInfo.mipLevels = m_Desc.mipLevels;
			imageInfo.imageType = Helpers::GetImageType(m_Desc.type);
			imageInfo.format = Helpers::GetFormat(m_Desc.format);
			imageInfo.tiling = ((m_Desc.flags & RHI::TextureFlags::Dynamic) != RHI::TextureFlags::None) ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
			imageInfo.samples = Helpers::GetSampleCount(m_Desc.samples);
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			// usage
			imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			if ((m_Desc.flags & RHI::TextureFlags::NoSampling) == RHI::TextureFlags::None)
				imageInfo.usage |=  VK_IMAGE_USAGE_SAMPLED_BIT;
			if ((m_Desc.flags & RHI::TextureFlags::Storage) != RHI::TextureFlags::None)
				imageInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
			if ((m_Desc.flags & RHI::TextureFlags::InputAttachment) != RHI::TextureFlags::None)
				imageInfo.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

			if ((m_Desc.flags & RHI::TextureFlags::Color) != RHI::TextureFlags::None)
				imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if ((m_Desc.flags & RHI::TextureFlags::DepthStencil) != RHI::TextureFlags::None)
				imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

			// flags
			// Always mutable?
			imageInfo.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
			if (m_Desc.type == RHI::TextureType::Cubemap || m_Desc.type == RHI::TextureType::CubemapArray)
				imageInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			if (m_Desc.type == RHI::TextureType::Tex2DArray)
				imageInfo.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
			// TODO: other flags

			vkres = pDevice->vkCreateImage(imageInfo, m_Image);
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create vulkan image (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return false;
			}

			VkMemoryRequirements memReqs;
			pDevice->vkGetImageMemoryRequirements(m_Image, memReqs);
			m_MemorySize = memReqs.size;

			VulkanAllocator* pAllocator = ((VulkanContext*)m_pContext)->GetAllocator();
			VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			if ((m_Desc.flags & RHI::TextureFlags::Dynamic) != RHI::TextureFlags::None)
				memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

			m_pAllocation = pAllocator->Allocate(memReqs, memProps);
			if (!m_pAllocation)
			{
				//g_Logger.LogError(LogVulkanRHI(), "Failed to allocate a vulkan allocation!");
				Destroy();
				return false;
			}

			vkres = pDevice->vkBindImageMemory(m_Image, m_pAllocation->GetMemory(), m_pAllocation->GetOffset());
			if (vkres != VK_SUCCESS)
			{
				//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to bind vulkan image memory (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
				return false;
			}

			if ((m_Desc.flags & RHI::TextureFlags::Static) == RHI::TextureFlags::None && (m_Desc.flags & RHI::TextureFlags::Dynamic) == RHI::TextureFlags::None && (m_Desc.flags & RHI::TextureFlags::RenderTargetable) == RHI::TextureFlags::None)
			{
				m_pStagingBuffer = new VulkanBuffer();
				b8 res = m_pStagingBuffer->Create(m_pContext, RHI::BufferType::Staging, memReqs.size, RHI::BufferFlags::None);
			}

			// Transition layout
			if (m_Desc.layout != RHI::TextureLayout::Unknown)
			{
				RHI::TextureLayoutTransition transition = {};
				transition.layout = m_Desc.layout;
				transition.baseArrayLayer = 0;
				transition.layerCount = m_Desc.layerCount;
				transition.baseMipLevel = 0;
				transition.mipLevelCount = m_Desc.mipLevels;
				m_Desc.layout = RHI::TextureLayout::Unknown;

				if (pCommandList)
				{
					pCommandList->TransitionTextureLayout(RHI::PipelineStage::Transfer, RHI::PipelineStage::Transfer, this, transition);
				}
				else
				{
					RHI::CommandListManager* pManager = m_pContext->GetCommandListManager();
					RHI::Queue* pQueue = m_pContext->GetQueue(RHI::QueueType::Transfer);
					pCommandList = pManager->CreateSingleTimeCommandList(pQueue);

					pCommandList->TransitionTextureLayout(RHI::PipelineStage::Transfer, RHI::PipelineStage::Transfer, this, transition);

					pManager->EndSingleTimeCommandList(pCommandList);
				}
			}
		}

		// Create image view
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.format = Helpers::GetFormat(m_Desc.format);
		viewInfo.viewType = Helpers::GetImageViewType(m_Desc.type);;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		VkImageAspectFlags aspect = m_Desc.format.HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Desc.format.HasStencilComponent())
			aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
		viewInfo.subresourceRange.aspectMask = aspect;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = m_Desc.layerCount;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = m_Desc.mipLevels;

		vkres = pDevice->vkCreateImageView(viewInfo, m_View);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create vulkan image view (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		return true;
	}
}
