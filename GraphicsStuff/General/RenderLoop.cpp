#include "RenderLoop.h"

#include "../Vulkan/VulkanDynamicRHI.h"
#include "../RHI/Queue.h"

#include "../Scenes/BasicScene.h"

#include <GLFW/glfw3.h>


RenderLoop::RenderLoop()
	: m_pWindow(nullptr)
	, m_pRHI(nullptr)
{
}

RenderLoop::~RenderLoop()
{
}

i32 RenderLoop::Run()
{
	b8 res = Init();

	if (!res)
		return -1;

	while (!glfwWindowShouldClose(m_pWindow))
	{

		glfwPollEvents();

		m_pScene->Update(0.f);
		m_pScene->Render(0.f);
		m_pScene->RenderGUI(0.f);


		m_pScene->Present();
	}


	res = ShutDown();
	if (!res)
		return -1;


	return 0;
}

b8 RenderLoop::Init()
{
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_pWindow = glfwCreateWindow(m_Width, m_Height, "GraphicsStuff", nullptr, nullptr);
	if (!m_pWindow)
		return false;

	m_pRHI = new Vulkan::VulkanDynamicRHI();
	RHI::RHIDesc desc = {};

	RHI::QueueInfo queueInfo = {};
	queueInfo.type = RHI::QueueType::Graphics | RHI::QueueType::Transfer;
	queueInfo.priority = RHI::QueuePriority::High;
	queueInfo.count = 1;

	desc.queueInfo.push_back(queueInfo);

	desc.validationLevel = RHI::RHIValidationLevel::Warning;

	b8 res = m_pRHI->Init(desc, m_pWindow);
	if (!res)
		return false;

	m_pScene = new BasicScene();
	m_pScene->SetGeneral(m_pRHI, m_pWindow);
	res = m_pScene->Init();
	if (!res)
		return false;

	return true;
}

b8 RenderLoop::ShutDown()
{
	m_pScene->Shutdown();
	delete m_pScene;
	m_pScene = nullptr;

	m_pRHI->Destroy();
	delete m_pRHI;
	m_pRHI = nullptr;

	glfwDestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	glfwTerminate();

	return true;
}
