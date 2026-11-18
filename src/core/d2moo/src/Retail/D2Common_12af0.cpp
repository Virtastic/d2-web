// Diagnostic log line writer, moved from the stand-alone d2common wave batch into the game tree.
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>

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
