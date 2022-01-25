#include "DllMain.hpp"
#include "utils/MathUtils.hpp"

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
	if (S4::Init())
	{
		AyyLog("S4Bot injected, press \"", LookupKeyName(S4::Keys::EXIT), "\" to eject");
		// S4Bot::Game::ShowTextMsg(">> S4Bot injected!", 8, 0, 0, 0);

		// hook all important methods
		if (auto dummyDevice = GetD3D9DummyDevice())
		{
			HookEndScene = new Hook::VFuncHook<tEndScene>(GwEndScene, dummyDevice, 42, 7);
		}

		HookSendNetEvent = new Hook::DirectHook<tSendNetEvent>(GwSendNetEvent, S4::Game::_SendNetEvent, 5, 0);
		HookSendLocalEvent = new Hook::DirectHook<tSendLocalEvent>(GwSendLocalEvent, S4::Game::_SendLocalEvent, 6, 0);

		// stuff used for debugging only, enabling them may cause a lot of spam logging
		// HookBuildCheck = new Hook::DirectHook<tBuildCheck>(GwBuildCheck, S4Bot::Game::_BuildCheck, 6, 0);

		AyyLog("PlayerId:            ", S4::Game::LocalPlayerId());
		AyyLog("Nation:              ", S4::Game::NationName());

		AyyLog("SendNetEvent:        0x", POINTER_AS_HEX(S4::Game::_SendNetEvent));
		AyyLog("SendLocalEvent:      0x", POINTER_AS_HEX(S4::Game::_SendLocalEvent));
		AyyLog("BuildCheck:          0x", POINTER_AS_HEX(S4::Game::_BuildCheck));

		// S4Bot::Game::BuildBuilding(S4Building::Stonequarry, 381, 180);
		// S4Bot::Game::MoveSettlers(new unsigned short[] { 412 }, 1, 381, 180);

		BotLogic = new RomanDebugBotLogic();

		// wait until we should eject
		while (!KEY_PRESSED(S4::Keys::EXIT) && !GameExited)
		{
			std::this_thread::sleep_for(S4::Config::MAIN_LOOP_SLEEP_TIME);
		}

		// wait for the EndScene to cleanup and exit
		EndSceneShouldExit = true;
		while (!EndSceneExited) std::this_thread::sleep_for(S4::Config::MAIN_LOOP_SLEEP_TIME);

		// cleanup hooks
		if (HookEndScene) delete HookEndScene;
		if (HookSendNetEvent) delete HookSendNetEvent;
		if (HookSendLocalEvent) delete HookSendLocalEvent;
		if (HookBuildCheck) delete HookBuildCheck;

		if (BotLogic) delete BotLogic;
	}
	else
	{
		AyyLog("S4Bot failed to init...");

		// wait until user ejects the bot as it failed to init
		while (!KEY_PRESSED(S4::Keys::EXIT))
		{
			std::this_thread::sleep_for(S4::Config::MAIN_LOOP_SLEEP_TIME);
		}
	}

	DBG_CLOSE_CONSOLE;
	ExitThread(0);
	return 0;
}

void S4BotTick() noexcept
{
	if (BotLogic)
	{
		BotLogic->Tick();
	}

	// used to trigger debug actions with a key
	if (static bool sVar = false; OnKeyPressedOnce(S4::Keys::DEBUG, sVar))
	{
		AyyLog("Executed Debug-Action at tick: ", S4::Game::CurrentTick());

		// search for stone spots and build a stonequarry        
		for (const auto& spot : S4::Game::FindResourceSpots(S4Resource::Stone1, S4Resource::Stone16))
		{
			S4::Game::BuildAtClosestSpot(S4Building::Stonequarry, spot.CenterPosition());
			break;
		}

		// search for forest and build a woodcutter
		for (const auto& spot : S4::Game::FindResourceSpots(S4Resource::Wood, S4Resource::Wood))
		{
			S4::Game::BuildAtClosestSpot(S4Building::Woodcutter, spot.CenterPosition());
			break;
		}

		// dump stuff for debug purposes
		std::ofstream landscapeFile;
		std::ofstream resourceFile;
		std::ofstream landownersFile;
		std::ofstream buildspotsFile;

		landscapeFile.open("s4bot-landscape-dump.txt");
		resourceFile.open("s4bot-resource-dump.txt");
		landownersFile.open("s4bot-landowners-dump.txt");
		buildspotsFile.open("s4bot-buildspots-dump.txt");

		const unsigned int mapSize = S4::Game::MapSize();

		for (unsigned short y = 0; y < mapSize; ++y)
		{
			for (unsigned short x = 0; x < mapSize; ++x)
			{
				const S4Position pos{ x, y };
				S4WorldTile* w = S4::Game::Landscape(pos);

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

				S4Resource r = S4::Game::Resource(pos);

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

				unsigned char p = S4::Game::LandscapeOwner(pos);
				landownersFile << p ? std::to_string(static_cast<int>(p)) : " ";

				if (p == S4::Game::LocalPlayerId())
				{
					S4BuildCheckResult canBuild = S4::Game::CanBuild(pos, S4Building::Stonequarry);
					buildspotsFile << std::to_string(p);
				}
				else
				{
					buildspotsFile << " ";
				}
			}

			landscapeFile << std::endl;
			resourceFile << std::endl;
			landownersFile << std::endl;
			buildspotsFile << std::endl;
		}

		landscapeFile.close();
		resourceFile.close();
		landownersFile.close();
		buildspotsFile.close();
	}
}

void S4BotEventHandler(S4Event* s4Event, bool isNet) noexcept
{
#define EVENT_LOG_PREFIX "[", std::format("{:06}", s4Event->Tick), "] ", isNet ? "  NET" : "LOCAL", " ", std::format("{:04}", static_cast<unsigned long>(s4Event->Id)), ": "

	switch (s4Event->Id)
	{
	case S4EventId::GameTick:
		S4BotTick();
		break;

	case S4EventId::SecondlyTick:
		break;

	case S4EventId::EnterBuildMode:
		AyyLog(EVENT_LOG_PREFIX, "EnterBuildMode(vtable: 0x", POINTER_AS_HEX(s4Event->VTable), ")");
		break;

	case S4EventId::SettlersMove:
		AyyLog(EVENT_LOG_PREFIX, "SettlersMove(player: ", static_cast<int>(s4Event->Player), ", x: ", s4Event->Param0AsPointer<unsigned short>()[0], ", y: ", s4Event->Param0AsPointer<unsigned short>()[1], ", count: ", s4Event->SettlersSize / sizeof(unsigned short), ")");

		for (size_t i = 0; i < s4Event->SettlersSize / sizeof(unsigned short); ++i)
		{
			AyyLog("                     -> Settler[", i, "]: id: ", s4Event->Settlers[i], " ptr: 0x", POINTER_AS_HEX(S4::Game::Entity(s4Event->Settlers[i])));
		}

		break;

	case S4EventId::BuildingBuild:
		AyyLog(EVENT_LOG_PREFIX, "BuildingBuild(player: ", static_cast<int>(s4Event->Player), ", type: ", s4Event->Params[0], ", x: ", s4Event->Param1AsPointer<unsigned short>()[0], ", y: ", s4Event->Param1AsPointer<unsigned short>()[1], ")");
		break;

	case S4EventId::MenuEnterSingleplayer:
		AyyLog(EVENT_LOG_PREFIX, "EnterMenuSinglePlayer(vtable: 0x", POINTER_AS_HEX(s4Event->VTable), ")");
		break;

	case S4EventId::MenuEnterMultiplayer:
		AyyLog(EVENT_LOG_PREFIX, "MenuEnterMultiplayer(vtable: 0x", POINTER_AS_HEX(s4Event->VTable), ")");
		break;

	case S4EventId::MenuExitGame:
		AyyLog(EVENT_LOG_PREFIX, "Game exited, ejecting...");
		GameExited = true;
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
		// disabled to avoid console spam
		// AyyLog(EVENT_LOG_PREFIX, "vtable: 0x", POINTER_AS_HEX(s4Event->VTable));
		break;
	}
}

IDirect3DDevice9* GetD3D9DummyDevice() noexcept
{
	const HWND windowHandle = S4::S4MainModule.Read<HWND>(S4::Offsets::MAINHWND);

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
	if (GetWindowRect(S4::S4MainModule.Read<HWND>(S4::Offsets::MAINHWND), &DebugUiWindowRect))
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

	if (static bool sVar = false; OnKeyPressedOnce(S4::Keys::DEBUG_UI, sVar))
	{
		// toggle debug window visibility
		renderDebugUi = !renderDebugUi;
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

int __stdcall GwBuildCheck(unsigned int x, unsigned int y, unsigned int playerId, int buildingId, int unk) noexcept
{
	auto ret = HookBuildCheck->Original(x, y, playerId, buildingId, unk);
	AyyLog("(x: ", x, ", y: ", y, ", playerId: ", playerId, ", buildingId: ", buildingId, ", unk: ", unk, ") = ", ret);
	return ret;
}
