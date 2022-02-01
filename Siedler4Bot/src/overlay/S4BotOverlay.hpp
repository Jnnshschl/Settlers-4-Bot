#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "../utils/Win32.hpp"

constexpr auto DUI_DEFAULT_FONT = "Consolas";
constexpr D3DCOLOR DebugUiTextColor = 0xFFFFFFFF;
constexpr D3DCOLOR DebugUiBackgroundColor = 0xFF141414;
constexpr D3DCOLOR DebugUiBorderColor = 0xFF75D700;
constexpr D3DCOLOR DebugUiWhiteColor = 0xFFFFFFFF;
constexpr D3DCOLOR DebugUiRedColor = 0xFFFF0000;

class S4BotOverlay
{
	HWND Hwnd;
	IDirect3DDevice9* D3d9Device;

	int X;
	int Y;
	int W;
	int H;

	bool Visible;

	ID3DXLine* DefaultLine;
	ID3DXFont* DefaultHeaderFont;
	ID3DXFont* DefaultTextFont;

	RECT DebugUiWindowRect;

	D3DRECT DebugUiPanelRect;
	D3DXVECTOR2 DebugUiPanelBorderVectors[5];
	RECT DebugUiHeaderRect;

public:
	S4BotOverlay(HWND hwnd, IDirect3DDevice9* d3d9Device, int width = 400, int height = 500, int initialX = 300, int initialY = 32);
	~S4BotOverlay();

	void Render() noexcept;
	void SwitchVisibility() noexcept;
};
