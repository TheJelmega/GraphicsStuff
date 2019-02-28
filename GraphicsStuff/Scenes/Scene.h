#pragma once
#include "../General/TypesAndMacros.h"

struct GLFWwindow;

namespace RHI {
	class SwapChain;
	class IDynamicRHI;
}

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual b8 Init();

	virtual void Update(f32 dt);
	virtual void Render(f32 dt);
	virtual void RenderGUI(f32 dt);

	virtual b8 Shutdown();

	virtual void OnWindowResize();

	void SetGeneral(RHI::IDynamicRHI* pRhi, GLFWwindow* pWindow);

	void Present();

protected:

	RHI::IDynamicRHI* m_pRhi;
	GLFWwindow* m_pWindow;

	RHI::SwapChain* m_pSwapChain;
};