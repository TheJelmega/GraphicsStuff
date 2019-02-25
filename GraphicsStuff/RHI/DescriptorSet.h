// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// DescriptorSet.h: Descriptor set
#pragma once
#include <vector>
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;
	class DescriptorSetLayout;
	class Sampler;
	class Texture;
	class Buffer;
	class DescriptorSetManager;

	struct DescriptorSetBufferInfo
	{
		u32 binding;				/**< Binding */
		u32 arrayElement;			/**< Initial offset in array */
		std::vector<Buffer*> buffers;	/**< Buffers */
		std::vector<u64> offsets;		/**< Buffer offsets */
		std::vector<u64> ranges;		/**< Buffer ranges (range of u64(-1) will use the whole buffer, regardless of size) */
	};

	struct DescriptorSetTextureSamplerInfo
	{
		u32 binding;					/**< Binding */
		u32 arrayElement;				/**< Initial offset in array */
		std::vector<Texture*> textures;	/**< Textures (may be empty for sampler descriptor type) */
		std::vector<Sampler*> samplers;	/**< Sampler (may be empty for image descriptor types) */
	};

	class DescriptorSet
	{
	public:
		DescriptorSet();
		virtual ~DescriptorSet();

		/**
		 * Create the descriptor set
		 * @param[in] pContext	RHI context
		 * @param[in] pManager	Descriptor set manager
		 * @param[in] pLayout	Descriptor set layout
		 * @return				True if the descriptor set was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, DescriptorSetManager* pManager, DescriptorSetLayout* pLayout) = 0;
		/**
		 * Destroy the descriptor set
		 * @return	True if the descriptor set was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Write a buffer to the descriptor set
		 * @param[in] binding		Binding to write to
		 * @param[in] pBuffer		Buffer to write
		 * @param[in] offset		Offset in buffer
		 * @param[in] range			Range in buffer
		 * @param[in] arrayElement	Element of the array to write buffer to
		 */
		virtual b8 Write(u32 binding, Buffer* pBuffer, u64 offset = 0, u64 range = u64(-1), u32 arrayElement = 0) = 0;
		/**
		 * Write a texture and a sampler to the descriptor set
		 * @param[in] binding	Binding to write to
		 * @param[in] pTexture	Texture to write
		 * @param[in] pSampler	sampler to write
		 * @param[in] arrayElement	Element of the array to write buffer to
		 */
		virtual b8 Write(u32 binding, Texture* pTexture, Sampler* pSampler, u32 arrayElement = 0) = 0;
		/**
		 * Write a texture and a sampler to the descriptor set
		 * @param[in] buffers			Buffer write info
		 * @param[in] texAndSamplers		Texture and sampler write info
		 */
		virtual b8 Write(const std::vector<DescriptorSetBufferInfo>& buffers, const std::vector<DescriptorSetTextureSamplerInfo>& texAndSamplers) = 0;

		/**
		 * Get the descriptor set's layout
		 * @return	Descriptor set layout
		 */
		DescriptorSetLayout* GetLayout() { return m_pLayout; }
		
	protected:
		RHIContext* m_pContext;		/**< RHI context */
		DescriptorSetManager* m_pManager;	/**< Descriptor set manager */
		DescriptorSetLayout* m_pLayout;		/**< Descriptor set layout */
	};

}
