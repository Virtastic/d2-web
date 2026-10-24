#include <cstdint>
#include <cstdio>

// 1.13c Storm+0x09BA1
// _NMSG_WRITE(rterrnum): retail looks the number up in a 19-entry message table and either writes
// the text to stderr (console mode) or shows a Win32 MessageBox. Wasm has no MessageBox, so the text
// goes to stderr in both modes; error 0xFC (banner suppressed) writes nothing in the box path.
extern "C" void Storm_9BA1_NmsgWrite(const char* pszText, int32_t nRtErr)
{
    if (!pszText || nRtErr == 0xFC) return;
    std::fprintf(stderr, "Runtime Error!\n\n%s\n", pszText);
}
