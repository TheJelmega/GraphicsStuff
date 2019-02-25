// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// BlendStateDesc.h: Blend state descriptor
#pragma once
#include "../General/TypesAndMacros.h"

namespace RHI {
	class Shader;

	struct TessellationDesc
	{
		b8 enabled;
		Shader* pHullShader;
		Shader* pDomainShader;
		u32 controlPoints;
	};

}
