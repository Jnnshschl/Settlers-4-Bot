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
	S4 = new S4Api();

	if (S4->Init())
	{
		AyyLog("S4Bot injected, press \"", LookupKeyName(S4Bot::Keys::EXIT), "\" to eject");

		// hook all important methods
		if (auto dummyDevice = GetD3D9DummyDevice())
		{
			HookEndScene = new Hook::VFuncHook<tEndScene>(GwEndScene, dummyDevice, 42, 7);
		}
		else
		{
			// skip EndScene stuff when exiting
			EndSceneExited = true;
		}

		HookSendNetEvent = new Hook::DirectHook<tSendNetEvent>(GwSendNetEvent, S4->_SendNetEvent, 5, 0);
		HookSendLocalEvent = new Hook::DirectHook<tSendLocalEvent>(GwSendLocalEvent, S4->_SendLocalEvent, 6, 0);
		HookBuildingFinished = new Hook::DirectHook<tBuildingFinished>(GwBuildingFinished, S4->_BuildingFinished, 5, 0);

		// stuff used for debugging only, enabling them may cause a lot of spam logging
		// HookBuildCheck = new Hook::DirectHook<tBuildCheck>(GwBuildCheck, S4Bot::Game::_BuildCheck, 6, 0);

		AyyLog("PlayerId:            ", S4->LocalPlayerId());
		AyyLog("Nation:              ", S4->NationName());
		AyyLog("SendNetEvent:        0x", POINTER_AS_HEX(S4->_SendNetEvent));
		AyyLog("SendLocalEvent:      0x", POINTER_AS_HEX(S4->_SendLocalEvent));
		AyyLog("BuildingFinished:    0x", POINTER_AS_HEX(S4->_BuildingFinished));

		BotLogic = new RomanSimpleBotLogic(S4);

		// wait until we should eject
		while (!KEY_PRESSED(S4Bot::Keys::EXIT) && !GameExited)
		{
			std::this_thread::sleep_for(S4Bot::Config::MAIN_LOOP_SLEEP_TIME);
		}

		// cleanup hooks
		if (HookSendNetEvent) delete HookSendNetEvent;
		if (HookSendLocalEvent) delete HookSendLocalEvent;
		if (HookBuildingFinished) delete HookBuildingFinished;
		if (HookBuildCheck) delete HookBuildCheck;

		// wait for the EndScene to cleanup and exit
		EndSceneShouldExit = true;
		while (!EndSceneExited) std::this_thread::sleep_for(S4Bot::Config::MAIN_LOOP_SLEEP_TIME);

		if (HookEndScene) delete HookEndScene;

		if (BotLogic) delete BotLogic;
	}
	else
	{
		AyyLog("S4Bot failed to init...");
	}

	if (S4) delete S4;

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
	if (static bool sVar = false; OnKeyPressedOnce(S4Bot::Keys::DEBUG, sVar))
	{
		AyyLog("Executed Debug-Action at tick: ", S4->CurrentTick());

		Vector2 center = S4->CalculateSettlementCenter();
		AyyLog("X: ", center.X, " Y: ", center.Y);

		// DumpDebugInfo();
	}
}

void DumpDebugInfo() noexcept
{
	// dump stuff for debug purposes to the executeable folder
	std::ofstream landscapeFile;
	std::ofstream resourceFile;
	std::ofstream landownersFile;
	std::ofstream buildspotsFile;

	landscapeFile.open("s4bot-landscape-dump.txt");
	resourceFile.open("s4bot-resource-dump.txt");
	landownersFile.open("s4bot-landowners-dump.txt");
	buildspotsFile.open("s4bot-buildspots-dump.txt");

	for (unsigned short y = 0; y < S4->MapSize(); ++y)
	{
		for (unsigned short x = 0; x < S4->MapSize(); ++x)
		{
			const Vector2 pos{ x, y };
			S4WorldTile* w = S4->Landscape(pos);

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

			S4Resource r = S4->Resource(pos);

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

			unsigned char p = S4->LandscapeOwner(pos);
			const auto pstr = std::to_string(p);

			landownersFile << (p ? pstr : " ");

			if (p == S4->LocalPlayerId())
			{
				S4BuildCheckResult canBuild = S4->CanBuild(pos, S4Building::Stonequarry);
				buildspotsFile << pstr;
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
			AyyLog("                     -> Settler[", i, "]: id: ", s4Event->Settlers[i], " ptr: 0x", POINTER_AS_HEX(S4->Entity(s4Event->Settlers[i])));
		}

		break;

	case S4EventId::RecruitCivil:
		AyyLog(EVENT_LOG_PREFIX, "RecruitCivil(vtable: 0x", POINTER_AS_HEX(s4Event->VTable), ", ecosector: ", s4Event->Param0AsPointer<unsigned short>()[1], ")");
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
		AyyLog(EVENT_LOG_PREFIX, "vtable: 0x", POINTER_AS_HEX(s4Event->VTable));
		break;
	}
}

IDirect3DDevice9* GetD3D9DummyDevice() noexcept
{
	const HWND windowHandle = S4->MainModule.Read<HWND>(S4Bot::Offsets::MAINHWND);

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

	if (d3d->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, 0x20L, &d3dpp, dummyDevice) != S_OK)
	{
		d3dpp.Windowed = true;

		if (d3d->CreateDevice(0, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, 0x20L, &d3dpp, dummyDevice) != S_OK)
		{
			AyyLog("Failed to create dummy device, unable to hook EndScene");
			return false;
		}
	}

	return true;
}

long __stdcall GwEndScene(IDirect3DDevice9* d3d9Device) noexcept
{
	if (EndSceneExited) return HookEndScene->Original(d3d9Device);

	if (!BotOverlay)
	{
		// only run the setup once
		BotOverlay = new S4BotOverlay(S4->MainModule.Read<HWND>(S4Bot::Offsets::MAINHWND), d3d9Device);
	}
	else
	{
		BotOverlay->Render();

		if (static bool sVar = false; OnKeyPressedOnce(S4Bot::Keys::DEBUG_UI, sVar))
		{
			BotOverlay->SwitchVisibility();
		}
	}

	// release DirectX stuff and signal that we are ready to unhook
	if (EndSceneShouldExit)
	{
		if (BotOverlay) delete BotOverlay;
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

void __fastcall GwBuildingFinished(int unk) noexcept
{
	HookBuildingFinished->Original(unk);
	AyyLog("Woodcutter Count: ", S4->BuildingCount(S4Building::Woodcutter)); 
}

int __stdcall GwBuildCheck(unsigned int x, unsigned int y, unsigned int playerId, int buildingId, int unk) noexcept
{
	auto ret = HookBuildCheck->Original(x, y, playerId, buildingId, unk);
	AyyLog("(x: ", x, ", y: ", y, ", playerId: ", playerId, ", buildingId: ", buildingId, ", unk: ", unk, ") = ", ret);
	return ret;
}
