#pragma once
#include "../General/TypesAndMacros.h"
#include <glm/vec4.hpp>

namespace RHI {
	
	struct ClearValue
	{
		union
		{
			glm::vec4 color;
			struct
			{
				f32 depth;
				u32 stencil;
			};
		};

		ClearValue()
			: color()
		{
		}

		ClearValue(glm::vec4 color)
			: color(color)
		{
		}

		ClearValue(f32 depth, u32 stencil)
			: depth(depth)
			, stencil(stencil)
		{
		}

		ClearValue(const ClearValue& value)
			: color(value.color)
		{
		}

		ClearValue(ClearValue&& value) noexcept
			: color(std::forward<glm::vec4>(value.color))
		{
		}

		ClearValue& operator=(const ClearValue& value)
		{
			color = value.color;
			return *this;
		}

		ClearValue& operator=(ClearValue&& value) noexcept
		{
			color = std::forward<glm::vec4>(value.color);
			return *this;
		}
	};


}
