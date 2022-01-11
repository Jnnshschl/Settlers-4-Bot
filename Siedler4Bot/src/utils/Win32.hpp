#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <psapi.h>
#include <intrin.h>
#include <winternl.h>

#include <string>
#include <iostream>
#include <algorithm>

typedef struct C_LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    void* DllBase;
    void* EntryPoint;
    unsigned long SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    unsigned long Flags;
    unsigned short LoadCount;
    unsigned short TlsIndex;
    union
    {
        LIST_ENTRY HashLinks;
        struct
        {
            void* SectionPointer;
            unsigned long CheckSum;
        };
    };
    union
    {
        unsigned long TimeDateStamp;
        void* LoadedImports;
    };
    _ACTIVATION_CONTEXT* EntryPointActivationContext;
    void* PatchInformation;
    LIST_ENTRY ForwarderLinks;
    LIST_ENTRY ServiceTagLinks;
    LIST_ENTRY StaticLinks;
} CLDR_DATA_TABLE_ENTRY, * CPLDR_DATA_TABLE_ENTRY;

static bool AyyGetModuleHandle(const std::string& moduleName, void** moduleBase, unsigned int* moduleSize = nullptr)
{
    constexpr auto pebOffset = 0x30;
    constexpr auto ldrOffset = 0x0C;
    constexpr auto listOffset = 0x0C;
    unsigned long pPeb = __readfsdword(pebOffset);

    std::wstring wModuleName(moduleName.begin(), moduleName.end());
    std::transform(wModuleName.begin(), wModuleName.end(), wModuleName.begin(), ::toupper);

    pPeb = *reinterpret_cast<decltype(pPeb)*>(pPeb + ldrOffset);
    auto pModuleList = *reinterpret_cast<CPLDR_DATA_TABLE_ENTRY*>(pPeb + listOffset);

    while (pModuleList->DllBase)
    {
        std::wstring dName(pModuleList->BaseDllName.Buffer);
        std::transform(dName.begin(), dName.end(), dName.begin(), ::toupper);

        if (dName == wModuleName)
        {
            if (moduleSize)
            {
                *moduleSize = pModuleList->SizeOfImage;
            }

            *moduleBase = pModuleList->DllBase;
            return true;
        }

        pModuleList = reinterpret_cast<CPLDR_DATA_TABLE_ENTRY>(pModuleList->InLoadOrderLinks.Flink);
    }

    return false;
}

template<typename T>
static T AyyGetProcAddress(void* hModule, const char* apiName)
{
    auto pModuleBase = reinterpret_cast<unsigned char*>(hModule);
    auto pImageDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(pModuleBase);

    if (pImageDosHeader->e_magic == 0x5A4D)
    {
        auto pImageNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(pModuleBase + pImageDosHeader->e_lfanew);

        if (pImageNtHeaders->Signature == 0x4550)
        {
            auto pImageExportDirectory = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(pModuleBase + pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

            for (size_t i = 0; i < pImageExportDirectory->NumberOfNames; ++i)
            {
                auto name = reinterpret_cast<char*>(pModuleBase + reinterpret_cast<unsigned long*>(pModuleBase + pImageExportDirectory->AddressOfNames)[i]);

                if (!strcmp(name, apiName))
                {
                    auto ordinalAddress = reinterpret_cast<unsigned short*>(pModuleBase + pImageExportDirectory->AddressOfNameOrdinals)[i];
                    return reinterpret_cast<T>(pModuleBase + reinterpret_cast<unsigned long*>(pModuleBase + pImageExportDirectory->AddressOfFunctions)[ordinalAddress]);
                }
            }
        }
    }

    return nullptr;
}

static std::string LookupKeyName(unsigned char key)
{
    char buffer[16];
    GetKeyNameTextA((MapVirtualKeyA(key, 0) << 16) | (1 << 24) | (1 << 25), buffer, 16);
    return std::string(buffer);
}