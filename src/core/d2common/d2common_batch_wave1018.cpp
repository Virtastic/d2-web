// D2Common implementation batch (1.13c RVAs 0x12AF0, 0x32E10, 0x382F0, 0x3D250, 0x4F120, 0x66630).
// Cached 1.13c decompiles define the control flow. Calls into retail globals, CRT state, and the
// engine's not-yet-mapped helpers are explicit seams so the behavior can be wired as those rows land.
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
    // 0x66630 reads room bounds and a room-chain before reporting fatal errors.
    struct RoomBounds
    {
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
    };
    // 0x32E10 constructs a temporary stat list using the shared stat table and helpers.
    struct StatRecord
    {
        int32_t fields[15];
    };
}

// 1.13c D2Common+0x382F0: copies successive 8-byte records from the selected entry table while
// their upper halfword matches the requested type and the caller's output limit is not reached.
extern "C" int32_t D2Common_382F0_CopyMatchingEntries(void* pOutput, uint32_t nType, int32_t nLimit,
    int32_t nEntryMode, const uint8_t* pPrimary, int32_t nPrimaryCount, const uint8_t* pSecondary,
    int32_t nSecondaryCount, int32_t (*pfnFindEntry38220)(void))
{
    if (pOutput == nullptr || nLimit <= 0 || pfnFindEntry38220 == nullptr)
        return 0;
    const uint8_t* pEntries = nEntryMode < 0 ? pSecondary : pPrimary;
    const int32_t nCount = nEntryMode < 0 ? nSecondaryCount : nPrimaryCount;
    int32_t nIndex = pfnFindEntry38220();
    int32_t nCopied = 0;
    if (nIndex < 0 || pEntries == nullptr)
        return 0;
    while (nIndex < nCount && nCopied < nLimit)
    {
        const uint8_t* pEntry = pEntries + static_cast<size_t>(nIndex) * 8;
        uint32_t nHeader;
        std::memcpy(&nHeader, pEntry, sizeof(nHeader));
        if ((nHeader >> 16) != nType)
            break;
        std::memcpy(static_cast<uint8_t*>(pOutput) + static_cast<size_t>(nCopied) * 8, pEntry, 8);
        ++nCopied;
        ++nIndex;
    }
    return nCopied;
}

// 1.13c D2Common+0x3D250: walks the linked text nodes, releases each one, then clears the
// associated 16-bit count. Node pointers are passed as retail-width addresses by the caller.
extern "C" void D2Common_3D250_ClearTextList(uint8_t* pTextOwner, void* pFirstNode,
    void* (*pfnGetNextNode)(void*), void (*pfnTextDiagnostic)(const char*, uint32_t, uint32_t))
{
    if (pTextOwner == nullptr)
        return;
    void* pNode = pFirstNode;
    while (pNode != nullptr)
    {
        void* pNext = pfnGetNextNode ? pfnGetNextNode(pNode) : nullptr;
        if (pfnTextDiagnostic != nullptr)
            pfnTextDiagnostic("..\\Source\\D2Common\\Text\\Text.cpp", 0x3c, 0);
        pNode = pNext;
    }
    *reinterpret_cast<uint16_t*>(pTextOwner + 4) = 0;
}

// 1.13c D2Common+0x66630: usercall (ESI=pRoom, EDI=pUnit, stack bCheckList). Reads the unit's
// tile (dynamic: pPath+0xC/+0x10 dwords; else pPath ? +2/+6 words), tests it against the room's
// box (+0x4C x,+0x50 y,+0x54 w,+0x58 h); outside -> fatal 0x2e. Inside: walk room unit list
// (+0x74, next at +0xE8); found -> 1 (or fatal 0x36 when bCheckList); absent -> 0 for !bCheckList.
// The fatal path (ordinals 10265/10024 then _exit(-1)) is reported through pfnFatal.
extern "C" int32_t D2Common_66630_ValidateRoomMembership(const uint8_t* pRoom, const uint8_t* pUnit,
    int32_t bCheckList, void (*pfnFatal)(uint32_t))
{
    const int32_t nType = *reinterpret_cast<const int32_t*>(pUnit);
    const uint8_t* pPath = *reinterpret_cast<const uint8_t* const*>(pUnit + 0x2c);
    const bool bDyn = nType == 2 || (nType > 3 && nType < 6);
    int32_t x, y;
    if (bDyn) { x = *reinterpret_cast<const int32_t*>(pPath + 0xc); y = *reinterpret_cast<const int32_t*>(pPath + 0x10); }
    else { x = pPath ? *reinterpret_cast<const uint16_t*>(pPath + 2) : 0; y = pPath ? *reinterpret_cast<const uint16_t*>(pPath + 6) : 0; }
    const int32_t* r = reinterpret_cast<const int32_t*>(pRoom);
    uint32_t nCode = 0x2e;
    if (pRoom && x >= r[0x4c / 4] && x < r[0x4c / 4] + r[0x54 / 4] && y >= r[0x50 / 4] && y < r[0x50 / 4] + r[0x58 / 4])
    {
        for (const uint8_t* u = *reinterpret_cast<const uint8_t* const*>(pRoom + 0x74); ; u = *reinterpret_cast<const uint8_t* const*>(u + 0xe8))
        {
            if (!u) return 1;
            if (u == pUnit) break;
        }
        if (!bCheckList) return 0;
        nCode = 0x36;
    }
    if (pfnFatal) pfnFatal(nCode);
    return -1;
}

// 1.13c D2Common+0x12AF0: emits a timestamped tab-separated diagnostic prefix and the caller's
// formatted message when logging is enabled. The environment and output stream are explicit inputs.
extern "C" void D2Common_12AF0_WriteDiagnostic(uint32_t bEnabled, const char* szTimestamp,
    const int32_t* pFields, const char* szLabel, const char* szFormat, va_list args,
    void (*pfnWrite)(const char*, size_t), void (*pfnFlush)(void))
{
    if (bEnabled == 0 || szTimestamp == nullptr || pFields == nullptr || szFormat == nullptr || pfnWrite == nullptr)
        return;
    char buffer[1024]{};
    const int nPrefix = std::snprintf(buffer, sizeof(buffer), "%s\t%d\t%d\t%d\t%02d\t%.50s\t%.5d\t",
        szTimestamp, pFields[0], pFields[1], pFields[2], pFields[3], szLabel ? szLabel : "", pFields[5]);
    if (nPrefix < 0 || static_cast<size_t>(nPrefix) >= sizeof(buffer))
        return;
    const size_t nOffset = static_cast<size_t>(nPrefix);
    const int nMessage = std::vsnprintf(buffer + nOffset, sizeof(buffer) - nOffset, szFormat, args);
    if (nMessage < 0)
        return;
    size_t nLength = nOffset + static_cast<size_t>(nMessage);
    if (nLength >= sizeof(buffer))
        nLength = sizeof(buffer) - 1;
    if (nLength + 2 <= sizeof(buffer))
    {
        buffer[nLength++] = '\r';
        buffer[nLength++] = '\n';
    }
    pfnWrite(buffer, nLength);
    if (pfnFlush != nullptr)
        pfnFlush();
}

// 0x32E10 is UNITS_CharacterStartRunningOrKnockback (src/core/d2moo/src/Units/Units.cpp); the word-array placeholder that stood here is retired.
