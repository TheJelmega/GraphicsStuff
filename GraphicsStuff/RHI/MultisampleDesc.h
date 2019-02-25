// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// MultisampleDesc.h: Multisample descriptor
#pragma once
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {

	// TODO check for additional options, like vulkan has
	struct MultisampleDesc
	{
		b8 enable;					/**< Enable multisampling */
		RHI::SampleCount samples;	/**< Multisample count */
	};

}
