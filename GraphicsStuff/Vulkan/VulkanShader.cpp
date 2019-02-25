
#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanHelpers.h"

namespace Vulkan {


	VulkanShader::VulkanShader()
		: Shader()
		, m_ShaderModule(VK_NULL_HANDLE)
		, m_StageInfo()
	{
	}

	VulkanShader::~VulkanShader()
	{
	}

	b8 VulkanShader::Create(RHI::RHIContext* pContext, const RHI::ShaderDesc& desc)
	{
		assert(desc.language == RHI::ShaderLanguage::Spirv);
		m_pContext = pContext;
		m_Type = desc.type;
		m_FilePath = desc.filePath;
		m_EntryPoint = desc.entryPoint;
		m_Language = desc.language;

		b8 res = LoadShaderSource();
		if (!res)
			return false;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_Source.size();
		createInfo.pCode = (u32*)m_Source.data();

		VkResult vkres = pDevice->vkCreateShaderModule(createInfo, m_ShaderModule);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Error, "Failed to create shader module (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		// Create shader stage info
		// TODO: how to do specialization info
		m_StageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_StageInfo.module = m_ShaderModule;
		m_StageInfo.stage = Helpers::GetShaderStage(m_Type);
		m_StageInfo.pName = m_EntryPoint.c_str();

		return true;
	}

	b8 VulkanShader::Destroy()
	{
		if (m_ShaderModule)
		{
			VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
			pDevice->vkDestroyShaderModule(m_ShaderModule);
			m_ShaderModule = VK_NULL_HANDLE;
		}
		return true;
	}
}
