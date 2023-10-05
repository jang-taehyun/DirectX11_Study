/*
Grphcis class

- 역할 : 렌더링 담당, 그래픽 관련 전역 설정 담당
*/

#ifndef  _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <windows.h>

/* Graphics 관련 전역 설정 */
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
	/* constructor & destructor */
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	/* method */
private:
	bool Render();

	/* interface */
public:
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
};

#endif