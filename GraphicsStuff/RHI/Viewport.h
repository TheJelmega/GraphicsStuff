// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// Viewport.h: Viewport
#pragma once
#include "../General/TypesAndMacros.h"

namespace RHI {
	
	struct Viewport
	{
		f32 x;			/**< Left position */
		f32 y;			/**< Top position */
		f32 width;		/**< Width */
		f32 height;		/**< Height */
		f32 minDepth;	/**< Min depth [0, 1] */
		f32 maxDepth;	/**< Max depth [0, 1] */

		Viewport()
			: x(0.f)
			, y(0.f)
			, width(0.f)
			, height(0.f)
			, minDepth(0.f)
			, maxDepth(0.f)
		{
		}

		Viewport(f32 x, f32 y, f32 width, f32 height)
			: x(x)
			, y(y)
			, width(width)
			, height(height)
			, minDepth(0.f)
			, maxDepth(1.f)
		{
		}

		Viewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth, f32 maxDepth)
			: x(x)
			, y(y)
			, width(width)
			, height(height)
			, minDepth(minDepth)
			, maxDepth(maxDepth)
		{
		}
	};

}