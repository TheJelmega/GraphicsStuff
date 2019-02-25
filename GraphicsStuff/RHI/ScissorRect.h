// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// ScissorRect.h: Scissor rect
#pragma once
#include "../General/TypesAndMacros.h"

namespace RHI {

	struct ScissorRect
	{
		i32 x;			/**< Left position */
		i32 y;			/**< Top position */
		u32 width;		/**< Width */
		u32 height;		/**< Height */

		ScissorRect()
			: x(0)
			, y(0)
			, width(0)
			, height(0)
		{
		}

		ScissorRect(i32 x, i32 y, u32 width, u32 height)
			: x(x)
			, y(y)
			, width(width)
			, height(height)
		{
		}


	};

}
