#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include "Application.h"

#include <iostream>


#include <thread>
#include <chrono>

#include "Hw/Camera.h"
#include "Utility/StateMachine.h"

extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}


void AttachConsoleWindow()
{
    AllocConsole();
    FILE* fpOut;
    FILE* fpErr;
    FILE* fpIn;
    freopen_s(&fpOut, "CONOUT$", "w", stdout);
    freopen_s(&fpErr, "CONOUT$", "w", stderr);
    freopen_s(&fpIn, "CONIN$", "r", stdin);
    std::ios::sync_with_stdio();
}
// entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    AttachConsoleWindow();
    std::cout << "Starting DirectX 11 Application..." << std::endl;
	/*
    Application app(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    app.Run();
    */
    return 0;
}

