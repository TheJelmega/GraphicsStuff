// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Sampler.h: Sampler
#pragma once
#include <glm/vec4.hpp>
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;
	
	struct SamplerDesc
	{
		FilterMode magFilter;		/**< Magnification filter */
		FilterMode minFilter;		/**< Minification filter */
		AddressMode addressU;		/**< U coordinate address mode */
		AddressMode addressV;		/**< V coordinate address mode */
		AddressMode addressW;		/**< W coordinate address mode */

		b8 enableAnisotropy;		/**< Enable anisotropic filtering */
		u8 anisotropy;				/**< Anisotropy used */
		
		b8 enableCompare;			/**< Enable reference value compare */
		CompareOp compareOp;		/**< Compare op */

		f32 mipLodBias;				/**< LOD bias */
		f32 minLod;					/**< Min LOD */
		f32 maxLod;					/**< Max LOD */
		
		glm::vec4 borderColor;		/**< Border color (exact color not quarenteed and based upon RHI) */

		b8 unnormalizedCoordinates;	/**< Use unnormalized coordinates (has restriction on use cases) */

		/**
		 * Set the sampler's filter mode
		 * @param[in] filter	Filter mode
		 */
		void SetFilter(FilterMode filter) { magFilter = minFilter = filter; }
		/**
		 * Set the sampler's address mode
		 * @param[in] addressMode	Address mode
		 */
		void SetAddressMode(AddressMode addressMode) { addressU = addressV = addressW = addressMode; }
	};

	class Sampler
	{
	public:
		Sampler();
		virtual ~Sampler();

		/**
		 * Create the sampler
		 * @param[in] pContext	RHI context
		 * @param[in] desc		Sampler description
		 * @return				True if the sampler was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, const SamplerDesc& desc) = 0;
		/**
		 * Create the sampler
		 * @return				True if the sampler was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

	protected:
		RHIContext* m_pContext;	/**< RHI context */
		SamplerDesc m_Desc;				/**< Sampler description */
	};

}
