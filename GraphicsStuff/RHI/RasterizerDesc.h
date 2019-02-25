// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// RasterizerDesc.h: Rasterizer descriptor
#pragma once
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	
	struct RasterizerDesc
	{
		RHI::FillMode fillMode;					/**< Fill mode */
		CullMode cullMode;					/**< Cull mode */
		FrontFace frontFace;				/**< Front face */
		b8 enableDiscard;					/**< If the geometry should discarded before the rasterizer state */
		b8 enabledepthClamp;				/**< Enable depth clamp */
		b8 enabledepthBias;					/**< Enable depth bias */
		f32 depthBiasConstantFactor;		/**< Scalar factor controlling the constant depth value added to each fragment */
		f32 depthBiasClamp;					/**< Maximum (or minimum) depth bias of a fragment */
		f32 depthBiasSlopeFactor;			/**< scalar factor applied to a fragment’s slope in depth bias calculations */
		f32 lineWidth;						/**< Line width */
	};

}
