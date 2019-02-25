// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// DepthStencilDesc.h: Depth stencil descriptor
#pragma once
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {

	struct DepthStencilDesc
	{
		b8 enableDepthTest;			/**< If a depth test needs to be used */
		b8 enableDepthWrite;		/**< If the depth needs to be written to */
		b8 enableDepthBoundTest;	/**< If a depth bound test needs to be used */
		b8 enableStencilTest;		/**< If a stencil test needs to be used */
		CompareOp depthCompareOp;	/**< If a depth test needs to be used */
		StencilOpState front;		/**< Stencil op state for front face */
		StencilOpState back;		/**< Stencil op state for back face */
		f32 minDepthBound;			/**< Min depth bound for depth bound test */
		f32 maxDepthBound;			/**< Min depth bound for depth bound test */
	};

}
