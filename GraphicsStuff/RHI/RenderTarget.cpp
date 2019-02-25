
#include "RenderTarget.h"

namespace RHI {

	RenderTarget::RenderTarget()
		: m_pContext(nullptr)
		, m_pTexture(nullptr)
		, m_Type(RenderTargetType::None)
	{
	}

	RenderTarget::~RenderTarget()
	{
	}
}
