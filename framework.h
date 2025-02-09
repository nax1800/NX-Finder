#pragma once

#include <windows.h>
#include <iostream>
#include <fstream>
#include <format>
#include <d3d11.h>
#include <dxgi.h>
#include <filesystem>


#include "memcury.h"
#include "MinHook/include/MinHook.h"
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "SDK.h"

using namespace UE4;
using namespace std;

HANDLE GetConsoleHandle() { return GetStdHandle(STD_OUTPUT_HANDLE); }

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define LogInfo(...) { SetConsoleTextAttribute(GetConsoleHandle(), 10); cout << "NX-Finder:Info: " << format(__VA_ARGS__) << endl; SetConsoleTextAttribute(GetConsoleHandle(), 7); }
#define LogError(...) { SetConsoleTextAttribute(GetConsoleHandle(), 12); cout <<  "NX-Finder:Error: " << format(__VA_ARGS__) << endl; SetConsoleTextAttribute(GetConsoleHandle(), 7); }
#define LogWarning(...) { SetConsoleTextAttribute(GetConsoleHandle(), 14); cout <<  "NX-Finder:Warning: " << format(__VA_ARGS__) << endl; SetConsoleTextAttribute(GetConsoleHandle(), 7); }

#include "OffsetFinder.h"

#include "Globals.h"

#include "Overlay.h"
