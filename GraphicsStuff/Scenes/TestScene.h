#pragma once
#include "Scene.h"
#include "../RHI/Buffer.h"

#include <glm/glm.hpp>
#include "../RHI/DescriptorSetLayout.h"

namespace RHI {
	class Framebuffer;
	class Pipeline;
	class RenderPass;
	class DescriptorSet;
	class RenderTarget;
	class Sampler;
	class Shader;
}

class TestScene : public Scene
{
private:
	struct UBO
	{
		glm::mat4 projMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 modelMatrix;
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec2 uv;
	};

public:
	TestScene();
	~TestScene();


	b8 Init() override;
	void Update(f32 dt) override;
	void Render(f32 dt) override;
	void RenderGUI(f32 dt) override;
	b8 Shutdown() override;
	void OnWindowResize() override;

private:
	void SizeDependCreate();
	void SizeDependDestroy(b8 destroySwapChain);

	RHI::Shader* m_pVertShader;
	RHI::Shader* m_pFragShader;

	RHI::Sampler* m_pSampler;

	RHI::Buffer* m_pVertexBuffer;
	RHI::Buffer* m_pIndexBuffer;
	RHI::Buffer* m_pUniformBuffer;

	RHI::DescriptorSet* m_pDescriptorSet;
	RHI::DescriptorSetLayout* m_pDescriptorSetLayout;

	RHI::RenderPass* m_pRenderPass;
	RHI::Pipeline* m_pPipeline;

	RHI::CommandList* m_pCommandLists[3];
	RHI::Framebuffer* m_pFrameBuffers[3];
	RHI::RenderTarget* m_pDepthStencils[3];

	UBO m_Ubo;
};
