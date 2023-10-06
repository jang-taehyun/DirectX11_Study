/*
System class

- 역할 : 윈도우 생성/제거, 프로그램 동작
*/

#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

/* 자주 사용하지 않는 WIN32 API header를 포함시키지 않게 하여 빌드 속도를 높인다. */
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
	bool Frame();																					// Application의 모든 작업 처리
	void InitializeWindows(int& ScreenWidth, int& ScreenHeight);									// 윈도우 등록 및 생성, 렌더링 화면 설정, 윈도우 표시
	void ShutdownWindows();																			// 화면 설정을 되돌리고 윈도우 리소스 반환

public:
	/* interface */
	bool Initialize();																				// Application의 모든 초기화 작업 수행
	void Shutdown();																				// System 객체 정리
	void Run();																						// Application의 모든 작업(Message, 입력 등)을 처리

	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);			// 윈도우 메세지 및 Application 작업 메세지 처리
};


/* 윈도우 프로시저 */
static LRESULT CALLBACK WndProc(HWND hWnd, UINT umessage, WPARAM wParam, LPARAM lParam);			// 윈도우 메세지 처리

/* System class 접근 포인터 */
static SystemClass* ApplicationHandler = nullptr;

#endif