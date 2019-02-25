#include "Scene.h"
#include "../RHI/IDynamicRHI.h"
#include "../RHI/SwapChain.h"
#include <SFML/Window/Window.hpp>

Scene::Scene()
	: m_pRhi(nullptr)
{
}

Scene::~Scene()
{
}

b8 Scene::Init()
{
	m_pSwapChain = m_pRhi->CreateSwapChain(m_pWindow, RHI::VSyncMode::Tripple);


	return true;
}

void Scene::Update(f32 dt)
{
}

void Scene::Render(f32 dt)
{
}

void Scene::RenderGUI(f32 dt)
{
}

b8 Scene::Shutdown()
{
	m_pRhi->DestroySwapChain(m_pSwapChain);

	return true;
}

void Scene::OnWindowResize()
{
}

void Scene::SetGeneral(RHI::IDynamicRHI* pRhi, sf::Window* pWindow)
{
	m_pRhi = pRhi;
	m_pWindow = pWindow;
}

void Scene::Present()
{
	if (m_pSwapChain)
		m_pSwapChain->Present();
	m_pWindow->display();
}
