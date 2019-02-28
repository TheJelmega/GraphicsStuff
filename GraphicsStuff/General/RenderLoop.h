#pragma once
#include "TypesAndMacros.h"

namespace RHI
{
	class IDynamicRHI;
}

class Scene;
struct GLFWwindow;

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

	GLFWwindow* m_pWindow;

	RHI::IDynamicRHI* m_pRHI;

	Scene* m_pScene;
};