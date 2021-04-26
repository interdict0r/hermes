#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detours.lib")

#include <Windows.h>
#include <iostream>
#include <string>
#include <Detours.h>
#include <imgui.h>
#include <d3d9.h>
#include <d3dx9.h> 
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

typedef HRESULT(_stdcall* EndScene)(LPDIRECT3DDEVICE9 pDevice);
HRESULT _stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice);
EndScene oEndScene;

LPDIRECT3DDEVICE9  g_pd3dDevice = NULL;
IDirect3D9*        g_pD3D = NULL;
HWND               window = NULL;

LRESULT WINAPI     WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC            wndproc_orig = NULL;

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) 
{
    DWORD wndProcID;
    GetWindowThreadProcessId(handle, &wndProcID);

    if (GetCurrentProcessId() != wndProcID) {
        return TRUE;
    }

    window = handle;
    return FALSE;
}

HWND GetProcessWindow()
{
    EnumWindows(EnumWindowsCallback, NULL);
    return window;
}

bool GetD3D9Device(void ** pTable, size_t size)
{
    if (!pTable) {
        return false;
    }

    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!g_pD3D) {
        return false;
    }

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = GetProcessWindow();
    d3dpp.Windowed = false;

    g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice);

    if(!g_pd3dDevice)
    {
        g_pD3D->Release();
        return false;
    }

    memcpy(pTable, *reinterpret_cast<void***>(g_pd3dDevice), size);
    g_pd3dDevice->Release();
    g_pD3D->Release();
    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }

    if (g_pD3D) {
        g_pD3D->Release();
        g_pD3D = NULL;
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

