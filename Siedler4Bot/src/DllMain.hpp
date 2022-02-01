#pragma once

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <format>
#include <chrono>
#include <deque>
#include <tuple>

#include "overlay/S4BotOverlay.hpp"

#include "utils/Hook.hpp"

#include "logging/AyyLog.hpp"

#include "sdk/S4Api.hpp"
#include "sdk/static/S4Event.hpp"
#include "sdk/landscape/S4WorldTile.hpp"
#include "sdk/entities/S4Entity.hpp"

#include "bot/IBotLogic.hpp"
#include "bot/logic/simple/RomanSimpleBotLogic.hpp"

// # entry point and bot loop
static S4Api* S4 = nullptr;

static IBotLogic* BotLogic = nullptr;
static S4BotOverlay* BotOverlay = nullptr;

static bool GameExited = false;

static bool EndSceneShouldExit = false;
static bool EndSceneExited = false;

unsigned long __stdcall S4BotEntry(void* hModule) noexcept;
void S4BotTick() noexcept;
void DumpDebugInfo() noexcept;
void S4BotEventHandler(S4Event* s4Event, bool isNet = false) noexcept;
IDirect3DDevice9* GetD3D9DummyDevice() noexcept;
bool CreateD3D9DummyDevice(IDirect3D9* d3d, HWND windowHandle, IDirect3DDevice9** dummyDevice) noexcept;

// # hooks and gateway functions
static Hook::VFuncHook<tEndScene>* HookEndScene = nullptr;
static Hook::DirectHook<tSendNetEvent>* HookSendNetEvent = nullptr;
static Hook::DirectHook<tSendLocalEvent>* HookSendLocalEvent = nullptr;
static Hook::DirectHook<tBuildingFinished>* HookBuildingFinished = nullptr;
static Hook::DirectHook<tBuildCheck>* HookBuildCheck = nullptr;

long __stdcall GwEndScene(IDirect3DDevice9* d3ddevice) noexcept;
void __stdcall GwSendNetEvent(S4Event* s4Event) noexcept;
void __fastcall GwSendLocalEvent(void* eventEngine, void* unk, S4Event* s4Event) noexcept;
void __fastcall GwBuildingFinished(int unk) noexcept;
int __stdcall GwBuildCheck(unsigned int x, unsigned int y, unsigned int playerId, int buildingId, int unk) noexcept;
