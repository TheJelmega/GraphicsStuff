
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetManager.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanHelpers.h"

namespace Vulkan {


	VulkanDescriptorSet::VulkanDescriptorSet()
		: m_DescriptorSet(VK_NULL_HANDLE)
		, m_Pool(VK_NULL_HANDLE)
	{
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
	}

	b8 VulkanDescriptorSet::Create(RHI::RHIContext* pContext, RHI::DescriptorSetManager* pManager, RHI::DescriptorSetLayout* pLayout)
	{
		m_pContext = pContext;
		m_pManager = pManager;
		m_pLayout = pLayout;
		m_Pool = ((VulkanDescriptorSetManager*)m_pManager)->GetDescriptorPool();

		VkDescriptorSetLayout layout = ((VulkanDescriptorSetLayout*)m_pLayout)->GetLayout();

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_Pool;
		allocInfo.descriptorSetCount = 1;	// TODO: How to do more than 1???
		allocInfo.pSetLayouts = &layout;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres = pDevice->vkAllocateDescriptorSet(allocInfo, m_DescriptorSet);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to allocate the vulkan descriptor set (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			Destroy();
			return false;
		}

		pLayout->IncRefs();
		return true;
	}

	b8 VulkanDescriptorSet::Destroy()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		if (m_DescriptorSet)
		{
			pDevice->vkFreeDescriptorSet(m_Pool, m_DescriptorSet);
			m_DescriptorSet = VK_NULL_HANDLE;
			m_pLayout->DecRefs();
		}

		return true;
	}

	b8 VulkanDescriptorSet::Write(u32 binding, RHI::Buffer* pBuffer, u64 offset, u64 range, u32 arrayElement)
	{
		std::vector<RHI::DescriptorSetBinding> bindings = m_pLayout->GetBindings();

		assert(binding < bindings.size());
		assert(arrayElement < bindings[binding].count);
		assert(offset < pBuffer->GetSize());

		if (range == u64(-1) || range > pBuffer->GetSize() - offset)
		{
			range = pBuffer->GetSize() - offset;
		}

		VkWriteDescriptorSet writeInfo = {};
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet = m_DescriptorSet;
		writeInfo.descriptorType = Helpers::GetDescriptorType(bindings[binding].type);
		writeInfo.dstBinding = binding;
		writeInfo.dstArrayElement = arrayElement;

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.offset = 0;
		bufferInfo.buffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
		bufferInfo.offset = offset;
		bufferInfo.range = range;

		writeInfo.descriptorCount = 1;
		writeInfo.pBufferInfo = &bufferInfo;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		pDevice->vkUpdateDescriptorSets(1, &writeInfo, 0, nullptr);

		return true;
	}

	b8 VulkanDescriptorSet::Write(u32 binding, RHI::Texture* pTexture, RHI::Sampler* pSampler, u32 arrayElement)
	{
		std::vector<RHI::DescriptorSetBinding> bindings = m_pLayout->GetBindings();

		assert(binding < bindings.size());

		VkWriteDescriptorSet writeInfo = {};
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet = m_DescriptorSet;
		writeInfo.descriptorType = Helpers::GetDescriptorType(bindings[binding].type);
		writeInfo.dstBinding = binding;
		writeInfo.dstArrayElement = arrayElement;

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = Helpers::GetImageLayout(pTexture->GetLayout());
		imageInfo.imageView = ((VulkanTexture*)pTexture)->GetImageView();
		imageInfo.sampler = ((VulkanSampler*)pSampler)->GetSampler();

		writeInfo.descriptorCount = 1;
		writeInfo.pImageInfo = &imageInfo;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		pDevice->vkUpdateDescriptorSets(1, &writeInfo, 0, nullptr);

		return true;
	}

	b8 VulkanDescriptorSet::Write(const std::vector<RHI::DescriptorSetBufferInfo>& buffers,
		const std::vector<RHI::DescriptorSetTextureSamplerInfo>& texAndSamplers)
	{
		std::vector<VkWriteDescriptorSet> writeInfos;
		std::vector<VkDescriptorBufferInfo> bufferInfos;
		std::vector<VkBufferView> texelBuffers;
		std::vector<VkDescriptorImageInfo> imageInfos;

		writeInfos.reserve(buffers.size() + texAndSamplers.size());
		bufferInfos.reserve(buffers.size());
		imageInfos.reserve(texAndSamplers.size());

		std::vector<RHI::DescriptorSetBinding> bindings = m_pLayout->GetBindings();

		for (const RHI::DescriptorSetBufferInfo& info : buffers)
		{
			assert(info.buffers.size() == info.offsets.size());
			assert(info.buffers.size() == info.ranges.size());

			VkWriteDescriptorSet writeInfo = {};
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.dstSet = m_DescriptorSet;
			writeInfo.descriptorType = Helpers::GetDescriptorType(bindings[info.binding].type);
			writeInfo.dstBinding = info.binding;
			writeInfo.dstArrayElement = info.arrayElement;

			RHI::DescriptorSetBindingType type = bindings[info.binding].type;
			if (type == RHI::DescriptorSetBindingType::StorageTexelBuffer || type == RHI::DescriptorSetBindingType::UniformTexelBuffer)
			{
				sizeT arrOffset = texelBuffers.size();
				for (sizeT i = 0; i < info.buffers.size(); ++i)
				{
					VulkanBuffer* pBuffer = (VulkanBuffer*)info.buffers[i];

					texelBuffers.push_back(pBuffer->GetBufferView());
				}

				writeInfo.descriptorCount = u32(info.buffers.size());
				writeInfo.pTexelBufferView = texelBuffers.data() + arrOffset;
			}
			else
			{
				sizeT arrOffset = bufferInfos.size();
				for (sizeT i = 0; i < info.buffers.size(); ++i)
				{
					RHI::Buffer* pBuffer = info.buffers[i];
					u64 offset = info.offsets[i];
					u64 range = info.ranges[i];

					if (range == u64(-1) || range > pBuffer->GetSize() - offset)
					{
						range = pBuffer->GetSize() - offset;
					}

					VkDescriptorBufferInfo bufferInfo = {};
					bufferInfo.offset = 0;
					bufferInfo.buffer = ((VulkanBuffer*)pBuffer)->GetBuffer();
					bufferInfo.offset = offset;
					bufferInfo.range = range;

					bufferInfos.push_back(bufferInfo);
				}

				writeInfo.descriptorCount = u32(info.buffers.size());
				writeInfo.pBufferInfo = bufferInfos.data() + arrOffset;
			}

			writeInfos.push_back(writeInfo);	
		}

		for (const RHI::DescriptorSetTextureSamplerInfo& info : texAndSamplers)
		{
			sizeT arrOffset = imageInfos.size();

			RHI::DescriptorSetBindingType type = bindings[info.binding].type;
			if (type == RHI::DescriptorSetBindingType::CombinedImageSampler)
			{
				assert(info.textures.size() == info.samplers.size());
				for (sizeT i = 0; i < info.textures.size(); ++i)
				{
					RHI::Texture* pTexture = info.textures[i];
					RHI::Sampler* pSampler = info.samplers[i];
						
					VkDescriptorImageInfo imageInfo = {};
					imageInfo.imageLayout = Helpers::GetImageLayout(pTexture->GetLayout());
					imageInfo.imageView = ((VulkanTexture*)pTexture)->GetImageView();
					imageInfo.sampler = ((VulkanSampler*)pSampler)->GetSampler();

					imageInfos.push_back(imageInfo);
				}
			}
			else if (type == RHI::DescriptorSetBindingType::Sampler)
			{
				for (sizeT i = 0; i < info.samplers.size(); ++i)
				{
					RHI::Sampler* pSampler = info.samplers[i];

					VkDescriptorImageInfo imageInfo = {};
					imageInfo.sampler = ((VulkanSampler*)pSampler)->GetSampler();

					imageInfos.push_back(imageInfo);
				}
			}
			else
			{
				for (sizeT i = 0; i < info.textures.size(); ++i)
				{
					RHI::Texture* pTexture = info.textures[i];

					VkDescriptorImageInfo imageInfo = {};
					imageInfo.imageLayout = Helpers::GetImageLayout(pTexture->GetLayout());
					imageInfo.imageView = ((VulkanTexture*)pTexture)->GetImageView();

					imageInfos.push_back(imageInfo);
				}
			}

			VkWriteDescriptorSet writeInfo = {};
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.dstSet = m_DescriptorSet;
			writeInfo.descriptorType = Helpers::GetDescriptorType(bindings[info.binding].type);
			writeInfo.dstBinding = info.binding;
			writeInfo.dstArrayElement = info.arrayElement;

			writeInfo.descriptorCount = type == RHI::DescriptorSetBindingType::Sampler ? u32(info.samplers.size()) : u32(info.textures.size());
			writeInfo.pImageInfo = imageInfos.data() + arrOffset;

			writeInfos.push_back(writeInfo);
		}

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		pDevice->vkUpdateDescriptorSets(u32(writeInfos.size()), writeInfos.data(), 0, nullptr);

		return true;
	}
}
