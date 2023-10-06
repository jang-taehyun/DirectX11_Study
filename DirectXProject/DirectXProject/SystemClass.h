/*
System class

- ���� : ������ ����/����, ���α׷� ����
*/

#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

/* ���� ������� �ʴ� WIN32 API header�� ���Խ�Ű�� �ʰ� �Ͽ� ���� �ӵ��� ���δ�. */
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include "GraphicsClass.h"
#include "InputClass.h"

class SystemClass
{
private:
	/* member variable */
	LPCWSTR m_ApplicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;

public:
	/* constructor & destructor */
	SystemClass();
	SystemClass(const SystemClass& ref);
	~SystemClass();

private:
	/* method */
	bool Frame();																					// Application�� ��� �۾� ó��
	void InitializeWindows(int& ScreenWidth, int& ScreenHeight);									// ������ ��� �� ����, ������ ȭ�� ����, ������ ǥ��
	void ShutdownWindows();																			// ȭ�� ������ �ǵ����� ������ ���ҽ� ��ȯ

public:
	/* interface */
	bool Initialize();																				// Application�� ��� �ʱ�ȭ �۾� ����
	void Shutdown();																				// System ��ü ����
	void Run();																						// Application�� ��� �۾�(Message, �Է� ��)�� ó��

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);			// ������ �޼��� �� Application �۾� �޼��� ó��
};


/* ������ ���ν��� */
static LRESULT CALLBACK WndProc(HWND hWnd, UINT umessage, WPARAM wParam, LPARAM lParam);			// ������ �޼��� ó��

/* System class ���� ������ */
static SystemClass* ApplicationHandler = nullptr;

#endif