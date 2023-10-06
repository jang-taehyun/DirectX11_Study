/*
Grphcis class

- ���� : ������ ���, �׷��� ���� ���� ���� ���
*/

#ifndef  _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "D3DClass.h"

/* Graphics ���� ���� ���� */
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
	bool Render();																// Rendering ����

	/* interface */
public:
	bool Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd);				// graphics ���� ��ü �ʱ�ȭ
	void Shutdown();															// grphaics ���� ��ü ����
	bool Frame();																// 1 frame ������ �Ͼ�� ��� �۾� ����

	D3DClass* GetDirect3DObject();												// D3D Class ��ȯ
};

#endif