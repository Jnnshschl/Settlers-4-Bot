#pragma once

#include "Win32.hpp"

namespace Hook
{
    template<typename T>
    constexpr T GetVirtualFunction(void* base, int index)
    {
        return (*reinterpret_cast<T**>(base))[index];
    }

    template <typename T, unsigned int index, typename ...Args>
    constexpr T CallVirtualFunction(void* base, Args... args)
    {
        return ((*reinterpret_cast<T(__thiscall***)(void*, Args...)>(base))[index])(base, args...);
    }

    inline void PatchMemCpy(void* destination, void* source, unsigned int size, unsigned long access = PAGE_EXECUTE_READWRITE)
    {
        unsigned long oldProtection;
        VirtualProtect(destination, size, access, &oldProtection);

        memcpy(destination, source, size);

        unsigned long newOldProtection;
        VirtualProtect(destination, size, oldProtection, &newOldProtection);
    }

    inline void PatchMemSet(void* destination, int value, unsigned int size, unsigned long access = PAGE_EXECUTE_READWRITE)
    {
        unsigned long oldProtection;
        VirtualProtect(destination, size, access, &oldProtection);

        memset(destination, value, size);

        unsigned long newOldProtection;
        VirtualProtect(destination, size, oldProtection, &newOldProtection);
    }

    template<typename T>
    class IHook
    {
    public:
        void* HookMethod;
        int Size;
        void* Position;
        T Original;
        unsigned char* OriginalInstructions;

        IHook(void* hookMethod, void* position, int size, int offset) noexcept
            : HookMethod(hookMethod),
            Size(size),
            Position(position),
            Original(nullptr),
            OriginalInstructions(new unsigned char[size])
        {
            Hook();
        }

        ~IHook() noexcept
        {
            PatchMemCpy(Position, OriginalInstructions, Size);
            VirtualFree(Original, 0, MEM_RELEASE);
            delete[] OriginalInstructions;
        }

        IHook(const IHook&) = delete;
        IHook& operator=(const IHook&) = delete;

        virtual void Hook() noexcept
        {
            memcpy(OriginalInstructions, Position, Size);
            Original = reinterpret_cast<T>(TrampHook(Position, HookMethod, Size));
        }

        virtual unsigned char* TrampHook(void* source, void* destination, std::ptrdiff_t lenght, std::ptrdiff_t jumpOffset = 0) noexcept
        {
            if (lenght >= 5)
            {
                unsigned long size = lenght + 5;
                unsigned char* gateway = reinterpret_cast<unsigned char*>(VirtualAlloc(0, size, 0x00001000 | 0x00002000, 0x40));

                if (gateway)
                {
                    // copy original instructions
                    memcpy(gateway, source, lenght);
                    auto jumpAddress = static_cast<unsigned int>(static_cast<unsigned char*>(source) - gateway - 5);

                    // place JMP back to original function
                    *(gateway + lenght) = static_cast<unsigned char>(0xE9);
                    *reinterpret_cast<unsigned int*>(gateway + lenght + 1) = jumpAddress;

                    // unprotect original function
                    unsigned long oldProtection;
                    VirtualProtect(source, lenght, 0x40, &oldProtection);

                    // NOP out the bytes
                    memset(source, 0x90, lenght);

                    // place JMP at the original function
                    *(static_cast<unsigned char*>(source) + jumpOffset) = static_cast<unsigned char>(0xE9);
                    *reinterpret_cast<unsigned int*>(static_cast<unsigned char*>(source) + 1 + jumpOffset) = (unsigned int)(reinterpret_cast<unsigned int>(destination) - reinterpret_cast<unsigned int>(source) - 5 - jumpOffset);

                    // reprotect with original protection
                    unsigned long newOldProtection;
                    VirtualProtect(source, lenght, oldProtection, &newOldProtection);

                    return gateway;
                }
            }

            return nullptr;
        }
    };

    template<typename T>
    class DirectHook : public IHook<T>
    {
    public:
        DirectHook(void* hookMethod, void* position, int size, int offset = 0) noexcept
            : IHook<T>{ hookMethod, reinterpret_cast<void*>(reinterpret_cast<unsigned int>(position) + offset), size, offset }
        {}
    };

    template<typename T>
    class VFuncHook : public IHook<T>
    {
    public:
        VFuncHook(void* hookMethod, void* base, int index, int size, int offset = 0) noexcept
            : IHook<T>{ hookMethod, reinterpret_cast<void*>(GetVirtualFunction<unsigned int>(base, index) + offset), size, offset }
        {}
    };
}