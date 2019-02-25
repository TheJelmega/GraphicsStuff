#include "RenderLoop.h"

#include <SFML/Window/Event.hpp>

#include "../Vulkan/VulkanDynamicRHI.h"
#include "../RHI/Queue.h"

#include "../Scenes/TestScene.h"


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

	while (m_pWindow->isOpen())
	{
		sf::Event event;
		while (m_pWindow->pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				m_pWindow->close();
				break;
			case sf::Event::Resized:
				m_pScene->OnWindowResize();
				break;
			case sf::Event::LostFocus: break;
			case sf::Event::GainedFocus: break;
			case sf::Event::TextEntered: break;
			case sf::Event::KeyPressed: break;
			case sf::Event::KeyReleased: break;
			case sf::Event::MouseWheelMoved: break;
			case sf::Event::MouseWheelScrolled: break;
			case sf::Event::MouseButtonPressed: break;
			case sf::Event::MouseButtonReleased: break;
			case sf::Event::MouseMoved: break;
			case sf::Event::MouseEntered: break;
			case sf::Event::MouseLeft: break;
			case sf::Event::JoystickButtonPressed: break;
			case sf::Event::JoystickButtonReleased: break;
			case sf::Event::JoystickMoved: break;
			case sf::Event::JoystickConnected: break;
			case sf::Event::JoystickDisconnected: break;
			case sf::Event::TouchBegan: break;
			case sf::Event::TouchMoved: break;
			case sf::Event::TouchEnded: break;
			case sf::Event::SensorChanged: break;
			default: ;
			}
		}

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
	sf::ContextSettings contextSettings = sf::ContextSettings(0, 0, 0, 0, 0, sf::ContextSettings::Core, false);
	m_pWindow = new sf::Window(sf::VideoMode(m_Width, m_Height), "Graphics Stuff", sf::Style::Default, contextSettings);
	if (!m_pWindow)
		return false;

	m_pRHI = new Vulkan::VulkanDynamicRHI();
	RHI::RHIDesc desc = {};

	RHI::QueueInfo queueInfo = {};
	queueInfo.type = RHI::QueueType::Graphics;
	queueInfo.priority = RHI::QueuePriority::High;
	queueInfo.count = 1;

	desc.queueInfo.push_back(queueInfo);

	desc.validationLevel = RHI::RHIValidationLevel::Warning;

	b8 res = m_pRHI->Init(desc, m_pWindow);
	if (!res)
		return false;

	m_pScene = new TestScene();
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

	m_pWindow->close();
	delete m_pWindow;
	m_pWindow = nullptr;

	return true;
}
