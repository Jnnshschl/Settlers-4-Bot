#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <format>

#include "S4Bot.hpp"

#include "utils/Hook.hpp"

#include "logging/AyyLog.hpp"

#include "sdk/Landscape/S4WorldTile.hpp"
#include "sdk/Entities/S4Entity.hpp"

#define KEY_PRESSED(x) (GetAsyncKeyState(x) & 0x8000)

typedef long(__stdcall* tEndScene) (IDirect3DDevice9*);

// # entry point and bot loop
unsigned long __stdcall S4BotEntry(void* hModule) noexcept;
void S4BotTick() noexcept;

void S4BotEventHandler(S4Event* s4Event, bool isNet = false) noexcept;

IDirect3DDevice9* GetD3D9DummyDevice() noexcept;
bool CreateD3D9DummyDevice(IDirect3D9* d3d, HWND windowHandle, IDirect3DDevice9** dummyDevice) noexcept;

// # debug ui stuff
static HWND DebugUiMainWindowHandle = nullptr;
static RECT DebugUiWindowRect;
static int DebugUiWindowWidth;
static int DebugUiWindowHeight;

static ID3DXLine* DefaultLine = nullptr;
static ID3DXFont* DefaultHeaderFont = nullptr;
static ID3DXFont* DefaultTextFont = nullptr;

constexpr auto DUI_DEFAULT_FONT = "Consolas";
constexpr D3DCOLOR DebugUiTextColor = 0xFFFFFFFF;
constexpr D3DCOLOR DebugUiBackgroundColor = 0xFF141414;
constexpr D3DCOLOR DebugUiBorderColor = 0xFF75D700;
constexpr D3DCOLOR DebugUiWhiteColor = 0xFFFFFFFF;
constexpr D3DCOLOR DebugUiRedColor = 0xFFFF0000;

static int DUI_PANEL_X = 300;
static int DUI_PANEL_Y = 32;
static int DUI_PANEL_WIDTH = 400;
static int DUI_PANEL_HEIGHT = 500;

static D3DRECT DebugUiPanelRect{};
static D3DXVECTOR2 DebugUiPanelBorderVectors[5];
static RECT DebugUiHeaderRect{};

void SetupDebugUi(IDirect3DDevice9* d3d9Device) noexcept;

// # hooks and gateway functions
static bool EndSceneShouldExit = false;
static bool EndSceneExited = false;

static Hook::VFuncHook<tEndScene>* HookEndScene = nullptr;
static long __stdcall GwEndScene(IDirect3DDevice9* d3ddevice) noexcept;

static Hook::DirectHook<tSendNetEvent>* HookSendNetEvent = nullptr;
static void __stdcall GwSendNetEvent(S4Event* s4Event) noexcept;

static Hook::DirectHook<tSendLocalEvent>* HookSendLocalEvent = nullptr;
static void __fastcall GwSendLocalEvent(void* eventEngine, void* unk, S4Event* s4Event) noexcept;

static Hook::DirectHook<tGenerateBuildPoints>* DirectHookGenerateBuildPoints = nullptr;
static unsigned int __stdcall GwGenerateBuildPoints(int playerId, int buildingId, int unk) noexcept;

// # inline helper functions
__forceinline bool IsOnScreen(int x, int y) noexcept
{
    return x >= 0 && y >= 0 && x <= DebugUiWindowWidth && y <= DebugUiWindowHeight;
}

/// <summary>
/// This helper is used to trigger stuff only once when a key 
/// gets pressed instead of spamming it.
/// </summary>
/// <param name="vKey">Key to observe</param>
/// <param name="switchVar">Static boolean</param>
/// <returns>True when the key got pressed, but only once</returns>
__forceinline bool OnKeyPressedOnce(unsigned char vKey, bool& sVar)
{
    if (KEY_PRESSED(vKey))
    {
        if (!sVar)
        {
            return sVar = true;
        }
    }
    else if (sVar)
    {
        sVar = false;
    }

    return false;
}
