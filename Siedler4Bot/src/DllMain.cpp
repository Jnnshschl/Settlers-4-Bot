#include "DllMain.hpp"

int __stdcall DllMain(HMODULE hModule, unsigned long dwReason, void* lpReserved) noexcept
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DBG_OPEN_CONSOLE;
        AyyLog("Injection successful");

        DisableThreadLibraryCalls(hModule);
        S4BotEntry(hModule);
    }

    return 1;
}

unsigned long __stdcall S4BotEntry(void* hModule) noexcept
{
    if (S4Bot::Init())
    {
        AyyLog("S4Bot Running, press \"", LookupKeyName(S4Bot::Keys::EXIT), "\" to eject");
        // S4Bot::Game::ShowTextMsg(">> S4Bot injected!", 8, 0, 0, 0);

        // hook all important methods
        if (auto dummyDevice = GetD3D9DummyDevice())
        {
            HookEndScene = new Hook::VFuncHook<tEndScene>(GwEndScene, dummyDevice, 42, 7);
            HookEndScene->Hook();
        }

        AyyLog("PlayerId:            ", S4Bot::Game::LocalPlayerId());
        AyyLog("Nation:              ", S4Bot::Game::NationName());
        AyyLog("TEST PTR:            0x", POINTER_AS_HEX(&S4Bot::Game::Landscape(285, 411)->Height));

        AyyLog("GenerateBuildPoints: 0x", POINTER_AS_HEX(S4Bot::Game::GenerateBuildPoints));
        AyyLog("SendNetEvent:        0x", POINTER_AS_HEX(S4Bot::Game::SendNetEvent));
        AyyLog("SendLocalEvent:      0x", POINTER_AS_HEX(S4Bot::Game::SendLocalEvent));

        HookSendNetEvent = new Hook::DirectHook<tSendNetEvent>(GwSendNetEvent, S4Bot::Game::SendNetEvent, 5, 0);
        HookSendNetEvent->Hook();

        HookSendLocalEvent = new Hook::DirectHook<tSendLocalEvent>(GwSendLocalEvent, S4Bot::Game::SendLocalEvent, 6, 0);
        HookSendLocalEvent->Hook();

        // DirectHookGenerateBuildPoints = new Hook::DirectHook<tGenerateBuildPoints>(GwGenerateBuildPoints, S4Bot::Game::GenerateBuildPoints, 6, 0);
        // DirectHookGenerateBuildPoints->Hook();

        // some test calls
        // S4Bot::Game::BuildBuilding(S4Building::BStonequarry, 381, 180);
        // S4Bot::Game::MoveSettlers(new unsigned short[] { 412 }, 1, 381, 180);

        while (!KEY_PRESSED(S4Bot::Keys::EXIT))
        {
            S4BotTick();
            std::this_thread::sleep_for(S4Bot::Config::MAIN_LOOP_SLEEP_TIME);
        }

        // wait for the EndScene to cleanup and exit
        EndSceneShouldExit = true;
        while (!EndSceneExited) std::this_thread::sleep_for(S4Bot::Config::MAIN_LOOP_SLEEP_TIME);

        // cleanup hooks
        delete HookEndScene;
        delete HookSendNetEvent;
        delete HookSendLocalEvent;
        // delete DirectHookGenerateBuildPoints;
    }
    else
    {
        AyyLog("S4Bot failed to init, exiting...");
    }

    DBG_CLOSE_CONSOLE;
    ExitThread(0);
    return 0;
}

void S4BotTick() noexcept
{
    // used to trigger debug actions with a key
    if (static bool sVar = false; OnKeyPressedOnce(S4Bot::Keys::DEBUG, sVar))
    {
        AyyLog("CurrentTick: ", S4Bot::Game::CurrentTick());

        // AyyLog("SelectionCount: ", S4Bot::Game::SelectionCount());
        // 
        // for (size_t i = 0; i < S4Bot::Game::SelectionCount(); ++i)
        // {
        //     AyyLog("-> id: ", S4Bot::Game::Selection()[i]);
        // }

        // dump stuff for debug purposes
        std::ofstream landscapeFile;
        landscapeFile.open("s4bot-landscape-dump.txt");

        std::ofstream resourceFile;
        resourceFile.open("s4bot-resource-dump.txt");

        unsigned int mapSize = S4Bot::Game::MapSize();

        for (unsigned short y = 0; y < mapSize; ++y)
        {
            for (unsigned short x = 0; x < mapSize; ++x)
            {
                S4WorldTile* w = S4Bot::Game::Landscape(x, y);

                if (w)
                {
                    if (w->Type >= S4GroundType::Water1 && w->Type <= S4GroundType::Water8) { landscapeFile << "~"; }
                    else if (w->Type == S4GroundType::Beach) { landscapeFile << "B"; }
                    else if (w->Type >= S4GroundType::Grass && w->Type <= S4GroundType::Grass_Mud) { landscapeFile << "G"; }
                    else if (w->Type == S4GroundType::Rock || w->Type == S4GroundType::Rock_Snow) { landscapeFile << "R"; }
                    else { landscapeFile << " "; }
                }
                else
                {
                    landscapeFile << " ";
                }

                S4Resource r = S4Bot::Game::Resource(x, y);

                if (r == S4Resource::None) { resourceFile << " "; }
                else if (r == S4Resource::Wood) { resourceFile << "W"; }
                else if (r >= S4Resource::Stone1 && r <= S4Resource::Stone16) { resourceFile << "S"; }
                else if (r >= S4Resource::Coal1 && r <= S4Resource::Coal16) { resourceFile << "C"; }
                else if (r >= S4Resource::Fish1 && r <= S4Resource::Fish16) { resourceFile << "F"; }
                else if (r >= S4Resource::Gold1 && r <= S4Resource::Gold16) { resourceFile << "G"; }
                else if (r >= S4Resource::Iron1 && r <= S4Resource::Iron16) { resourceFile << "I"; }
                else if (r >= S4Resource::Stonemine1 && r <= S4Resource::Stonemine16) { resourceFile << "M"; }
                else if (r >= S4Resource::Sulphur1 && r <= S4Resource::Sulphur16) { resourceFile << "U"; }
                else { resourceFile << "!"; }
            }

            landscapeFile << std::endl;
            resourceFile << std::endl;
        }

        landscapeFile.close();
        resourceFile.close();
    }
}

void S4BotEventHandler(S4Event* s4Event, bool isNet) noexcept
{
#define EVENT_LOG_PREFIX "[", std::format("{:06}", s4Event->Tick), "] ", isNet ? "  NET" : "LOCAL", " ", std::format("{:04}", static_cast<unsigned long>(s4Event->Id)), ": "

    switch (s4Event->Id)
    {
    case S4EventId::GameTick:
        break;

    case S4EventId::SecondlyTick:
        break;

    case S4EventId::EnterBuildMode:
        AyyLog(EVENT_LOG_PREFIX, "ENTER_BUILD_MODE vtable: 0x", POINTER_AS_HEX(s4Event->VTable));
        break;

    case S4EventId::SettlersMove:
        AyyLog(EVENT_LOG_PREFIX, "SETTLER_MOVE(player: ", static_cast<int>(s4Event->Player), ", x: ", s4Event->P0UShort()[0], ", y: ", s4Event->P0UShort()[1], ", count: ", s4Event->SettlersSize / sizeof(unsigned short), ")");

        for (size_t i = 0; i < s4Event->SettlersSize / sizeof(unsigned short); ++i)
        {
            AyyLog("                     -> Settler[", i, "]: id: ", s4Event->Settlers[i], " ptr: 0x", POINTER_AS_HEX(S4Bot::Game::Entity(s4Event->Settlers[i])));
        }
        break;

    case S4EventId::BuildingBuild:
        AyyLog(EVENT_LOG_PREFIX, "BUILDING_BUILD(player: ", static_cast<int>(s4Event->Player), ", type: ", s4Event->Params[0], ", x: ", s4Event->P1UShort()[0], ", y: ", s4Event->P1UShort()[1], ")");
        break;


    case S4EventId::None:
    case S4EventId::MouseMovement:
    case S4EventId::MouseMovementWindow:
    case S4EventId::MouseLeftDown:
    case S4EventId::MouseLeftUp:
    case S4EventId::MouseRightDown:
    case S4EventId::MouseRightUp:
    case S4EventId::WindowFocusChanged:
        // ignored events
        break;

    default:
        AyyLog(EVENT_LOG_PREFIX, "vtable: 0x", POINTER_AS_HEX(s4Event->VTable));
        break;
    }
}

IDirect3DDevice9* GetD3D9DummyDevice() noexcept
{
    const HWND windowHandle = S4Bot::S4MainModule.Read<HWND>(S4Bot::Offsets::MAINHWND);

    if (!windowHandle)
    {
        AyyLog("Failed to get window handle");
        return nullptr;
    }

    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
    {
        AyyLog("Failed to create Direct3D9 interface");
        return nullptr;
    }

    IDirect3DDevice9* dummyDevice = nullptr;
    const auto createdDummyDevice = CreateD3D9DummyDevice(d3d, windowHandle, &dummyDevice);
    d3d->Release();

    if (!createdDummyDevice)
    {
        AyyLog("Failed to hook EndScene");
        return nullptr;
    }

    return dummyDevice;
}

bool CreateD3D9DummyDevice(IDirect3D9* d3d, HWND windowHandle, IDirect3DDevice9** dummyDevice) noexcept
{
    D3DPRESENT_PARAMETERS d3dpp{};
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = windowHandle;

    if (auto result = d3d->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, 0x20L, &d3dpp, dummyDevice) != S_OK)
    {
        d3dpp.Windowed = true;

        if (result = d3d->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, 0x20L, &d3dpp, dummyDevice) != S_OK)
        {
            AyyLog("Failed to create dummy device, unable to hook EndScene");
            return false;
        }
    }

    return true;
}

void SetupDebugUi(IDirect3DDevice9* d3d9Device) noexcept
{
    D3DXCreateLine(d3d9Device, &DefaultLine);
    DefaultLine->SetWidth(1);

    D3DXCreateFontA(d3d9Device, 24, 0, FW_DONTCARE, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DUI_DEFAULT_FONT, &DefaultHeaderFont);
    D3DXCreateFontA(d3d9Device, 14, 0, FW_DONTCARE, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DUI_DEFAULT_FONT, &DefaultTextFont);

    // align debug ui in the upper right corner if possible
    if (GetWindowRect(S4Bot::S4MainModule.Read<HWND>(S4Bot::Offsets::MAINHWND), &DebugUiWindowRect))
    {
        DebugUiWindowWidth = DebugUiWindowRect.right - DebugUiWindowRect.left;
        DebugUiWindowHeight = DebugUiWindowRect.bottom - DebugUiWindowRect.top;

        DUI_PANEL_X = DebugUiWindowWidth - DUI_PANEL_WIDTH - 16;
        DUI_PANEL_Y = 0; // to align at bottom: DebugUiWindowHeight - DUI_PANEL_HEIGHT - 1
    }

    DebugUiPanelRect =
    {
        DUI_PANEL_X,
        DUI_PANEL_Y,
        DUI_PANEL_X + DUI_PANEL_WIDTH,
        DUI_PANEL_Y + DUI_PANEL_HEIGHT
    };

    DebugUiPanelBorderVectors[0] = D3DXVECTOR2(DUI_PANEL_X, DUI_PANEL_Y);
    DebugUiPanelBorderVectors[1] = D3DXVECTOR2(DUI_PANEL_X + DUI_PANEL_WIDTH, DUI_PANEL_Y);
    DebugUiPanelBorderVectors[2] = D3DXVECTOR2(DUI_PANEL_X + DUI_PANEL_WIDTH, DUI_PANEL_Y + DUI_PANEL_HEIGHT);
    DebugUiPanelBorderVectors[3] = D3DXVECTOR2(DUI_PANEL_X, DUI_PANEL_Y + DUI_PANEL_HEIGHT);
    DebugUiPanelBorderVectors[4] = D3DXVECTOR2(DUI_PANEL_X, DUI_PANEL_Y);

    DebugUiHeaderRect =
    {
        DUI_PANEL_X + 4,
        DUI_PANEL_Y + 2,
        DUI_PANEL_X + DUI_PANEL_WIDTH,
        DUI_PANEL_Y + DUI_PANEL_HEIGHT
    };
}

long __stdcall GwEndScene(IDirect3DDevice9* d3d9Device) noexcept
{
    if (EndSceneExited) return HookEndScene->Original(d3d9Device);

    static bool debugUiSetup = false;

    if (!debugUiSetup)
    {
        // only run the setup once
        debugUiSetup = true;
        SetupDebugUi(d3d9Device);
    }

    static bool renderDebugUi = false;

    if (static bool sVar = false; OnKeyPressedOnce(S4Bot::Keys::DEBUG_UI, sVar))
    {
        // toggle debug window visibility
        renderDebugUi = !renderDebugUi;
    }

    unsigned int sX, sY;
    S4Bot::Game::WorldToScreen(285, 411, &sX, &sY);

    if (IsOnScreen(sX, sY))
    {
        D3DRECT DebugUiTestRect;
        DebugUiTestRect.x1 = sX - 4;
        DebugUiTestRect.y1 = sY - 4;
        DebugUiTestRect.x2 = sX + 4;
        DebugUiTestRect.y2 = sY + 4;

        d3d9Device->Clear(1, &DebugUiTestRect, D3DCLEAR_TARGET, DebugUiWhiteColor, 0, 0);
    }

    if (renderDebugUi)
    {
        d3d9Device->Clear(1, &DebugUiPanelRect, D3DCLEAR_TARGET, DebugUiBackgroundColor, 0, 0);
        DefaultHeaderFont->DrawTextA(0, "S4Bot", -1, &DebugUiHeaderRect, DT_LEFT | DT_NOCLIP, 0xFFFFFFFF);
        DefaultLine->Draw(DebugUiPanelBorderVectors, 5, DebugUiBorderColor);

    }

    if (EndSceneShouldExit && debugUiSetup)
    {
        // need to release our DirectX stuff and signal that we are ready to unhook
        DefaultLine->Release();
        DefaultHeaderFont->Release();
        DefaultTextFont->Release();
        EndSceneExited = true;
    }

    return HookEndScene->Original(d3d9Device);
}

void __stdcall GwSendNetEvent(S4Event* s4Event) noexcept
{
    S4BotEventHandler(s4Event, true);
    HookSendNetEvent->Original(s4Event);
}

void __fastcall GwSendLocalEvent(void* eventEngine, void* unk, S4Event* s4Event) noexcept
{
    S4BotEventHandler(s4Event);
    HookSendLocalEvent->Original(eventEngine, unk, s4Event);
}

unsigned int __stdcall GwGenerateBuildPoints(int playerId, int buildingId, int unk) noexcept
{
    unsigned int retVal = DirectHookGenerateBuildPoints->Original(playerId, buildingId, unk);
    AyyLog("(playerId: ", playerId, ", buildingId: ", buildingId, ", unk: ", unk, ") -> ", retVal);
    return retVal;
}
