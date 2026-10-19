// src/core/d2ddraw/d2ddraw_crt_get_string_type_a.cpp -- UNMAPPED 1.13c D2DDraw.dll row reimplemented
// from the cached decompile build-wasm/ghidra-d2ddraw/decomp/0x4F88.c (alpha lane: no spec, no vector).
// Retail base 0x6F8C0000; every global is named at its retail RVA.
//
// D2DDraw+0x04F88 (421 B, callers 0x03EDF/0x05C5E) is MSVC 2003 (VC7.1) Release CRT glue, the
// ___crtGetStringTypeA body. Signature:
//   BOOL __cdecl __crtGetStringTypeA(_locale_t plocinfo, DWORD info_type, LPCSTR src, int cch,
//                                    LPWORD out, int code_page, BOOL b_error)
//
// First call probes the wide API: GetStringTypeW(1, L"", 1, &w). Success sets the state at +0x0F27C to
// 1 (wide path); ERROR_CALL_NOT_IMPLEMENTED (0x78) sets it to 2 (ANSI path); a silent failure leaves 0.
// State 2 or 0 takes the ANSI path: the locale is code_page or the default at +0x0F264, the target is
// `out` or the default ctype table at +0x0F274, the locale's ANSI codepage comes from 0x05919 (-1 falls
// through to 0), and when it differs from code_page the string is converted through 0x05960; the
// conversion result is freed and a null conversion returns 0. GetStringTypeA then answers. State 1
// takes the wide path: MultiByteToWideChar(code_page, b_error ? 9 : 1, src, cch) sizes a zeroed buffer,
// the second call fills it, GetStringTypeW answers, and the buffer is freed. A failed conversion
// returns 0.
//
// The Win32 entry points and the two unported internal rows (0x05919, 0x05960) are seams on the env
// struct. The Ghidra listing shuffles the stack arguments after the injected __SEH_prolog; the call
// shapes below are the CRT contract, as already modelled for the same 421-byte body in
// src/core/realm/bnclient_batch_030c3.cpp.
#include <cstdint>
#include <cstring>

// Retail module state, all in D2DDraw's data section.
static uint32_t g_gst_default_locale;                    // +0x0F264  DAT_6f8cf264
static uint16_t* g_gst_default_pctype;                   // +0x0F274  DAT_6f8cf274 (_pctype)
static uint32_t g_gst_wide_state;                        // +0x0F27C  DAT_6f8cf27c

struct D2DDrawCrtGetStringTypeAEnv {
    uint32_t (*get_last_error)(void);                                              // GetLastError
    int32_t  (*get_string_type_w)(uint32_t info_type, const uint16_t* src, int32_t cch,
                                  uint16_t* out);                                  // GetStringTypeW
    int32_t  (*get_string_type_a)(uint32_t locale, uint32_t info_type, const char* src,
                                  int32_t cch, uint16_t* out);                     // GetStringTypeA
    uint32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src,
                                        int32_t cb, uint16_t* out, int32_t cch);   // MultiByteToWideChar
    int32_t  (*locale_ansi_codepage)(uint32_t locale, const void* env);            // +0x05919
    // +0x05960: convert `src` between codepages; null on failure, *pcb updated on success.
    char*    (*convert)(uint32_t from, uint32_t to, const char* src, uint32_t* pcb,
                        char* outbuf, int32_t outlen, const void* env);
    void*    (*malloc)(uint32_t bytes);                                            // +0x01F05
    void     (*free)(void* p);                                                     // +0x01E94
};

// 1.13c D2DDraw+0x04F88 (421 B): the ANSI/wide GetStringTypeA described above.
extern "C"
int32_t d2ddraw_crt_get_string_type_a(uint32_t plocinfo, uint32_t info_type, const char* src,
        int32_t cch, uint16_t* out, uint32_t code_page, int32_t b_error,
        const D2DDrawCrtGetStringTypeAEnv* env) {
    (void)plocinfo;
    if (g_gst_wide_state == 0) {
        uint16_t probe[2] = {0, 0};
        if (env->get_string_type_w(1, reinterpret_cast<const uint16_t*>(u""), 1, probe) != 0)
            g_gst_wide_state = 1;
        else if (env->get_last_error() == 0x78)
            g_gst_wide_state = 2;
    }
    if (g_gst_wide_state == 2 || g_gst_wide_state == 0) {
        const uint32_t locale = code_page != 0 ? code_page : g_gst_default_locale;
        uint16_t* dest = out != nullptr ? out : g_gst_default_pctype;
        const int32_t ansi_cp = env->locale_ansi_codepage(locale, env);
        if (ansi_cp != -1) {
            char* converted = nullptr;
            if (static_cast<uint32_t>(ansi_cp) != code_page) {
                uint32_t length = static_cast<uint32_t>(cch);
                converted = env->convert(code_page, static_cast<uint32_t>(ansi_cp), src, &length,
                                         nullptr, 0, env);
                if (converted == nullptr) return 0;
                cch = static_cast<int32_t>(length);
            }
            const int32_t rc = env->get_string_type_a(locale, info_type,
                                                      converted != nullptr ? converted : src,
                                                      cch, dest);
            if (converted != nullptr) env->free(converted);
            return rc;
        }
    } else if (g_gst_wide_state == 1) {
        uint16_t* dest = out != nullptr ? out : g_gst_default_pctype;
        const int32_t wide_count = static_cast<int32_t>(env->multi_byte_to_wide_char(
            code_page, static_cast<uint32_t>(b_error != 0) * 8 + 1, src, cch, nullptr, 0));
        if (wide_count != 0) {
            uint16_t* wide = static_cast<uint16_t*>(
                env->malloc((static_cast<uint32_t>(wide_count) * 2u + 3u) & ~3u));
            if (wide == nullptr) return 0;
            std::memset(wide, 0, static_cast<size_t>(wide_count) * 2u);
            int32_t rc = 0;
            if (env->multi_byte_to_wide_char(code_page, 1, src, cch, wide, wide_count) != 0)
                rc = env->get_string_type_w(info_type, wide, wide_count, dest);
            env->free(wide);
            return rc;
        }
    }
    return 0;
}
