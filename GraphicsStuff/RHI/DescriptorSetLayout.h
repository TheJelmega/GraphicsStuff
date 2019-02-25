// Copyright 2018 Jelte Meganck. All Rights Reserved.
//
// DescriptorSetLayout.h: Descriptor set layout
#pragma once
#include <vector>
#include "../General/TypesAndMacros.h"
#include "RHICommon.h"

namespace RHI {
	class RHIContext;
	class DescriptorSetManager;

	struct DescriptorSetBinding
	{
		DescriptorSetBindingType type;	/**< Descriptor set type */
		ShaderType shadertype;	/**< Shader type/stage */
		u32 count;				/**< Number of uniform variables of this type, 0 is reserved */
	};

	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout();
		virtual ~DescriptorSetLayout();

		/**
		 * Create the descriptor set
		 * @param[in] pContext	RHI context
		 * @param[in] pManager	Descriptor set manager
		 * @param[in] bindings	Descriptor set bindings
		 * @return				True if the descriptor set was created successfully, false otherwise
		 */
		virtual b8 Create(RHIContext* pContext, DescriptorSetManager* pManager, const std::vector<DescriptorSetBinding>& bindings) = 0;
		/**
		 * Destroy the descriptor set
		 * @return	True if the descriptor set was destroyed successfully, false otherwise
		 */
		virtual b8 Destroy() = 0;

		/**
		 * Check if a group of binding matches the layout's bindings
		 * @return	True if the bindings match, false otherwise
		 */
		b8 Matches(const std::vector<DescriptorSetBinding>& bindings);

		/**
		 * Get the layout's descriptor set bindings
		 * @return	Descriptor set bindings
		 */
		std::vector<DescriptorSetBinding>& GetBindings() { return m_Bindings; }

		/**
		 * Increment the layout reference count
		 */
		void IncRefs() { ++m_RefCount; }
		/**
		 * Decrement the layout reference count
		 */
		void DecRefs() { if (m_RefCount > 0) { --m_RefCount; } }
		/**
		 * Get the layout's reference count
		 * @return	Reference count
		 */
		u32 GetReferenceCount() const { return m_RefCount; }
		
	protected:
		RHIContext* m_pContext;
		DescriptorSetManager* m_pManager;
		std::vector<DescriptorSetBinding> m_Bindings;
		u32 m_RefCount;
	};

}
