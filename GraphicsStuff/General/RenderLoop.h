#pragma once
#include "TypesAndMacros.h"
#include <SFML/Window/Window.hpp>

namespace RHI
{
	class IDynamicRHI;
}

class Scene;

class RenderLoop
{
public:
	RenderLoop();
	~RenderLoop();

	i32 Run();

private:
	b8 Init();
	b8 ShutDown();


	i32 m_Width = 1280;
	i32 m_Height = 720;

	sf::Window* m_pWindow;

	RHI::IDynamicRHI* m_pRHI;

	Scene* m_pScene;
};