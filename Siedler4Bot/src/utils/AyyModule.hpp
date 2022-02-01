#pragma once

#include "Win32.hpp"

struct AyyModule
{
	const char* Name;
	void* Base;
	unsigned int Size;

	explicit AyyModule(const char* name)
		: Name(name),
		Base(nullptr),
		Size(0)
	{}

	bool Init() noexcept
	{
		return AyyGetModuleHandle(Name, &Base, &Size);
	}

	template<typename T>
	constexpr T Read(ptrdiff_t offset = 0) noexcept
	{
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(Base) + offset);
	}
};
