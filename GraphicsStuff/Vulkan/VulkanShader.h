#pragma once
#include <vulkan/vulkan.h>
#include "../RHI/Shader.h"

namespace Vulkan {
	
	class VulkanShader final : public RHI::Shader
	{
	public:
		VulkanShader();
		~VulkanShader();

		/**
		* Create a shader
		* @param[in] pContext		RHI context
		* @param[in] desc			Shader description
		* @return	True if the shader was created successfully, false otherwise
		*/
		b8 Create(RHI::RHIContext* pContext, const RHI::ShaderDesc& desc) override final;

		/**
		 * Destroy the shader
		 * @return	True if the shader was destroyed successfully, false otherwise
		 */
		b8 Destroy() override final;

		const VkPipelineShaderStageCreateInfo& GetStageInfo() const { return m_StageInfo; }

	private:
		VkShaderModule m_ShaderModule;					/**< vulkan shader module */
		VkPipelineShaderStageCreateInfo m_StageInfo;	/**< Shader stage info */
	};

}
