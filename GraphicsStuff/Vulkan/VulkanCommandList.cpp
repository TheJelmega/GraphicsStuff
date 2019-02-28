#pragma once

#define CHECK_RECORDING\
	if (m_Status != RHI::CommandListState::Recording)\
	{\
		/*//g_Logger.LogWarning(LogVulkanRHI(), "Can't record to command list that isn't in the recording state");*/\
		return;\
	}
#include "VulkanCommandList.h"
#include "VulkanPipeline.h"
#include "VulkanHelpers.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "../RHI/ClearValue.h"
#include "../RHI/RenderTarget.h"
#include "VulkanBuffer.h"
#include "VulkanQueue.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

namespace Vulkan {

	VulkanCommandList::VulkanCommandList()
		: m_CommandBuffer(VK_NULL_HANDLE)
		, m_SrcStage(RHI::PipelineStage::None)
		, m_DstStage(RHI::PipelineStage::None)
	{
	}

	VulkanCommandList::~VulkanCommandList()
	{
	}

	b8 VulkanCommandList::Begin()
	{
		if (m_Status != RHI::CommandListState::Reset && m_Status != RHI::CommandListState::Finished)
		{
			//g_Logger.LogWarning(LogVulkanRHI(), "Failed to begin the command list, command list is still recording or in use!");
			return false;
		}

		// TODO: other flags for command buffer begin ?
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VkResult vkres = vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to begin the vulkan command buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		b8 res = m_pFence->Reset();
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to reset the command list's fence");
			return false;
		}

		m_BoundInputSlots.clear();

		m_Status = RHI::CommandListState::Recording;
		return true;
	}

	b8 VulkanCommandList::End()
	{
		UpdateBarriers();

		if (m_Status != RHI::CommandListState::Recording)
		{
			//g_Logger.LogWarning(LogVulkanRHI(), "Failed to end the command list, command list is not in a recording state!");
			return false;
		}

		if (m_pRenderPass)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to end the command list, command list is still inside renderpass!");
			return false;
		}

		VkResult vkres = vkEndCommandBuffer(m_CommandBuffer);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to end the vulkan command buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		m_Status = RHI::CommandListState::Recorded;
		return true;
	}

	void VulkanCommandList::BindPipeline(RHI::Pipeline* pPipeline)
	{
		CHECK_RECORDING;
		m_pPipeline = pPipeline;

		VulkanPipeline* pVulkanPipeline = (VulkanPipeline*)m_pPipeline;
		VkPipelineBindPoint bindPoint = Helpers::GetPipelineBindPoint(pPipeline->GetType());

		vkCmdBindPipeline(m_CommandBuffer, bindPoint, pVulkanPipeline->GetPipeline());
	}

	void VulkanCommandList::BeginRenderPass(RHI::RenderPass* pRenderPass, RHI::Framebuffer* pFramebuffer)
	{
		CHECK_RECORDING;
		m_pRenderPass = pRenderPass;
		m_pFramebuffer = pFramebuffer;

		VulkanRenderPass* pVulkanRenderPass = (VulkanRenderPass*)m_pRenderPass;
		VulkanFramebuffer* pVulkanFramebuffer = (VulkanFramebuffer*)m_pFramebuffer;

		const std::vector<RHI::RenderTarget*>& renderTargets = pVulkanFramebuffer->GetRenderTargets();
		std::vector<VkClearValue> clearValues;

		for (const RHI::RenderTarget* pRT : renderTargets)
		{
			if (pRT->GetType() == RHI::RenderTargetType::DepthStencil)
			{
				VkClearValue vkValue;
				RHI::ClearValue value = pRT->GetClearValue();

				vkValue.depthStencil.depth = value.depth;
				vkValue.depthStencil.stencil = value.stencil;
				vkValue.color.float32[0] = value.depth;
				clearValues.push_back(vkValue);
			}
			else
			{
				VkClearValue vkValue;
				RHI::ClearValue value = pRT->GetClearValue();

				vkValue.color.float32[0] = value.color[0];
				vkValue.color.float32[1] = value.color[1];
				vkValue.color.float32[2] = value.color[2];
				vkValue.color.float32[3] = value.color[3];
				clearValues.push_back(vkValue);
			}
		}

		VkRenderPassBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = pVulkanRenderPass->GetRenderPass();
		beginInfo.framebuffer = pVulkanFramebuffer->GetFrameBuffer();
		beginInfo.clearValueCount = u32(clearValues.size());
		beginInfo.pClearValues = clearValues.data();
		beginInfo.renderArea.extent.width = m_pFramebuffer->GetWidth();
		beginInfo.renderArea.extent.height = m_pFramebuffer->GetHeight();

		// TODO: Currently disallow secondary buffer, figure it out when implementing DX12
		vkCmdBeginRenderPass(m_CommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandList::EndRenderPass()
	{
		CHECK_RECORDING;
		UpdateBarriers();

		if (m_pRenderPass)
		{
			vkCmdEndRenderPass(m_CommandBuffer);
			m_pRenderPass = nullptr;
		}
	}

	void VulkanCommandList::BindVertexBuffer(u16 inputSlot, RHI::Buffer* pBuffer, u64 offset)
	{
		CHECK_RECORDING;
		assert(pBuffer->GetType() == RHI::BufferType::Vertex || pBuffer->GetType() == RHI::BufferType::Default);
		UpdateBarriers();

		m_BoundInputSlots.push_back(inputSlot);
		VkBuffer buffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		vkCmdBindVertexBuffers(m_CommandBuffer, inputSlot, 1, &buffer, &offset);
	}

	void VulkanCommandList::BindVertexBuffer(u16 inputSlot, const std::vector<RHI::Buffer*>& buffers, const std::vector<u64>& offsets)
	{
		CHECK_RECORDING;
		assert(buffers.size() == offsets.size());
		UpdateBarriers();

		std::vector<VkBuffer> vkBuffers;
		vkBuffers.reserve(buffers.size());
		u16 slot = inputSlot;
		for (RHI::Buffer* pBuffer : buffers)
		{
			assert(pBuffer->GetType() == RHI::BufferType::Vertex || pBuffer->GetType() == RHI::BufferType::Default);
			m_BoundInputSlots.push_back(slot++);
			vkBuffers.push_back(((VulkanBuffer*)pBuffer)->GetBuffer());
		}
		vkCmdBindVertexBuffers(m_CommandBuffer, inputSlot, u32(vkBuffers.size()), vkBuffers.data(), offsets.data());
	}

	void VulkanCommandList::BindIndexBuffer(RHI::Buffer* pBuffer, u64 offset)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		assert(pBuffer->GetType() == RHI::BufferType::Index);
		VkBuffer buffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		vkCmdBindIndexBuffer(m_CommandBuffer, buffer, offset, Helpers::GetIndexType(pBuffer->GetIndexType()));
	}

	void VulkanCommandList::BindIndexBuffer(RHI::Buffer* pBuffer, u64 offset, RHI::IndexType type)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		assert(pBuffer->GetType() == RHI::BufferType::Index || pBuffer->GetType() == RHI::BufferType::Default);
		VkBuffer buffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		vkCmdBindIndexBuffer(m_CommandBuffer, buffer, offset, Helpers::GetIndexType(type));
	}

	void VulkanCommandList::BindDescriptorSets(u32 firstSet, RHI::DescriptorSet* pSet)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		assert(m_pPipeline);
		VkPipelineBindPoint bindpoint = Helpers::GetPipelineBindPoint(m_pPipeline->GetType());
		VkPipelineLayout layout = ((VulkanPipeline*)m_pPipeline)->GetLayout();
		VkDescriptorSet vulkanSet = ((VulkanDescriptorSet*)pSet)->GetDescriptorSet();

		vkCmdBindDescriptorSets(m_CommandBuffer, bindpoint, layout, firstSet, 1, &vulkanSet, 0, nullptr);
	}

	void VulkanCommandList::BindDescriptorSets(u32 firstSet, RHI::DescriptorSet* pSet, u32 dynamicOffset)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		assert(m_pPipeline);
		VkPipelineBindPoint bindpoint = Helpers::GetPipelineBindPoint(m_pPipeline->GetType());
		VkPipelineLayout layout = ((VulkanPipeline*)m_pPipeline)->GetLayout();
		VkDescriptorSet vulkanSet = ((VulkanDescriptorSet*)pSet)->GetDescriptorSet();

		vkCmdBindDescriptorSets(m_CommandBuffer, bindpoint, layout, firstSet, 1, &vulkanSet, 1, &dynamicOffset);
	}

	void VulkanCommandList::BindDescriptorSets(u32 firstSet, const std::vector<RHI::DescriptorSet*>& sets)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		assert(m_pPipeline);
		VkPipelineBindPoint bindpoint = Helpers::GetPipelineBindPoint(m_pPipeline->GetType());
		VkPipelineLayout layout = ((VulkanPipeline*)m_pPipeline)->GetLayout();

		std::vector<VkDescriptorSet> vulkanSets;
		vulkanSets.reserve(sets.size());
		for (const RHI::DescriptorSet* pSet : sets)
		{
			vulkanSets.push_back(((VulkanDescriptorSet*)pSet)->GetDescriptorSet());
		}

		vkCmdBindDescriptorSets(m_CommandBuffer, bindpoint, layout, firstSet, u32(vulkanSets.size()), vulkanSets.data(), 0, nullptr);
	}

	void VulkanCommandList::BindDescriptorSets(u32 firstSet, const std::vector<RHI::DescriptorSet*>& sets,
		const std::vector<u32>& dynamicOffsets)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		assert(m_pPipeline);
		VkPipelineBindPoint bindpoint = Helpers::GetPipelineBindPoint(m_pPipeline->GetType());
		VkPipelineLayout layout = ((VulkanPipeline*)m_pPipeline)->GetLayout();

		std::vector<VkDescriptorSet> vulkanSets;
		vulkanSets.reserve(sets.size());
		for (const RHI::DescriptorSet* pSet : sets)
		{
			vulkanSets.push_back(((VulkanDescriptorSet*)pSet)->GetDescriptorSet());
		}

		vkCmdBindDescriptorSets(m_CommandBuffer, bindpoint, layout, firstSet, u32(vulkanSets.size()), vulkanSets.data(), u32(dynamicOffsets.size()), dynamicOffsets.data());
	}

	void VulkanCommandList::SetViewport(RHI::Viewport& viewport)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		VkViewport vp;
		vp.x = viewport.x;
		vp.y = viewport.y;
		vp.width = viewport.width;
		vp.height = viewport.height;
		vp.minDepth = viewport.minDepth;
		vp.maxDepth = viewport.maxDepth;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &vp);
	}

	void VulkanCommandList::SetScissor(RHI::ScissorRect& scissor)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		VkRect2D rect;
		rect.offset.x = scissor.x;
		rect.offset.y = scissor.y;
		rect.extent.width = scissor.width;
		rect.extent.height = scissor.height;
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &rect);
	}

	void VulkanCommandList::Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void VulkanCommandList::DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset,
		u32 firstInstance)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void VulkanCommandList::CopyBuffer(RHI::Buffer* pSrcBuffer, u64 srcOffset,
		RHI::Buffer* pDstBuffer, u64 dstOffset, u64 size)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		VkBuffer vkSrcBuffer = ((VulkanBuffer*)pSrcBuffer)->GetBuffer();
		VkBuffer vkDstBuffer = ((VulkanBuffer*)pDstBuffer)->GetBuffer();
		VkBufferCopy region = {};
		region.srcOffset = srcOffset;
		region.dstOffset = dstOffset;
		region.size = size;

		vkCmdCopyBuffer(m_CommandBuffer, vkSrcBuffer, vkDstBuffer, 1, &region);
	}

	void VulkanCommandList::CopyBuffer(RHI::Buffer* pSrcBuffer, RHI::Buffer* pDstBuffer,
		const std::vector<RHI::BufferCopyRegion>& regions)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		std::vector<VkBufferCopy> copies;
		copies.reserve(regions.size());

		for (const RHI::BufferCopyRegion& region : regions)
		{
			VkBufferCopy copy = {};
			copy.srcOffset = region.srcOffset;
			copy.dstOffset = region.dstOffset;
			copy.size = region.size;
			copies.push_back(copy);
		}

		VkBuffer vkSrcBuffer = ((VulkanBuffer*)pSrcBuffer)->GetBuffer();
		VkBuffer vkDstBuffer = ((VulkanBuffer*)pDstBuffer)->GetBuffer();
		vkCmdCopyBuffer(m_CommandBuffer, vkSrcBuffer, vkDstBuffer, u32(copies.size()), copies.data());
	}

	void VulkanCommandList::CopyTexture(RHI::Texture* pSrcTex, RHI::Texture* pDstTex,
		const RHI::TextureCopyRegion& region)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		VkImageCopy copyRegion = {};
		copyRegion.extent.width = region.extent.x;
		copyRegion.extent.height = region.extent.y;
		copyRegion.extent.depth = region.extent.z;

		copyRegion.srcOffset.x = region.srcOffset.x;
		copyRegion.srcOffset.y = region.srcOffset.y;
		copyRegion.srcOffset.z = region.srcOffset.z;

		VkImageAspectFlags aspect = pSrcTex->GetFormat().HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (pSrcTex->GetFormat().HasStencilComponent())
			aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
		copyRegion.srcSubresource.aspectMask = aspect;
		copyRegion.srcSubresource.mipLevel = region.srcMipLevel;
		copyRegion.srcSubresource.baseArrayLayer = region.srcBaseArrayLayer;
		copyRegion.srcSubresource.layerCount = region.layerCount;

		copyRegion.dstOffset.x = region.dstOffset.x;
		copyRegion.dstOffset.y = region.dstOffset.y;
		copyRegion.dstOffset.z = region.dstOffset.z;

		aspect = pDstTex->GetFormat().HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (pDstTex->GetFormat().HasStencilComponent())
			aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
		copyRegion.dstSubresource.aspectMask = aspect;
		copyRegion.dstSubresource.mipLevel = region.dstMipLevel;
		copyRegion.dstSubresource.baseArrayLayer = region.dstBaseArrayLayer;
		copyRegion.dstSubresource.layerCount = region.layerCount;

		VkImage srcImage = ((VulkanTexture*)pSrcTex)->GetImage();
		VkImageLayout srcLayout = Helpers::GetImageLayout(pSrcTex->GetLayout());
		VkImage dstImage = ((VulkanTexture*)pDstTex)->GetImage();
		VkImageLayout dstLayout = Helpers::GetImageLayout(pDstTex->GetLayout());
		vkCmdCopyImage(m_CommandBuffer, srcImage, srcLayout, dstImage, dstLayout, 1, &copyRegion);
	}

	void VulkanCommandList::CopyTexture(RHI::Texture* pSrcTex, RHI::Texture* pDstTex,
		const std::vector<RHI::TextureCopyRegion>& regions)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		std::vector<VkImageCopy> copyRegions;
		copyRegions.reserve(regions.size());

		for (const RHI::TextureCopyRegion& region : regions)
		{
			VkImageCopy copyRegion = {};
			copyRegion.extent.width = region.extent.x;
			copyRegion.extent.height = region.extent.y;
			copyRegion.extent.depth = region.extent.z;

			copyRegion.srcOffset.x = region.srcOffset.x;
			copyRegion.srcOffset.y = region.srcOffset.y;
			copyRegion.srcOffset.z = region.srcOffset.z;

			VkImageAspectFlags aspect = pSrcTex->GetFormat().HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			if (pSrcTex->GetFormat().HasStencilComponent())
				aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
			copyRegion.srcSubresource.aspectMask = aspect;
			copyRegion.srcSubresource.mipLevel = region.srcMipLevel;
			copyRegion.srcSubresource.baseArrayLayer = region.srcBaseArrayLayer;
			copyRegion.srcSubresource.layerCount = region.layerCount;

			copyRegion.dstOffset.x = region.dstOffset.x;
			copyRegion.dstOffset.y = region.dstOffset.y;
			copyRegion.dstOffset.z = region.dstOffset.z;

			aspect = pDstTex->GetFormat().HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			if (pDstTex->GetFormat().HasStencilComponent())
				aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
			copyRegion.dstSubresource.aspectMask = aspect;
			copyRegion.dstSubresource.mipLevel = region.dstMipLevel;
			copyRegion.dstSubresource.baseArrayLayer = region.dstBaseArrayLayer;
			copyRegion.dstSubresource.layerCount = region.layerCount;

			copyRegions.push_back(copyRegion);
		}

		VkImage srcImage = ((VulkanTexture*)pSrcTex)->GetImage();
		VkImageLayout srcLayout = Helpers::GetImageLayout(pSrcTex->GetLayout());
		VkImage dstImage = ((VulkanTexture*)pDstTex)->GetImage();
		VkImageLayout dstLayout = Helpers::GetImageLayout(pDstTex->GetLayout());
		vkCmdCopyImage(m_CommandBuffer, srcImage, srcLayout, dstImage, dstLayout, u32(copyRegions.size()), copyRegions.data());
	}

	void VulkanCommandList::CopyBufferToTexture(RHI::Buffer* pBuffer, RHI::Texture* pTexture,
		const RHI::TextureBufferCopyRegion& region)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = region.bufferOffset;
		copyRegion.bufferRowLength = region.bufferRowLength;
		copyRegion.bufferImageHeight = region.bufferImageHeight;
		copyRegion.imageExtent.width = region.texExtent.x;
		copyRegion.imageExtent.height = region.texExtent.y;
		copyRegion.imageExtent.depth = region.texExtent.z;
		copyRegion.imageOffset.x = i32(region.texOffset.x);
		copyRegion.imageOffset.y = i32(region.texOffset.y);
		copyRegion.imageOffset.z = i32(region.texOffset.z);

		PixelFormat format = pTexture->GetFormat();
		VkImageAspectFlags aspect = format.HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (format.HasStencilComponent())
			aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

		copyRegion.imageSubresource.aspectMask = aspect;
		copyRegion.imageSubresource.mipLevel = region.mipLevel;
		copyRegion.imageSubresource.baseArrayLayer = region.baseArrayLayer;
		copyRegion.imageSubresource.layerCount = region.layerCount;

		VkBuffer vkBuffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		VkImage vkImage = ((VulkanTexture*)pTexture)->GetImage();
		VkImageLayout imageLayout = Helpers::GetImageLayout(pTexture->GetLayout());

		vkCmdCopyBufferToImage(m_CommandBuffer, vkBuffer, vkImage, imageLayout, 1, &copyRegion);
	}

	void VulkanCommandList::CopyBufferToTexture(RHI::Buffer* pBuffer, RHI::Texture* pTexture,
		const std::vector<RHI::TextureBufferCopyRegion>& regions)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		std::vector<VkBufferImageCopy> copyRegions;
		copyRegions.reserve(regions.size());

		for (const RHI::TextureBufferCopyRegion& region : regions)
		{
			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = region.bufferOffset;
			copyRegion.bufferRowLength = region.bufferRowLength;
			copyRegion.bufferImageHeight = region.bufferImageHeight;
			copyRegion.imageExtent.width = region.texExtent.x;
			copyRegion.imageExtent.height = region.texExtent.y;
			copyRegion.imageExtent.depth = region.texExtent.z;
			copyRegion.imageOffset.x = i32(region.texOffset.x);
			copyRegion.imageOffset.y = i32(region.texOffset.y);
			copyRegion.imageOffset.z = i32(region.texOffset.z);

			PixelFormat format = pTexture->GetFormat();
			VkImageAspectFlags aspect = format.HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			if (format.HasStencilComponent())
				aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

			copyRegion.imageSubresource.aspectMask = aspect;
			copyRegion.imageSubresource.mipLevel = region.mipLevel;
			copyRegion.imageSubresource.baseArrayLayer = region.baseArrayLayer;
			copyRegion.imageSubresource.layerCount = region.layerCount;

			copyRegions.push_back(copyRegion);
		}

		VkBuffer vkBuffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		VkImage vkImage = ((VulkanTexture*)pTexture)->GetImage();
		VkImageLayout imageLayout = Helpers::GetImageLayout(pTexture->GetLayout());

		vkCmdCopyBufferToImage(m_CommandBuffer, vkBuffer, vkImage, imageLayout, u32(copyRegions.size()), copyRegions.data());
	}

	void VulkanCommandList::CopyTextureToBuffer(RHI::Texture* pTexture, RHI::Buffer* pBuffer,
		const RHI::TextureBufferCopyRegion& region)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = region.bufferOffset;
		copyRegion.bufferRowLength = region.bufferRowLength;
		copyRegion.bufferImageHeight = region.bufferImageHeight;
		copyRegion.imageExtent.width = region.texExtent.x;
		copyRegion.imageExtent.height = region.texExtent.y;
		copyRegion.imageExtent.depth = region.texExtent.z;
		copyRegion.imageOffset.x = i32(region.texOffset.x);
		copyRegion.imageOffset.y = i32(region.texOffset.y);
		copyRegion.imageOffset.z = i32(region.texOffset.z);

		PixelFormat format = pTexture->GetFormat();
		VkImageAspectFlags aspect = format.HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (format.HasStencilComponent())
			aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

		copyRegion.imageSubresource.aspectMask = aspect;
		copyRegion.imageSubresource.mipLevel = region.mipLevel;
		copyRegion.imageSubresource.baseArrayLayer = region.baseArrayLayer;
		copyRegion.imageSubresource.layerCount = region.layerCount;

		VkBuffer vkBuffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		VkImage vkImage = ((VulkanTexture*)pTexture)->GetImage();
		VkImageLayout imageLayout = Helpers::GetImageLayout(pTexture->GetLayout());

		vkCmdCopyImageToBuffer(m_CommandBuffer, vkImage, imageLayout, vkBuffer, 1, &copyRegion);
	}

	void VulkanCommandList::CopyTextureToBuffer(RHI::Texture* pTexture, RHI::Buffer* pBuffer,
		const std::vector<RHI::TextureBufferCopyRegion>& regions)
	{
		CHECK_RECORDING;
		UpdateBarriers();

		std::vector<VkBufferImageCopy> copyRegions;
		copyRegions.reserve(regions.size());

		for (const RHI::TextureBufferCopyRegion& region : regions)
		{
			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = region.bufferOffset;
			copyRegion.bufferRowLength = region.bufferRowLength;
			copyRegion.bufferImageHeight = region.bufferImageHeight;
			copyRegion.imageExtent.width = region.texExtent.x;
			copyRegion.imageExtent.height = region.texExtent.y;
			copyRegion.imageExtent.depth = region.texExtent.z;
			copyRegion.imageOffset.x = i32(region.texOffset.x);
			copyRegion.imageOffset.y = i32(region.texOffset.y);
			copyRegion.imageOffset.z = i32(region.texOffset.z);

			PixelFormat format = pTexture->GetFormat();
			VkImageAspectFlags aspect = format.HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			if (format.HasStencilComponent())
				aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

			copyRegion.imageSubresource.aspectMask = aspect;
			copyRegion.imageSubresource.mipLevel = region.mipLevel;
			copyRegion.imageSubresource.baseArrayLayer = region.baseArrayLayer;
			copyRegion.imageSubresource.layerCount = region.layerCount;

			copyRegions.push_back(copyRegion);
		}

		VkBuffer vkBuffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		VkImage vkImage = ((VulkanTexture*)pTexture)->GetImage();
		VkImageLayout imageLayout = Helpers::GetImageLayout(pTexture->GetLayout());

		vkCmdCopyImageToBuffer(m_CommandBuffer, vkImage, imageLayout, vkBuffer, u32(copyRegions.size()), copyRegions.data());
	}

	void VulkanCommandList::TransitionTextureLayout(RHI::PipelineStage srcStage, RHI::PipelineStage dstStage, RHI::Texture* pTexture,
		const RHI::TextureLayoutTransition& transition)
	{
		CHECK_RECORDING;
		if (pTexture->GetLayout() == transition.layout)
			return;

		if (srcStage != m_SrcStage || dstStage != m_DstStage)
		{
			UpdateBarriers();
		}

		m_SrcStage = srcStage;
		m_DstStage = dstStage;

		VkImageMemoryBarrier imageBarrier = {};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.srcAccessMask = Helpers::GetImageTransitionAccessMode(srcStage, pTexture->GetLayout(), true);
		imageBarrier.dstAccessMask = Helpers::GetImageTransitionAccessMode(dstStage, transition.layout, false);
		imageBarrier.oldLayout = Helpers::GetImageLayout(pTexture->GetLayout());
		imageBarrier.newLayout = Helpers::GetImageLayout(transition.layout);
		imageBarrier.image = ((VulkanTexture*)pTexture)->GetImage();

		VkImageAspectFlags aspect = pTexture->GetFormat().HasDepthComponent() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (pTexture->GetFormat().HasStencilComponent())
			aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
		imageBarrier.subresourceRange.aspectMask = aspect;
		imageBarrier.subresourceRange.baseArrayLayer = transition.baseArrayLayer;
		imageBarrier.subresourceRange.layerCount = transition.layerCount;
		imageBarrier.subresourceRange.baseMipLevel = transition.baseMipLevel;
		imageBarrier.subresourceRange.levelCount = transition.mipLevelCount;

		u32 owningQueueFamily = ((VulkanTexture*)pTexture)->GetOwningQueueFamily();
		u32 queueFamily = ((VulkanQueue*)m_pQueue)->GetQueueFamily();
		imageBarrier.srcQueueFamilyIndex = owningQueueFamily != u32(-1) ? owningQueueFamily : queueFamily;
		imageBarrier.dstQueueFamilyIndex = queueFamily;

		m_ImageBarriers.push_back(imageBarrier);

		pTexture->SetLayout(transition.layout);
		((VulkanTexture*)pTexture)->SetOwningQueueFamily(queueFamily);
	}

	b8 VulkanCommandList::Submit()
	{
		VulkanQueue* pVulkanQueue = (VulkanQueue*)m_pQueue;
		VkQueue queue = pVulkanQueue->GetQueue();
		VkFence fence = ((VulkanFence*)m_pFence)->GetFence();

#if 1
		// Checks before submitting

		// Check data for pipeline
		if (m_pPipeline)
		{
			if (m_pPipeline->GetType() == RHI::PipelineType::Graphics)
			{
				const RHI::GraphicsPipelineDesc& desc = m_pPipeline->GetGraphicsDesc();
				const std::vector<u16> inputSlots = desc.inputDescriptor.GetInputSlots();
				for (u16 slot : inputSlots)
				{
					if (std::find(m_BoundInputSlots.begin(), m_BoundInputSlots.end(), slot) == m_BoundInputSlots.end())
					{
						//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "No vertex buffer bound to input slot %u!", slot);
						return false;
					}
				}
			}
		}
#endif

		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &m_CommandBuffer;

		VkResult vkres = pVulkanQueue->vkSubmit(info, fence);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to submit the vulkan command buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}

		m_Status = RHI::CommandListState::Submited;
		m_pFence->SetSubmitted();

		return true;
	}

	b8 VulkanCommandList::Submit(const std::vector<RHI::Semaphore*>& waitSemaphores, 
		const std::vector<RHI::PipelineStage>& waitStages,
		const std::vector<RHI::Semaphore*>& signalSemaphores)
	{
		VulkanQueue* pVulkanQueue = (VulkanQueue*)m_pQueue;
		VkQueue queue = pVulkanQueue->GetQueue();
		VkFence fence = ((VulkanFence*)m_pFence)->GetFence();

		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &m_CommandBuffer;

		std::vector<VkSemaphore> vulkanWaitSemaphores;
		std::vector<VkPipelineStageFlags> vulkanWaitStages;
		vulkanWaitSemaphores.reserve(waitSemaphores.size());
		vulkanWaitStages.reserve(waitSemaphores.size());
		for (u32 i = 0; i < waitSemaphores.size(); ++i)
		{
			vulkanWaitSemaphores.push_back(((VulkanSemaphore*)waitSemaphores[i])->GetSemaphore());
			vulkanWaitStages.push_back(Helpers::GetPipelineStage(waitStages[i]));
		}
		info.waitSemaphoreCount = u32(vulkanWaitSemaphores.size());
		info.pWaitSemaphores = vulkanWaitSemaphores.data();
		info.pWaitDstStageMask = vulkanWaitStages.data();

		std::vector<VkSemaphore> vulkanSignalSemaphores;
		vulkanSignalSemaphores.reserve(waitSemaphores.size());
		for (RHI::Semaphore* pSemaphore : signalSemaphores)
		{
			vulkanSignalSemaphores.push_back(((VulkanSemaphore*)pSemaphore)->GetSemaphore());
		}
		info.signalSemaphoreCount = u32(vulkanSignalSemaphores.size());
		info.pSignalSemaphores = vulkanSignalSemaphores.data();

		VkResult vkres = pVulkanQueue->vkSubmit(info, fence);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to submit the vulkan command buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return false;
		}
		m_Status = RHI::CommandListState::Submited;
		m_pFence->SetSubmitted();

		return true;
	}

	b8 VulkanCommandList::Wait(u64 timeout)
	{
		if (m_Status == RHI::CommandListState::Submited && m_pFence->GetStatus() == RHI::FenceStatus::Submitted)
		{
			b8 res = m_pFence->Wait(timeout);
			if (!res)
				return false;

			m_Status = RHI::CommandListState::Finished;
		}
		else if (m_pFence->GetStatus() == RHI::FenceStatus::Signaled)
		{
			m_Status = RHI::CommandListState::Finished;
		}
		return true;
	}

	b8 VulkanCommandList::Create(RHI::RHIContext* pContext, RHI::CommandListManager* pManager, RHI::Queue* pQueue)
	{
		m_pContext = pContext;
		m_pManager = pManager;
		m_pQueue = pQueue;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		// TODO: Secondary buffer?
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = ((VulkanCommandListManager*)m_pManager)->GetCommandPool(m_pQueue);

		VkResult vkres = pDevice->VkAllocateCommandBuffer(allocInfo, &m_CommandBuffer);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create a vulkan command buffer (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		// Create fence
		m_pFence = new VulkanFence();
		b8 res = m_pFence->Create(m_pContext, false);
		if (!res)
		{
			//g_Logger.LogError(LogVulkanRHI(), "Failed to create command list fence");
			Destroy();
			return false;
		}

		return true;
	}

	b8 VulkanCommandList::Destroy()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		if (m_pFence)
		{
			m_pFence->Destroy();
			delete m_pFence;
			m_pFence = nullptr;
		}

		if (m_CommandBuffer)
		{
			VkCommandPool commandPool = ((VulkanCommandListManager*)m_pManager)->GetCommandPool(m_pQueue);
			pDevice->vkFreeCommandBuffers(commandPool, m_CommandBuffer);
			m_CommandBuffer = VK_NULL_HANDLE;
		}

		return true;
	}

	void VulkanCommandList::UpdateBarriers()
	{
		if (m_GlobalBarriers.size() > 0 || m_BufferBarriers.size() > 0 || m_ImageBarriers.size() > 0)
		{
			VkPipelineStageFlags srcStage = Helpers::GetPipelineStage(m_SrcStage);
			VkPipelineStageFlags dstStage = Helpers::GetPipelineStage(m_DstStage);
			vkCmdPipelineBarrier(m_CommandBuffer,
				srcStage, dstStage,
				0, // TODO
				u32(m_GlobalBarriers.size()), m_GlobalBarriers.data(),
				u32(m_BufferBarriers.size()), m_BufferBarriers.data(),
				u32(m_ImageBarriers.size()), m_ImageBarriers.data());

			m_GlobalBarriers.clear();
			m_BufferBarriers.clear();
			m_ImageBarriers.clear();

			m_SrcStage = RHI::PipelineStage::None;
			m_DstStage = RHI::PipelineStage::None;
		}
	}
}

#undef CHECK_RECORDING
