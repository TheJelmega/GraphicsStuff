#include "RHIHelpers.h"
#include "../General/TypesAndMacros.h"
#include "GpuInfo.h"
#include "Texture.h"

namespace RHI { namespace Helpers {

	const char* GetGpuVendorString(GpuVendor vendor)
	{
		switch (vendor)
		{
		default:
		case GpuVendor::Unknown:	return "Unkown";
		case GpuVendor::Nvidia:		return "Nvidia";
		case GpuVendor::AMD:		return "AMD";
		case GpuVendor::Intel:		return "Intel";
		case GpuVendor::ARM:		return "ARM";
		case GpuVendor::Qualcomm:	return "Qualcomm";
		case GpuVendor::ImgTec:		return "Imagination Technologies";
		}
	}

	TextureLayout GetTextureLayoutFromRTType(RenderTargetType type)
	{
		assert(u8(type) <= u8(RenderTargetType::MultisampleResolve));
		return Tables::g_RTLayouts[u8(type)];
	}
} }
