#pragma once

static uintptr_t SearchPattern(const unsigned char* pattern, size_t patternSize, void* memory, size_t memorySize) noexcept
{
    auto patternPosition = 0u;
    auto possibleAddress = 0u;
    auto memoryBase = reinterpret_cast<uintptr_t>(memory);
    auto maxPatternIndex = patternSize - 1u;
    auto mem = reinterpret_cast<unsigned char*>(memory);

    for (auto i = 0u; i < memorySize; ++i)
    {
        if (mem[i] == pattern[patternPosition] || pattern[patternPosition] == '?')
        {
            if (patternPosition == 0u)
            {
                possibleAddress = i;
            }
            else if (patternPosition == maxPatternIndex)
            {
                return memoryBase + possibleAddress;
            }

            ++patternPosition;
        }
        else
        {
            if (patternPosition > 0u)
            {
                i = possibleAddress;
            }

            patternPosition = 0u;
        }
    }

    return 0u;
}

template<typename T>
__forceinline static T SearchFn(const unsigned char* pattern, size_t patternSize, void* memory, size_t memorySize) noexcept
{
    return reinterpret_cast<T>(SearchPattern(pattern, patternSize, memory, memorySize));
}
