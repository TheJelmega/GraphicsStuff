#pragma once
#include "../General/TypesAndMacros.h"

enum class PixelFormatComponents : u8
{
	UNKOWN,
	R8,
	R8G8,
	R8G8B8,
	R8G8B8A8,
	B8G8R8A8,
	R16,
	R16G16,
	R16G16B16A16,
	R32,
	R32G32,
	R32G32B32,
	R32G32B32A32,
	R5G6B5,
	R11G11B10,
	A2R10G10B10,
	D24,
	D24S8,
	D32,
	D32S8,
	BC1,
	BC2,
	BC3,
	BC4,
	BC5,
	BC6H,
	BC7,
	ASTC4X4,
	ASTC6X6,
	ASTC8X8,
	ASTC10X10,
	ASTC12X12,
	Count
};

/**
 * transform of the the image data
 */
enum class PixelFormatTransform : u8
{
	UNDEFINED,	/**< Undefined */
	UNORM,		/**< Unsigned normalized [0, 1] */
	SNORM,		/**< Signed normalized [-1, 1] */
	UINT,		/**< Unsigned integer */
	SINT,		/**< Signed integer */
	UFLOAT,		/**< Unsigned float */
	SFLOAT,		/**< Signed float */
	USCALED,	/**< Unsigned scaled (integer values that get converted to floats) */
	SSCALED,	/**< Signed scaled (integer values that get converted to floats) */
	SRGB,		/**< SRGB (non-linear) */
	Count
};

namespace Detail {

	static u8 g_FormatSizes[u32(PixelFormatComponents::Count)]
	{
		0,		// UNKOWN
		1,		// R8
		2,		// R8G8
		3,		// R8G8B8
		4,		// R8G8B8A8
		4,		// B8G8R8A8
		2,		// R16
		4,		// R16G16
		8,		// R16G16B16A16
		4,		// R32
		8,		// R32G32
		12,		// R32G32B32
		16,		// R32G32B32A32
		2,		// R5G6B5
		4,		// R11G11B10,
		4,		// A2R10G10B10,
		3,		// D24
		4,		// D24S8
		4,		// D32
		5,		// D32S8
		0xFF,	// BC1
		0xFF,	// BC2
		0xFF,	// BC3
		0xFF,	// BC4
		0xFF,	// BC5
		0xFF,	// BC6H
		0xFF,	// BC7
		0xFF,	// ASTC4X4
		0xFF,	// ASTC6X6
		0xFF,	// ASTC8X8
		0xFF,	// ASTC10X10
		0xFF,	// ASTC12X12
	};

}

/**
 * Pixel format
 */
struct PixelFormat
{
	PixelFormatComponents components;
	PixelFormatTransform transform;

	PixelFormat()
		: components(PixelFormatComponents::UNKOWN)
		, transform(PixelFormatTransform::UNDEFINED)
	{}

	PixelFormat(PixelFormatComponents components, PixelFormatTransform transform)
		: components(components)
		, transform(transform)
	{}

	b8 operator==(const PixelFormat& format) const { return components == format.components && transform == format.transform; }
	b8 operator!=(const PixelFormat& format) const { return components != format.components || transform != format.transform; }

	/**
	* Check if the format has a depth component
	* @return				True if the format has a depth component, false otherwise
	*/
	b8 HasDepthComponent() const
	{
		switch (components)
		{
		case PixelFormatComponents::D24:
		case PixelFormatComponents::D24S8:
		case PixelFormatComponents::D32:
		case PixelFormatComponents::D32S8:
			return true;
		default:
			return false;
		}
	}

	/**
	* Check if the format has a depth component
	* @return				True if the format has a depth component, false otherwise
	*/
	b8 HasStencilComponent() const
	{
		switch (components)
		{
		case PixelFormatComponents::D24S8:
		case PixelFormatComponents::D32S8:
			return true;
		default:
			return false;
		}
	}


	/**
	* Get the number of bytes that 1 element in the format takes up
	* @return Size of the element of a format in bytes, 0xFFFF if the format is not stored as single pixels, or 00 if format is unknown
	*/
	u8 GetSize() const
	{
		if (u8(components) < u8(PixelFormatComponents::Count))
			return 0;
		return Detail::g_FormatSizes[u8(components)];
	}

};


