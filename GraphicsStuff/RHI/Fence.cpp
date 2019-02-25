
#include "Fence.h"

namespace RHI {

	Fence::Fence()
		: m_pContext(nullptr)
		, m_Status(FenceStatus::Unsignaled)
		, m_FenceValue(0)
	{
	}

	Fence::~Fence()
	{
	}
}
