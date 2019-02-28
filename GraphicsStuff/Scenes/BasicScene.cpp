#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "BasicScene.h"
#include "../RHI/IDynamicRHI.h"
#include "../RHI/Shader.h"
#include "../RHI/Sampler.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "../RHI/CommandListManager.h"
#include "../RHI/Queue.h"
#include "../RHI/SwapChain.h"
#include "../RHI/RenderTarget.h"
#include "../RHI/DescriptorSetManager.h"


BasicScene::BasicScene()
	: m_pVertShader(nullptr)
	, m_pFragShader(nullptr)
	, m_pSampler(nullptr)
	, m_pUniformBuffer(nullptr)
{
}

BasicScene::~BasicScene()
{
}

b8 BasicScene::Init()
{
	Scene::Init();

	RHI::ShaderDesc vertexShader = { RHI::ShaderType::Vertex, "Shaders/Shader_vert.spv", "main", RHI::ShaderLanguage::Spirv };
	RHI::ShaderDesc fragmentShader = { RHI::ShaderType::Fragment, "Shaders/Shader_frag.spv", "main", RHI::ShaderLanguage::Spirv };

	m_pVertShader = m_pRhi->CreateShader(vertexShader);
	m_pFragShader = m_pRhi->CreateShader(fragmentShader);



	RHI::SamplerDesc samplerDesc = {};
	samplerDesc.SetAddressMode(RHI::AddressMode::Repeat);
	samplerDesc.SetFilter(RHI::FilterMode::Linear);
	samplerDesc.enableAnisotropy = false;
	samplerDesc.enableCompare = false;

	m_pSampler = m_pRhi->CreateSampler(samplerDesc);

	std::vector<Vertex> vertices = {
		//   position       normal        color        uv
		{ {-64,  64, 0} , {0, 0, 1} , {1, 0, 0, 1} , {0, 0} },
		{ {-64, -64, 0} , {0, 0, 1} , {0, 1, 0, 1} , {0, 0} },
		{ { 64,  64, 0} , {0, 0, 1} , {0, 0, 1, 1} , {0, 0} },
		{ { 64, -64, 0} , {0, 0, 1} , {1, 1, 1, 1} , {0, 0} },
	};

	m_pVertexBuffer = m_pRhi->CreateVertexBuffer(u32(vertices.size()), sizeof(Vertex), RHI::BufferFlags::Static);
	m_pVertexBuffer->Write(0, vertices.size() * sizeof(Vertex), vertices.data());


	std::vector<i16> indices = {
		0, 1, 2,
		2, 3, 1
	};

	m_pIndexBuffer = m_pRhi->CreateIndexBuffer(u32(indices.size()), RHI::IndexType::UShort, RHI::BufferFlags::Static);
	m_pIndexBuffer->Write(0, indices.size() * sizeof(i16), indices.data());


	m_pUniformBuffer = m_pRhi->CreateBuffer(RHI::BufferType::Uniform, sizeof(glm::mat4) * 3, RHI::BufferFlags::Dynamic);

	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(m_pWindow, &windowWidth, &windowHeight);
	f32 aspect = f32(windowWidth) / f32(windowHeight);
	m_Ubo.projMatrix = glm::ortho(-windowWidth / 2.f, windowWidth / 2.f, -windowHeight / 2.f, windowHeight / 2.f, 0.01f, 1000.f); //glm::perspective(60.f, aspect, 0.01f, 1000.f);
	m_Ubo.viewMatrix = glm::lookAtLH(glm::vec3(0, 0, -2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_Ubo.modelMatrix = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	m_pUniformBuffer->Write(0, sizeof(UBO), &m_Ubo);


	RHI::DescriptorSetManager* pDescriptorSetManager = m_pRhi->GetDescriptorSetManager();

	std::vector<RHI::DescriptorSetBinding> bindings;
	bindings.push_back({ RHI::DescriptorSetBindingType::Uniform, RHI::ShaderType::Vertex | RHI::ShaderType::Fragment, 1 });

	m_pDescriptorSetLayout = pDescriptorSetManager->CreateDescriptorSetLayout(bindings);

	m_pDescriptorSet = pDescriptorSetManager->CreateDescriptorSet(m_pDescriptorSetLayout);
	m_pDescriptorSet->Write(0, m_pUniformBuffer);



	RHI::CommandListManager* pCommandListManager = m_pRhi->GetCommandListManager();
	RHI::Queue* pQueue = m_pRhi->GetContext()->GetQueue(RHI::QueueType::Graphics);
	for (i32 i = 0; i < 3; ++i)
	{
		m_pCommandLists[i] = pCommandListManager->CreateCommandList(pQueue);
	}

	SizeDependCreate();

	return true;
}

void BasicScene::Update(f32 dt)
{
	Scene::Update(dt);
}

void BasicScene::Render(f32 dt)
{
	Scene::Render(dt);

	i32 index = m_pSwapChain->GetCurrentIndex();

	RHI::CommandList* pCommandList = m_pCommandLists[index];

	if (pCommandList->GetState() != RHI::CommandListState::Finished)
		pCommandList->Wait();

	pCommandList->Begin();

	pCommandList->BeginRenderPass(m_pRenderPass, m_pFrameBuffers[index]);

	pCommandList->BindPipeline(m_pPipeline);
	pCommandList->BindVertexBuffer(0, m_pVertexBuffer, 0);
	pCommandList->BindIndexBuffer(m_pIndexBuffer, 0, RHI::IndexType::UShort);
	pCommandList->BindDescriptorSets(0, m_pDescriptorSet);

	pCommandList->DrawIndexed(6);
	pCommandList->EndRenderPass();

	pCommandList->End();

	

	std::vector<RHI::Semaphore*> waitSemaphores = { m_pSwapChain->GetSignalSemaphore() };
	std::vector<RHI::PipelineStage> waitStages = { RHI::PipelineStage::TopOfPipe };
	std::vector<RHI::Semaphore*> signalSemaphores = { m_pSwapChain->GetWaitSemaphore() };
	pCommandList->Submit(waitSemaphores, waitStages, signalSemaphores);
}

void BasicScene::RenderGUI(f32 dt)
{
	Scene::RenderGUI(dt);
}

b8 BasicScene::Shutdown()
{
	RHI::CommandListManager* pCommandListManager = m_pRhi->GetCommandListManager();
	for (i32 i = 0; i < 3; ++i)
	{
		m_pCommandLists[i]->Wait();
		pCommandListManager->DestroyCommandList(m_pCommandLists[i]);
	}


	SizeDependDestroy(false);

	RHI::DescriptorSetManager* pDescriptorSetManager = m_pRhi->GetDescriptorSetManager();

	pDescriptorSetManager->DestroyDescriptorSet(m_pDescriptorSet);
	pDescriptorSetManager->DestroyDescriptorSetLayout(m_pDescriptorSetLayout);

	m_pRhi->DestroyBuffer(m_pVertexBuffer);
	m_pRhi->DestroyBuffer(m_pIndexBuffer);
	m_pRhi->DestroyBuffer(m_pUniformBuffer);

	m_pRhi->DestroySampler(m_pSampler);
	m_pRhi->DestroyShader(m_pVertShader);
	m_pRhi->DestroyShader(m_pFragShader);

	return Scene::Shutdown();
}

void BasicScene::OnWindowResize()
{
	SizeDependDestroy(true);
	SizeDependCreate();
}

void BasicScene::SizeDependCreate()
{
	if (!m_pSwapChain)
	{
		m_pSwapChain = m_pRhi->CreateSwapChain(m_pWindow, RHI::VSyncMode::Tripple);
	}

	// Render pass
	std::vector<RHI::RenderPassAttachment> attachments;

	RHI::RenderPassAttachment attachment = {};
	attachment.format = m_pSwapChain->GetCurrentRenderTarget()->GetTexture()->GetFormat();
	attachment.type = RHI::RenderTargetType::Presentable;
	attachment.samples = RHI::SampleCount::Sample1;
	attachment.storeOp = RHI::StoreOp::Store;
	attachment.loadOp = RHI::LoadOp::Clear;
	attachments.push_back(attachment);

	RHI::RenderPassAttachmentRef attachmentRef;
	attachmentRef.index = 0;
	attachmentRef.type = RHI::RenderTargetType::Presentable;

	RHI::SubRenderPass subpass;
	subpass.attachments.push_back(attachmentRef);

	attachment.format = PixelFormat(PixelFormatComponents::D32, PixelFormatTransform::SFLOAT);
	attachment.type = RHI::RenderTargetType::DepthStencil;
	attachment.samples = RHI::SampleCount::Sample1;
	attachment.storeOp = RHI::StoreOp::DontCare;
	attachment.loadOp = RHI::LoadOp::Clear;
	attachments.push_back(attachment);

	attachmentRef.index = 1;
	attachmentRef.type = RHI::RenderTargetType::DepthStencil;
	subpass.attachments.push_back(attachmentRef);

	std::vector<RHI::SubRenderPass> subpasses;
	subpasses.push_back(subpass);

	m_pRenderPass = m_pRhi->CreateRenderPass(attachments, subpasses);

	// Pipeline
	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(m_pWindow, &windowWidth, &windowHeight);
	RHI::GraphicsPipelineDesc pipelineDesc = {};
	pipelineDesc.viewport.x = 0.f;
	pipelineDesc.viewport.y = 0.f;
	pipelineDesc.viewport.width = f32(windowWidth);
	pipelineDesc.viewport.height = f32(windowHeight);
	pipelineDesc.viewport.minDepth = 0.f;
	pipelineDesc.viewport.maxDepth = 1.f;
	pipelineDesc.scissor.x = 0;
	pipelineDesc.scissor.y = 0;
	pipelineDesc.scissor.width = windowWidth;
	pipelineDesc.scissor.height = windowHeight;

	pipelineDesc.pVertexShader = m_pVertShader;
	pipelineDesc.pFragmentShader = m_pFragShader;

	// Vertex Layout
	RHI::InputDescriptor inputDesc;
	inputDesc.Append(RHI::InputElementDesc(RHI::InputSemantic::Position, 0, RHI::InputElementType::Float3, 0              , 0, 0));
	inputDesc.Append(RHI::InputElementDesc(RHI::InputSemantic::Normal  , 0, RHI::InputElementType::Float3, 3  * sizeof(f32), 0, 0));
	inputDesc.Append(RHI::InputElementDesc(RHI::InputSemantic::Color   , 0, RHI::InputElementType::Float4, 6  * sizeof(f32), 0, 0));
	inputDesc.Append(RHI::InputElementDesc(RHI::InputSemantic::TexCoord, 0, RHI::InputElementType::Float2, 10 * sizeof(f32), 0, 0));
	pipelineDesc.inputDescriptor = inputDesc;

	RHI::BlendAttachment blendAttachment = {};
	blendAttachment.components = RHI::ColorComponentMask::R | RHI::ColorComponentMask::G | RHI::ColorComponentMask::B | RHI::ColorComponentMask::A;
	blendAttachment.enable = false;
	pipelineDesc.blendState.attachments.push_back(blendAttachment);

	pipelineDesc.rasterizer.fillMode = RHI::FillMode::Solid;
	pipelineDesc.rasterizer.cullMode = RHI::CullMode::None;

	pipelineDesc.primitiveTopology = RHI::PrimitiveTopology::Triangle;

	pipelineDesc.pRenderPass = m_pRenderPass;

	pipelineDesc.descriptorSetLayouts.push_back(m_pDescriptorSet->GetLayout());

	RHI::DepthStencilDesc& depthStencil = pipelineDesc.depthStencil;
	depthStencil.enableDepthWrite = true;
	depthStencil.enableDepthTest = true;
	depthStencil.depthCompareOp = RHI::CompareOp::Greater;

	m_pPipeline = m_pRhi->CreatePipeline(pipelineDesc);

	// Framebuffers and depth stencils
	RHI::RenderTargetDesc depthDesc = {};
	depthDesc.type = RHI::RenderTargetType::DepthStencil;
	depthDesc.width = windowWidth;
	depthDesc.height = windowHeight;
	depthDesc.samples = RHI::SampleCount::Sample1;
	depthDesc.format = PixelFormat(PixelFormatComponents::D32, PixelFormatTransform::SFLOAT);

	for (i32 i = 0; i < 3; ++i)
	{
		RHI::RenderTarget* pColorRT = m_pSwapChain->GetRenderTarget(i);

		m_pDepthStencils[i] = m_pRhi->CreateRenderTarget(depthDesc);

		std::vector<RHI::RenderTarget*> pRTs = { pColorRT, m_pDepthStencils[i] };

		m_pFrameBuffers[i] = m_pRhi->CreateFramebuffer(pRTs, m_pRenderPass);
	}

}

void BasicScene::SizeDependDestroy(b8 destroySwapChain)
{

	for (i32 i = 0; i < 3; ++i)
	{
		m_pRhi->DestroyFramebuffer(m_pFrameBuffers[i]);
		m_pRhi->DestroyRenderTarget(m_pDepthStencils[i]);
	}

	m_pRhi->DestroyPipeline(m_pPipeline);
	m_pRhi->DestroyRenderPass(m_pRenderPass);

	if (destroySwapChain)
	{
		m_pRhi->DestroySwapChain(m_pSwapChain);
	}
}
