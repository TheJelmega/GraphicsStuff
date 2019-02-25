
#include "DescriptorSetLayout.h"

namespace RHI {


	DescriptorSetLayout::DescriptorSetLayout()
		: m_pContext(nullptr)
		, m_pManager(nullptr)
	{
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
	}

	b8 DescriptorSetLayout::Matches(const std::vector<DescriptorSetBinding>& bindings)
	{
		if (bindings.size() != m_Bindings.size())
			return false;

		for (sizeT i = 0; i < m_Bindings.size(); ++i)
		{
			const DescriptorSetBinding& binding0 = m_Bindings[i];
			const DescriptorSetBinding& binding1 = bindings[i];

			if (binding0.type != binding1.type ||
				binding0.shadertype != binding1.shadertype ||
				binding0.count != binding1.count)
			{
				return false;
			}
		}
		return true;
	}
}
