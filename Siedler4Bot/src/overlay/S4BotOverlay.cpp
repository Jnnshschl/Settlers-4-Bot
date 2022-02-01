#include "S4BotOverlay.hpp"

S4BotOverlay::S4BotOverlay(HWND hwnd, IDirect3DDevice9* d3d9Device, int width, int height, int initialX, int initialY)
	: Hwnd(hwnd),
	D3d9Device(d3d9Device),
	W(width),
	H(height),
	X(initialX),
	Y(initialY),
	Visible(false)
{
	D3DXCreateLine(d3d9Device, &DefaultLine);
	DefaultLine->SetWidth(1);

	D3DXCreateFontA(d3d9Device, 24, 0, FW_DONTCARE, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DUI_DEFAULT_FONT, &DefaultHeaderFont);
	D3DXCreateFontA(d3d9Device, 14, 0, FW_DONTCARE, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DUI_DEFAULT_FONT, &DefaultTextFont);

	// align debug ui in the upper right corner if possible
	if (GetWindowRect(Hwnd, &DebugUiWindowRect))
	{
		X = DebugUiWindowRect.right - DebugUiWindowRect.left - width - 16;
		Y = 0; // to align at bottom: DebugUiWindowHeight - DUI_PANEL_HEIGHT - 1
	}

	DebugUiPanelRect =
	{
		X,
		Y,
		X + W,
		Y + H
	};

	DebugUiPanelBorderVectors[0] = D3DXVECTOR2(X, Y);
	DebugUiPanelBorderVectors[1] = D3DXVECTOR2(X + W, Y);
	DebugUiPanelBorderVectors[2] = D3DXVECTOR2(X + W, Y + H);
	DebugUiPanelBorderVectors[3] = D3DXVECTOR2(X, Y + H);
	DebugUiPanelBorderVectors[4] = D3DXVECTOR2(X, Y);

	DebugUiHeaderRect =
	{
		X + 4,
		Y + 2,
		X + W,
		Y + H
	};
}

S4BotOverlay::~S4BotOverlay()
{
	DefaultLine->Release();
	DefaultHeaderFont->Release();
	DefaultTextFont->Release();
}

void S4BotOverlay::Render() noexcept
{
	if (Visible)
	{
		D3d9Device->Clear(1, &DebugUiPanelRect, D3DCLEAR_TARGET, DebugUiBackgroundColor, 0, 0);
		DefaultHeaderFont->DrawTextA(0, "S4Bot", -1, &DebugUiHeaderRect, DT_LEFT | DT_NOCLIP, 0xFFFFFFFF);
		DefaultLine->Draw(DebugUiPanelBorderVectors, 5, DebugUiBorderColor);
	}
}

void S4BotOverlay::SwitchVisibility() noexcept
{
	Visible = !Visible;
}
