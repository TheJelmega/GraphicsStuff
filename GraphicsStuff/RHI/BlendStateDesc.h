// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// BlendStateDesc.h: Blend state descriptor
#pragma once
#include <vector>

#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	
	struct BlendAttachment
	{
		ColorComponentMask components;
		b8 enable;
		BlendFactor srcColorBlendFactor;
		BlendFactor dstColorBlendFactor;
		BlendOp colorBlendOp;
		BlendFactor srcAlphaBlendFactor;
		BlendFactor dstAlphaBlendFactor;
		BlendOp alphaBlendOp;
	};

	struct BlendStateDesc
	{
		b8 enableLogicOp;
		LogicOp logicOp;
		std::vector<BlendAttachment> attachments;
		// TODO: Add constant color
	};

}
