// dllmain.cpp : Define o ponto de entrada para o aplicativo DLL.
#include <iostream>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")
#include <hack.cpp>
#include <cmath>
#include "offsets.h"
#include "vector3.h"
#include "draw.h"
#include "esp.h"
#include "aim.h"
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
#define PI 3.1415926535897
//directx hook
HWND window;
int width = 1024;
int height = 768;
bool snapLine = false, bEsp = false,bMenu=true,baim=false;
LPDIRECT3DDEVICE9 pDev_;

typedef HRESULT(APIENTRY* tEnd)(LPDIRECT3DDEVICE9 pDev);
tEnd OriginalEnd = nullptr;
Esp* esp = new Esp();
Vec3 menuSize = { 250,300 };
char menuName[] = "CheetahChug";
BOOL CALLBACK enumwind(HWND handle, LPARAM lp) {
    DWORD PID;
    GetWindowThreadProcessId(handle, &PID);
    if (GetCurrentProcessId() != PID) return TRUE;
    window = handle;
    return FALSE;
}
HWND getprocesswindow() {
    EnumWindows(enumwind, NULL);
    RECT size;
    GetWindowRect(window, &size);
    //width = size.right - size.left;
    //windowHeight = size.bottom - size.top;
    return window;
}
bool getDirectxDevice(void** pTable, size_t size) {
    if (!pTable) return false;
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D) return false;
    IDirect3DDevice9* dummyDvc = nullptr;
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = false;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = getprocesswindow();
    HRESULT dummy = pD3D->CreateDevice(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDvc);
    if (dummy != S_OK) {
        d3dpp.Windowed = !d3dpp.Windowed;
        HRESULT dummy = pD3D->CreateDevice(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDvc);
        if (dummy != S_OK) {
            pD3D->Release();
            return false;
        }
    }
    memcpy(pTable, *(void***)(dummyDvc), size);
    dummyDvc->Release();
    pD3D->Release();
    return true;
}

DWORD color;
void drawtext(const char* txt, float x, float y, D3DCOLOR color) {
    RECT recta;
    if (!esp->font) {
        D3DXCreateFont(pDev_, 25, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial", &esp->font);
    }
    SetRect(&recta, x + 1, y + 1, x + 1, y + 1);
    esp->font->DrawTextA(0, txt, -1, &recta, DT_CENTER | DT_NOCLIP, BLACK(255));
    SetRect(&recta, x + 1, y + 1, x + 1, y + 1);
    esp->font->DrawTextA(0, txt, -1, &recta, DT_CENTER | DT_NOCLIP, color);
}
VOID APIENTRY hooked_directx(LPDIRECT3DDEVICE9 o_pDev) {
    if (!pDev_)  pDev_ = o_pDev;
    CDraw draw;
    draw.GetDevice(pDev_);
    //draw cross hair
    draw.Circle(width / 2, height / 2, 5, 1, full, false, 10, WHITE(255));
    draw.CircleFilled(width / 2, height / 2, 1.5, 1, full, 10, WHITE(255));
    if (snapLine) {
        for (int i = 0; i < 64; i++) {
            ent* entity = esp->entlist->entlist[i].ent;
            if (i == 0) continue;
            if (!esp->isValid(entity)) {
                continue;
            }
            if (entity->team == esp->localPlayer->team) {
                color = YELLOW(255);
            }
            else {
                color = RED(255);
            }
            Vec3 ent_pos;
            if (esp->Wrld2Screen(entity->m_vecOrigin, ent_pos, width, height)) {
                draw.Line(ent_pos.x, ent_pos.y, width / 2 - 70, height, 1, true, color);
            }

        }
    }
    if (bEsp) {
        for (int i = 0; i < 64; i++) {
            Vec3 head;
            ent* entity = esp->entlist->entlist[i].ent;
            if (i ==0) continue;
            if (!esp->isValid(entity)) {
                continue;
            }
            if (entity->team == esp->localPlayer->team) {
                color = YELLOW(255);
            }
            else {
                color = RED(255);
            }
            Vec3 ent_pos;
            if (esp->Wrld2Screen(entity->m_vecOrigin, ent_pos, width, height)) {

                if (esp->Wrld2Screen(esp->bonePos(entity, 6), head, width, height)) {
                    float Height = abs(head.y - ent_pos.y);
                    float Width = Height / 2;
                    ent_pos.x -= Width / 2;

                    draw.Box(ent_pos.x, ent_pos.y - Height, Width, Height, 1.3f, D3DCOLOR_ARGB(255, 255, 0, 0));
                }
            }
        }
    }

    OriginalEnd(pDev_);
}



DWORD WINAPI mainThread(HMODULE mod) {
    //AllocConsole();
    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w", stdout);
    //-----------------------------------------------
    //  dxd9  hook
    bool selected;
    float smoothing = 4.0f;
    void* DxDvc[119];
    BYTE EndSceneByte[7]{ 0 };
    if (getDirectxDevice(DxDvc, sizeof(DxDvc))) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        //SetConsoleTextAttribute(hConsole, 10);
       // std::cout << "[+] DXD9 Hooked!\n";
        //SetConsoleTextAttribute(hConsole, 7);
        memcpy(EndSceneByte, (char*)DxDvc[42], 7);
        OriginalEnd = (tEnd)detours::TrampHook32((char*)(DxDvc[42]), (char*)hooked_directx, 7);
    }
    //-----------------------------------------------
    while (!GetAsyncKeyState(VK_F10) & 1) {
        esp->update();
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            snapLine = !snapLine;
        }
        if (GetAsyncKeyState(VK_HOME) & 1) {
            bEsp = !bEsp;
        }
        if (GetAsyncKeyState(VK_DELETE) & 1) {
            baim = !baim;
        }
        if (GetAsyncKeyState(VK_END) & 1) {
            bMenu = !bMenu;
        }
        if (baim) {
            for (int i = 0; i < 64; i++) {
                ent* entity = esp->entlist->entlist[i].ent;
                if (entity == esp->localPlayer) continue;
                if (!esp->isValid(entity)) {
                    continue;
                }
                *esp->vecview = calcAngle(esp->localPlayer->m_vecOrigin, esp->bonePos(entity, 6));
            }
        }
        Sleep(1);
    }

    Sleep(1000);
    //unhook directx;
    bEsp = false;
    snapLine = false;
    DWORD oldprotect;
    VirtualProtect((BYTE*)DxDvc[42], 7, PAGE_EXECUTE_READWRITE, &oldprotect);
    memcpy((BYTE*)DxDvc[42], EndSceneByte, 7);
    VirtualProtect((BYTE*)DxDvc[42], 7, oldprotect, &oldprotect);
    Sleep(1.2);
   // fclose(f);
   // FreeConsole();
    FreeLibraryAndExitThread(mod, 0);
    return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)mainThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

