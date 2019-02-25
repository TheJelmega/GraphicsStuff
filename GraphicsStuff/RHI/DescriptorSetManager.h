// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// DescriptorSetManager.h: Descriptor set manager
#pragma once
#include "RHICommon.h"
#include "DescriptorSetLayout.h"

namespace RHI {
	class DescriptorSet;
	class RHIContext;

	class DescriptorSetManager
	{
	public:
		DescriptorSetManager();
		virtual ~DescriptorSetManager();

		/**
		 * Create the descriptor set manager
		 * @param[in] pContext	RHI context
		 * @return				True if the descriptor set manager was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext) = 0;
		/**
		 * Create the descriptor set manager
		 * @return	True if the descriptor set manager was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Create a descriptor set
		 * @param[in] bindings	Descriptor set bindings
		 * @return				Pointer to a descriptor set, nullptr if the creation failed
		 */
		virtual DescriptorSetLayout* CreateDescriptorSetLayout(const std::vector<DescriptorSetBinding>& bindings) = 0;
		/**
		 * Create a descriptor set
		 * @param[in] pLayout	Descriptor set layout
		 * @return				Pointer to a descriptor set, nullptr if the creation failed
		 */
		virtual DescriptorSet* CreateDescriptorSet(DescriptorSetLayout* pLayout) = 0;
		/**
		 * Destroy a descriptor set
		 * @param[in] pLayout	Descriptor set layout to destroy
		 * @return				True if the descriptor set layout was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyDescriptorSetLayout(DescriptorSetLayout* pLayout) = 0;
		/**
		 * Destroy a descriptor set
		 * @param[in] pDescriptorSet	Descriptor set to destroy
		 * @return						True if the descriptor set was destroyed successfully, false otherwise
		 */
		virtual b8 DestroyDescriptorSet(DescriptorSet* pDescriptorSet) = 0;


	protected:
		RHIContext* m_pContext;
		std::vector<DescriptorSetLayout*> m_Layouts;
		std::vector<DescriptorSet*> m_DescriptorSets;
	};

}
