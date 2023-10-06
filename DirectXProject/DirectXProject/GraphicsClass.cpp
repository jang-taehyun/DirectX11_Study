#include "GraphicsClass.h"

GraphicsClass::GraphicsClass() : m_D3D(nullptr)
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& ref) : m_D3D(nullptr)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Render()
{
	/* buffer Ŭ���� */
	m_D3D->BeginScene(1.f, 1.f, 0.f, 0.5f);

	/* buffer�� �׷��� ������ ȭ�鿡 ǥ�� */
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd)
{
	bool result = false;

	/* Direct 3D ��ü ���� */
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return result;
	}

	/* Direct 3D ��ü �ʱ�ȭ */
	result = m_D3D->Initialize(ScreenWidth, ScreenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
	}

	return result;
}

void GraphicsClass::Shutdown()
{
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	bool result = false;

	/* Rendering ���� �۾� ���� */


	/* Rendering  ���� */
	result = Render();

	return result;
}

D3DClass* GraphicsClass::GetDirect3DObject()
{
	return m_D3D;
}
