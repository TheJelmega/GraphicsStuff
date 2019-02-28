
#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "VulkanHelpers.h"

namespace Vulkan {

	VulkanPipeline::VulkanPipeline()
		: Pipeline()
		, m_Layout(VK_NULL_HANDLE)
		, m_Pipeline(VK_NULL_HANDLE)
	{
	}

	VulkanPipeline::~VulkanPipeline()
	{
	}

	b8 VulkanPipeline::Create(RHI::RHIContext* pContext, const RHI::GraphicsPipelineDesc& desc)
	{
		m_pContext = pContext;
		m_Type = RHI::PipelineType::Graphics;
		m_GraphicsDesc = desc;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres;

		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// TODO: Push constants?

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		for (const RHI::DescriptorSetLayout* pLayout : desc.descriptorSetLayouts)
		{
			descriptorSetLayouts.push_back(((VulkanDescriptorSetLayout*)pLayout)->GetLayout());
		}

		layoutInfo.setLayoutCount = u32(descriptorSetLayouts.size());
		layoutInfo.pSetLayouts = descriptorSetLayouts.data();

		vkres = pDevice->vkCreatePipelineLayout(layoutInfo, m_Layout);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the compute pipeline layout (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		// TODO: flags??
		pipelineInfo.layout = m_Layout;
		pipelineInfo.renderPass = ((VulkanRenderPass*)desc.pRenderPass)->GetRenderPass();

		// Shader stages
		assert(desc.pVertexShader && desc.pFragmentShader);
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		stages.push_back(((VulkanShader*)desc.pVertexShader)->GetStageInfo());
		stages.push_back(((VulkanShader*)desc.pFragmentShader)->GetStageInfo());
		if (desc.pGeometryShader)
		{
			stages.push_back(((VulkanShader*)desc.pGeometryShader)->GetStageInfo());
		}
		if (desc.tesellation.pHullShader && desc.tesellation.pDomainShader && desc.tesellation.enabled)
		{
			stages.push_back(((VulkanShader*)desc.tesellation.pHullShader)->GetStageInfo());
			stages.push_back(((VulkanShader*)desc.tesellation.pDomainShader)->GetStageInfo());
		}
		pipelineInfo.stageCount = u32(stages.size());
		pipelineInfo.pStages = stages.data();

		// Vertex input state
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		Helpers::GetInputDescription(desc.inputDescriptor, bindingDescriptions, attributeDescriptions);

		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = u32(bindingDescriptions.size());
		vertexInputState.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputState.vertexAttributeDescriptionCount = u32(attributeDescriptions.size());
		vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();
		pipelineInfo.pVertexInputState = &vertexInputState;

		// Input assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = Helpers::GetPrimitiveTopology(desc.primitiveTopology);
		inputAssembly.primitiveRestartEnable = desc.enablePrimitiveRestart;
		pipelineInfo.pInputAssemblyState = &inputAssembly;

		// Resterizer
		VkPipelineRasterizationStateCreateInfo rasterState = {};
		rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterState.polygonMode = Helpers::GetPolygonMode(desc.rasterizer.fillMode);
		rasterState.cullMode = Helpers::GetCullMode(desc.rasterizer.cullMode);
		rasterState.frontFace = Helpers::GetFrontFace(desc.rasterizer.frontFace);
		rasterState.rasterizerDiscardEnable = desc.rasterizer.enableDiscard;
		rasterState.lineWidth = desc.rasterizer.lineWidth;
		rasterState.depthClampEnable = desc.rasterizer.enabledepthClamp;
		rasterState.depthBiasEnable = desc.rasterizer.enabledepthBias;
		rasterState.depthBiasClamp = desc.rasterizer.depthBiasClamp;
		rasterState.depthBiasConstantFactor = desc.rasterizer.depthBiasConstantFactor;
		rasterState.depthBiasSlopeFactor = desc.rasterizer.depthBiasSlopeFactor;
		pipelineInfo.pRasterizationState = &rasterState;

		VkViewport viewport;
		viewport.x = desc.viewport.x;
		viewport.y = desc.viewport.y;
		viewport.width = desc.viewport.width;
		viewport.height = desc.viewport.height;
		viewport.minDepth = desc.viewport.minDepth;
		viewport.maxDepth = desc.viewport.maxDepth;
		VkRect2D scissor;
		scissor.offset.x = desc.scissor.x;
		scissor.offset.y = desc.scissor.y;
		scissor.extent.width = desc.scissor.width;
		scissor.extent.height = desc.scissor.height;
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pViewports = &viewport;
		viewportState.viewportCount = 1;
		viewportState.pScissors = &scissor;
		viewportState.scissorCount = 1;
		pipelineInfo.pViewportState = &viewportState;

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = desc.multisample.enable ? Helpers::GetSampleCount(desc.multisample.samples) : VK_SAMPLE_COUNT_1_BIT;
		pipelineInfo.pMultisampleState = &multisampleState;

		// Blend state
		std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
		blendAttachments.reserve(desc.blendState.attachments.size());
		for (const RHI::BlendAttachment& attachment : desc.blendState.attachments)
		{
			VkPipelineColorBlendAttachmentState blendAttachment = {};
			blendAttachment.blendEnable = attachment.enable;
			blendAttachment.colorWriteMask = Helpers::GetColorWriteMask(attachment.components);
			blendAttachment.colorBlendOp = Helpers::GetBlendOp(attachment.colorBlendOp);
			blendAttachment.srcColorBlendFactor = Helpers::GetBlendFactor(attachment.srcColorBlendFactor);
			blendAttachment.dstColorBlendFactor = Helpers::GetBlendFactor(attachment.dstColorBlendFactor);
			blendAttachment.colorBlendOp = Helpers::GetBlendOp(attachment.alphaBlendOp);
			blendAttachment.srcColorBlendFactor = Helpers::GetBlendFactor(attachment.srcAlphaBlendFactor);
			blendAttachment.dstColorBlendFactor = Helpers::GetBlendFactor(attachment.dstAlphaBlendFactor);
			blendAttachments.push_back(blendAttachment);
		}

		VkPipelineColorBlendStateCreateInfo blendState = {};
		blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendState.attachmentCount = u32(blendAttachments.size());
		blendState.pAttachments = blendAttachments.data();
		blendState.logicOpEnable = desc.blendState.enableLogicOp;
		blendState.logicOp = Helpers::GetLogicOp(desc.blendState.logicOp);
		pipelineInfo.pColorBlendState = &blendState;

		// Depth stencic state
		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = desc.depthStencil.enableDepthTest;
		depthStencilState.depthWriteEnable = desc.depthStencil.enableDepthWrite;
		depthStencilState.depthBoundsTestEnable = desc.depthStencil.enableDepthBoundTest;
		depthStencilState.stencilTestEnable = desc.depthStencil.enableStencilTest;
		depthStencilState.depthCompareOp = Helpers::GetCompareOp(desc.depthStencil.depthCompareOp);

		VkStencilOpState stencilOp;
		const RHI::StencilOpState& frontStencilState = desc.depthStencil.front;
		stencilOp.failOp = Helpers::GetStencilOp(frontStencilState.failOp);
		stencilOp.passOp = Helpers::GetStencilOp(frontStencilState.passOp);
		stencilOp.depthFailOp = Helpers::GetStencilOp(frontStencilState.depthFailOp);
		stencilOp.compareOp = Helpers::GetCompareOp(frontStencilState.compareOp);
		stencilOp.compareMask = frontStencilState.compareMask;
		stencilOp.writeMask = frontStencilState.writeMask;
		stencilOp.reference = frontStencilState.reference;
		depthStencilState.front = stencilOp;

		const RHI::StencilOpState& backStencilState = desc.depthStencil.front;
		stencilOp.failOp = Helpers::GetStencilOp(backStencilState.failOp);
		stencilOp.passOp = Helpers::GetStencilOp(backStencilState.passOp);
		stencilOp.depthFailOp = Helpers::GetStencilOp(backStencilState.depthFailOp);
		stencilOp.compareOp = Helpers::GetCompareOp(backStencilState.compareOp);
		stencilOp.compareMask = backStencilState.compareMask;
		stencilOp.writeMask = backStencilState.writeMask;
		stencilOp.reference = backStencilState.reference;
		depthStencilState.back = stencilOp;

		depthStencilState.minDepthBounds = desc.depthStencil.minDepthBound;
		depthStencilState.maxDepthBounds = desc.depthStencil.maxDepthBound;

		pipelineInfo.pDepthStencilState = &depthStencilState;

		// Tessellation
		pipelineInfo.pTessellationState = nullptr;
		VkPipelineTessellationStateCreateInfo tessellationState = {};
		if (desc.tesellation.enabled)
		{
			tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			tessellationState.patchControlPoints = desc.tesellation.controlPoints;
			pipelineInfo.pTessellationState = &tessellationState;
		}

		// Dynamic state
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		std::vector<VkDynamicState> vkDynamicStates;
		pipelineInfo.pDynamicState = nullptr;
		if (u16(desc.dynamicState) != 0)
		{
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

			vkDynamicStates = Helpers::GetDynamicStates(desc.dynamicState);
			dynamicState.dynamicStateCount = u32(vkDynamicStates.size());
			dynamicState.pDynamicStates = vkDynamicStates.data();

			pipelineInfo.pDynamicState = &dynamicState;
		}

		// Unused
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = 0;

		vkres = pDevice->vkCreatePipeline(pipelineInfo, m_Pipeline);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the compute pipeline (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		return true;
	}

	b8 VulkanPipeline::Create(RHI::RHIContext* pContext, const RHI::ComputePipelineDesc& desc)
	{
		m_pContext = pContext;
		m_Type = RHI::PipelineType::Compute;
		m_ComputeDesc = desc;

		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();
		VkResult vkres;

		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		// TODO

		vkres = pDevice->vkCreatePipelineLayout(layoutInfo, m_Layout);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the compute pipeline layout (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		VkComputePipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		// TODO: flags??
		pipelineInfo.layout = m_Layout;
		pipelineInfo.stage = ((VulkanShader*)desc.pComputeShader)->GetStageInfo();

		vkres = pDevice->vkCreatePipeline(pipelineInfo, m_Pipeline);
		if (vkres != VK_SUCCESS)
		{
			//g_Logger.LogFormat(LogVulkanRHI(), LogLevel::Fatal, "Failed to create the compute pipeline (VkResult: %s)!", Helpers::GetResultstd::string(vkres));
			return vkres;
		}

		return true;
	}

	b8 VulkanPipeline::Destroy()
	{
		VulkanDevice* pDevice = ((VulkanContext*)m_pContext)->GetDevice();

		if (m_Pipeline)
		{
			pDevice->vkDestroyPipeline(m_Pipeline);
			m_Pipeline = VK_NULL_HANDLE;
		}

		if (m_Layout)
		{
			pDevice->vkDestroyPipelineLayout(m_Layout);
			m_Layout = VK_NULL_HANDLE;
		}

		return true;
	}
}
