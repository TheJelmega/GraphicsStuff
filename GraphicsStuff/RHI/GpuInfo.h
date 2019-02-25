#pragma once
#include "../General/TypesAndMacros.h"

namespace RHI {

	/**
	* Gpu driver version
	*/
	struct DriverVersion
	{
		u16 major;
		u16 minor;
	};
	/**
	* GPU vendor
	*/
	enum class GpuVendor : u8
	{
		Unknown,
		Nvidia,
		AMD,
		Intel,
		ARM,
		Qualcomm,
		ImgTec
	};

	enum class GpuType : u8
	{
		Unknown,
		Discrete,
		Integrated,
		CPU,
		Software,
		Count
	};

	struct GpuInfo
	{
		DriverVersion driver;		/**< Driver version */
		GpuVendor vendor;			/**< GPU vendor */
		GpuType type;				/**< GPU type */
		char name[256] = {};		/**< GPU name */
	};


}