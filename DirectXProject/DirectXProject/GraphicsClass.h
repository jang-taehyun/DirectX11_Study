/*
Grphcis class

- 역할 : 렌더링 담당, 그래픽 관련 전역 설정 담당
*/

#ifndef  _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "D3DClass.h"

/* Graphics 관련 전역 설정 */
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
	/* member varialbe */
private:
	D3DClass* m_D3D;

	/* constructor & destructor */
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& ref);
	~GraphicsClass();

	/* method */
private:
	bool Render();																// Rendering 수행

	/* interface */
public:
	bool Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd);				// graphics 관련 객체 초기화
	void Shutdown();															// grphaics 관련 객체 해제
	bool Frame();																// 1 frame 내에서 일어나는 모든 작업 수행

	D3DClass* GetDirect3DObject();												// D3D Class 반환
};

#endif