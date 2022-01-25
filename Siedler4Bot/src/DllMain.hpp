#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <format>
#include <chrono>
#include <deque>
#include <tuple>

#include "S4Bot.hpp"

#include "utils/Hook.hpp"

#include "logging/AyyLog.hpp"

#include "sdk/Landscape/S4WorldTile.hpp"
#include "sdk/Entities/S4Entity.hpp"

#include "bot/IBotLogic.hpp"
#include "bot/RomanDebugBotLogic.hpp"

typedef long(__stdcall* tEndScene) (IDirect3DDevice9*);

// # entry point and bot loop
bool GameExited = false;
IBotLogic* BotLogic = nullptr;

unsigned long __stdcall S4BotEntry(void* hModule) noexcept;
void S4BotTick() noexcept;

void S4BotEventHandler(S4Event* s4Event, bool isNet = false) noexcept;

IDirect3DDevice9* GetD3D9DummyDevice() noexcept;
bool CreateD3D9DummyDevice(IDirect3D9* d3d, HWND windowHandle, IDirect3DDevice9** dummyDevice) noexcept;

// # debug ui stuff
HWND DebugUiMainWindowHandle = nullptr;
RECT DebugUiWindowRect;
int DebugUiWindowWidth;
int DebugUiWindowHeight;

ID3DXLine* DefaultLine = nullptr;
ID3DXFont* DefaultHeaderFont = nullptr;
ID3DXFont* DefaultTextFont = nullptr;

constexpr auto DUI_DEFAULT_FONT = "Consolas";
constexpr D3DCOLOR DebugUiTextColor = 0xFFFFFFFF;
constexpr D3DCOLOR DebugUiBackgroundColor = 0xFF141414;
constexpr D3DCOLOR DebugUiBorderColor = 0xFF75D700;
constexpr D3DCOLOR DebugUiWhiteColor = 0xFFFFFFFF;
constexpr D3DCOLOR DebugUiRedColor = 0xFFFF0000;

int DUI_PANEL_X = 300;
int DUI_PANEL_Y = 32;
int DUI_PANEL_WIDTH = 400;
int DUI_PANEL_HEIGHT = 500;

D3DRECT DebugUiPanelRect{};
D3DXVECTOR2 DebugUiPanelBorderVectors[5];
RECT DebugUiHeaderRect{};

int DebugPointX = 0;
int DebugPointY = 0;

void SetupDebugUi(IDirect3DDevice9* d3d9Device) noexcept;

// # hooks and gateway functions
bool EndSceneShouldExit = false;
bool EndSceneExited = false;

Hook::VFuncHook<tEndScene>* HookEndScene = nullptr;
long __stdcall GwEndScene(IDirect3DDevice9* d3ddevice) noexcept;

Hook::DirectHook<tSendNetEvent>* HookSendNetEvent = nullptr;
void __stdcall GwSendNetEvent(S4Event* s4Event) noexcept;

Hook::DirectHook<tSendLocalEvent>* HookSendLocalEvent = nullptr;
void __fastcall GwSendLocalEvent(void* eventEngine, void* unk, S4Event* s4Event) noexcept;

Hook::DirectHook<tBuildCheck>* HookBuildCheck = nullptr;
int __stdcall GwBuildCheck(unsigned int x, unsigned int y, unsigned int playerId, int buildingId, int unk) noexcept;

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
