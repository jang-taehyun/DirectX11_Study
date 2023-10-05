// DirectXProject.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DirectXProject.h"
#include "SystemClass.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    SystemClass* System = nullptr;
    bool result = false;

    /* System 객체 생성 */
    System = new SystemClass;
    if (!System)
    {
        return result;
    }

    /* System 객체 초기화 및 실행 */
    result = System->Initialize();
    if (result)
    {
        System->Run();
    }

    /* System 객체 종료 및 메모리 반환 */
    System->Shutdown();
    delete System;
    System = nullptr;

    return 0;
}
