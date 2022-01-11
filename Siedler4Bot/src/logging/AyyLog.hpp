#pragma once

#include <string>
#include <iomanip>
#include <iostream>

static bool _consoleOpened;
static _iobuf* _stdoutHandle;

#define AYY_CONSOLE

#define POINTER_AS_HEX(x) std::hex, reinterpret_cast<unsigned int>(x), std::dec

#ifdef AYY_CONSOLE

#define AyyLog(...) _AYYLOGA(__FUNCTION__, __VA_ARGS__)
#define DBG_OPEN_CONSOLE if (_consoleOpened = AllocConsole()) { freopen_s(&_stdoutHandle, "CONOUT$", "w", stdout); SetConsoleTitleA("S4Bot"); }
#define DBG_CLOSE_CONSOLE if (_consoleOpened && _stdoutHandle) { fclose(_stdoutHandle); FreeConsole(); }

#else

#define AyyLog(...)
#define DBG_OPEN_CONSOLE
#define DBG_CLOSE_CONSOLE

#endif

template<typename ...Args>
void _AYYLOGA(const std::string& tag, Args&& ...args)
{
    std::cout << std::setw(24) << tag << " >> ";
    (std::cout << ... << args);
    std::cout << std::endl;
}