#pragma once
#include "../General/TypesAndMacros.h"

namespace sf {
	class Window;
}

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

	void SetGeneral(RHI::IDynamicRHI* pRhi, sf::Window* pWindow);

	void Present();

protected:

	RHI::IDynamicRHI* m_pRhi;
	sf::Window* m_pWindow;

	RHI::SwapChain* m_pSwapChain;
};