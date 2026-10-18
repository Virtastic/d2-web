// src/core/gfx/d2ddraw_batch_3e87.cpp -- twelve UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector yet). Retail
// base 0x6F8C0000; every RVA below is retail's and every module global is named at its retail RVA.
//
// These are MSVC 2003 (VC7.1) Release CRT rows the linker pulled into the DLL:
// the multibyte/locale bootstrap the static initialisers walk (0x3E87 code-page -> locale id,
// 0x3EB6 setSBCS, 0x3EDF table derivation, 0x407A __setmbcp_nolock, 0x420E __setmbcp,
// 0x4355/0x435E the unlock and ___initmbctable), the word-at-a-time memset/strlen/strcat/strncpy,
// and ___crtMessageBoxA. The CRT lock pair is already mapped (0x1ACC __lock, 0x1A17 __unlock) and so
// is _malloc (0x1811); _free (0x1E94), the kernel32 imports and the still-unmapped CRT callees
// (0x4F88 ___crtGetStringTypeA, 0x51D0 LCMapStringA wrapper) are host seams at the END of the
// argument list, the retail arguments keeping their order. Prior art: the VC7.1 CRT sources for
// mbset.c/string.c (the same bodies already ported in src/core/gfx/d2ddraw_batch_b32.cpp for _strcpy
// and in src/core/realm/mcp_exit.cpp for _strncpy).
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

extern "C" {
void  d2ddraw_sub_1A17(int locknum);   // 0x01A17 __unlock, d2ddraw_batch2_rowex.cpp
void  d2_fn_d2ddraw_1acc(int file);    // 0x01ACC __lock, d2ddraw_batch_10da.cpp
void* d2ddraw_malloc(size_t size);     // 0x01811 _malloc, d2ddraw_batch_rowex.cpp
}

// --- retail module state -------------------------------------------------------------------------
// +0x3A4/0x28C/0x284: the selected code page, the multi-byte flag (1 for a DBCS table) and the
// locale id handed to GetStringType/LCMapString.
static uint32_t g_codepage;        // +0x3A4 DAT_6f8d03a4
static uint32_t g_mb_cur_max;      // +0x28C DAT_6f8d028c
static uint32_t g_locale_id;       // +0x284 DAT_6f8d0284
// +0x3B0: three words copied into the saved locale block (table records' dwords 1..3).
static uint32_t g_cp_extra[3];     // +0x3B0 DAT_6f8d03b0/b4/b8
static uint8_t  g_mbctype[0x101];  // +0x2A0 DAT_6f8d02a0: per-byte (index = char + 1) __mbctype flags
static uint8_t  g_mbtolower[0x100];// +0x3C0 DAT_6f8d03c0: case-translation byte per character
static uint8_t* g_saved_mbcinfo;   // +0x288 DAT_6f8d0288: 0x220-byte saved locale block
static uint32_t g_setmbcp_flag;    // +0xCF214 DAT_6f8cf214: __setmbcp was asked for a pseudo code page
static uint32_t g_oem_cp;          // +0xCF274 DAT_6f8cf274: code page used for _MB_CP_OEM (-4)
static uint32_t g_initmbc_done;    // +0x10614 DAT_6f8d0614: ___initmbctable already ran

// +0x6F8CE5C0: four lead-byte category flags; +0x6F8CE5C8: five 0x30-byte code-page records.
// Record: +0 code page; +4/+8/0xC three words saved to 0x3B0..; +0x10..+0x2F four 8-byte ranges of
// byte pairs [lo,hi] flagged with the category at 0x6F8CE5C0[group].
static const uint8_t g_mbcs_lead_flags[4] = { 0x01, 0x02, 0x04, 0x08 };
static const uint8_t g_mbcs_table[5][0x30] = {
    { 0xA4,0x03,0x00,0x00, 0x60,0x82,0x79,0x82, 0x21,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0xA6,0xDF,0x00,0x00, 0x00,0x00,0x00,0x00, 0xA1,0xA5,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0x9F,0xE0,0xFC, 0x00,0x00,0x00,0x00, 0x40,0x7E,0x80,0xFC, 0x00,0x00,0x00,0x00 },
    { 0xA8,0x03,0x00,0x00, 0xC1,0xA3,0xDA,0xA3, 0x20,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00, 0x40,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00 },
    { 0xB5,0x03,0x00,0x00, 0xC1,0xA3,0xDA,0xA3, 0x20,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00, 0x41,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00 },
    { 0xB6,0x03,0x00,0x00, 0xCF,0xA2,0xE4,0xA2, 0x1A,0x00,0xE5,0xA2, 0xE8,0xA2,0x5B,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xFE,0x00,0x00, 0x00,0x00,0x00,0x00, 0x40,0x7E,0xA1,0xFE, 0x00,0x00,0x00,0x00 },
    { 0x51,0x05,0x00,0x00, 0x51,0xDA,0x5E,0xDA, 0x20,0x00,0x5F,0xDA, 0x6A,0xDA,0x32,0x00,
      0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
      0x81,0xD3,0xD8,0xDE, 0xE0,0xF9,0x00,0x00, 0x31,0x7E,0x81,0xFE, 0x00,0x00,0x00,0x00 },
};

// _cpinfo: UINT MaxCharSize; BYTE DefaultChar[2]; BYTE LeadByte[12].
struct D2DDrawCpInfo {
    uint32_t MaxCharSize;
    uint8_t  DefaultChar[2];
    uint8_t  LeadByte[12];
};

// --- seam environment ----------------------------------------------------------------------------
struct D2DDrawMbcsEnv {
    int      (*get_cp_info)(uint32_t cp, void* info);                                  // GetCPInfo
    int      (*get_string_type_a)(int locale, const char* src, int count, uint16_t* out,
                                  uint32_t cp, uint32_t lcid, int reserved);           // 0x04F88
    int      (*lcmap_string_a)(uint32_t lcid, uint32_t flags, const char* src, int src_len,
                               uint8_t* dst, int dst_len, uint32_t cp, int reserved);  // 0x051D0
    uint32_t (*get_oem_cp)(void);                                                      // GetOEMCP
    uint32_t (*get_acp)(void);                                                         // GetACP
    void     (*crt_free)(void* block);                                                 // 0x01E94
};

static int env_get_cp_info(uint32_t, void*) { return 0; }
static int env_get_string_type_a(int, const char*, int, uint16_t*, uint32_t, uint32_t, int) { return 0; }
static int env_lcmap_string_a(uint32_t, uint32_t, const char*, int, uint8_t*, int, uint32_t, int) { return 0; }
static uint32_t env_get_oem_cp(void) { return 0; }
static uint32_t env_get_acp(void) { return 0; }
static void env_crt_free(void* block) { std::free(block); }
static const D2DDrawMbcsEnv g_mbcs_env = {
    env_get_cp_info, env_get_string_type_a, env_lcmap_string_a,
    env_get_oem_cp, env_get_acp, env_crt_free,
};

struct D2DDrawMsgBoxEnv {
    void*    (*load_library_a)(const char* name);                                      // LoadLibraryA
    void*    (*get_proc_address)(void* module, const char* name);                      // GetProcAddress
    uint32_t (*get_active_window)(void);                                               // GetActiveWindow
    uint32_t (*get_last_active_popup)(uint32_t hwnd);                                  // GetLastActivePopup
    int      (*get_user_object_information_a)(uint32_t handle, int index, void* info,
                                              uint32_t len, uint32_t* needed);         // GetUserObjectInformationA
    uint32_t (*get_process_window_station)(void);                                      // GetProcessWindowStation
    int      (*message_box_a)(uint32_t hwnd, const char* text, const char* caption,
                              uint32_t type);                                          // MessageBoxA
};

static void* env_load_library_a(const char*) { return nullptr; }
static void* env_get_proc_address(void*, const char*) { return nullptr; }
static uint32_t env_get_active_window(void) { return 0; }
static uint32_t env_get_last_active_popup(uint32_t) { return 0; }
static int env_get_user_object_information_a(uint32_t, int, void*, uint32_t, uint32_t*) { return 0; }
static uint32_t env_get_process_window_station(void) { return 0; }
static int env_message_box_a(uint32_t, const char*, const char*, uint32_t) { return 0; }
static const D2DDrawMsgBoxEnv g_msgbox_env = {
    env_load_library_a, env_get_proc_address, env_get_active_window, env_get_last_active_popup,
    env_get_user_object_information_a, env_get_process_window_station, env_message_box_a,
};

// +0xCF218..0xCF228: cached user32 entry points; +0xCEEE4 platform id and +0xCEEF0 major version
// (both zero in the image until the CRT version probe runs, which this row does not).
static void*    g_msgbox_a;              // +0xCF218
static void*    g_getactivewindow;       // +0xCF21C
static void*    g_getlastactivepopup;    // +0xCF220
static void*    g_getprocesswindowstation; // +0xCF224
static void*    g_getuserobjectinfo;     // +0xCF228
static uint32_t g_os_platform;           // +0xCEEE4
static uint32_t g_os_version;            // +0xCEEF0

static inline uint32_t ld32(const uint8_t* p) {
    uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

extern "C" {
uint32_t d2ddraw_sub_3E87(uint32_t codepage);
void     d2ddraw_setSBCS(void);
void     d2ddraw_sub_3EDF(const D2DDrawMbcsEnv* env);
int32_t  d2ddraw_sub_407A(uint32_t codepage, const D2DDrawMbcsEnv* env);
int32_t  d2ddraw_setmbcp(int32_t codepage, const D2DDrawMbcsEnv* env);
void     d2ddraw_sub_4355(void);
}

// 1.13c D2DDraw+0x03E87: map the four CJK code pages to their default locale ids (932 -> 0x411,
// 936 -> 0x804, 949 -> 0x412, 950 -> 0x404); every other code page maps to 0. The caller passes the
// code page in EAX (the callee reads no stack argument), made explicit here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2ddraw_sub_3E87(uint32_t codepage) {
    if (codepage == 0x3A4) return 0x411;
    if (codepage == 0x3A8) return 0x804;
    if (codepage == 0x3B5) return 0x412;
    if (codepage == 0x3B6) return 0x404;
    return 0;
}

// 1.13c D2DDraw+0x03EB6 (setSBCS): zero the 0x101-byte __mbctype array and the selected code page,
// multi-byte flag, locale id and the three saved words -- the single-byte code page (C locale).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_setSBCS(void) {
    std::memset(g_mbctype, 0, sizeof g_mbctype);
    g_codepage = 0;
    g_mb_cur_max = 0;
    g_locale_id = 0;
    g_cp_extra[0] = g_cp_extra[1] = g_cp_extra[2] = 0;
}

// 1.13c D2DDraw+0x03EDF: derive __mbctype and the case map for the selected code page. When the
// kernel supplies a CPINFO, every lead-byte range is blanked to ' ' first, then GetStringTypeA fills
// the C1_* word per byte and LCMapStringA fills the lowercase (0x100) and uppercase (0x200) maps:
// an uppercase byte gets flag 0x10 and its lowercase map, a lowercase byte flag 0x20 and its
// uppercase map, anything else no case flag/map. Without CPINFO (GetCPInfo failed) the C-locale
// ASCII rule fills A-Z and a-z only.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_3EDF(const D2DDrawMbcsEnv* env) {
    D2DDrawCpInfo cpinfo;
    uint16_t ctype[256] = {};
    uint8_t  map_lower[256] = {};   // retail local_208: LCMapStringA flags 0x100
    uint8_t  map_upper[256] = {};   // retail local_308: LCMapStringA flags 0x200
    char     sbcs[256];

    if (env->get_cp_info(g_codepage, &cpinfo) == 1) {
        for (uint32_t i = 0; i < 0x100; i++) sbcs[i] = (char)i;
        sbcs[0] = ' ';
        if (cpinfo.LeadByte[0] != 0) {
            uint8_t lead[12];
            std::memcpy(lead, cpinfo.LeadByte, sizeof lead);
            const uint8_t* p = lead + 1;
            do {
                uint32_t lo = lead[0];
                if (lo <= *p) std::memset(sbcs + lo, ' ', *p - lo + 1);
                lead[0] = p[1];
                p += 2;
            } while (lead[0] != 0);
        }
        env->get_string_type_a(0x1, sbcs, 0x100, ctype, g_codepage, g_locale_id, 0);
        env->lcmap_string_a(g_locale_id, 0x100, sbcs, 0x100, map_lower, 0x100, g_codepage, 0);
        env->lcmap_string_a(g_locale_id, 0x200, sbcs, 0x100, map_upper, 0x100, g_codepage, 0);
        for (uint32_t i = 0; i < 0x100; i++) {
            if ((ctype[i] & 1) == 0) {
                if ((ctype[i] & 2) != 0) {
                    g_mbctype[i + 1] |= 0x20;
                    g_mbtolower[i] = map_upper[i];
                } else {
                    g_mbtolower[i] = 0;
                }
            } else {
                g_mbctype[i + 1] |= 0x10;
                g_mbtolower[i] = map_lower[i];
            }
        }
    } else {
        for (uint32_t i = 0; i < 0x100; i++) {
            if (i < 0x41 || i > 0x5A) {
                if (i > 0x60 && i < 0x7B) {
                    g_mbctype[i + 1] |= 0x20;
                    g_mbtolower[i] = (uint8_t)((char)i - 0x20);
                } else {
                    g_mbtolower[i] = 0;
                }
            } else {
                g_mbctype[i + 1] |= 0x10;
                g_mbtolower[i] = (uint8_t)((char)i + 0x20);
            }
        }
    }
}

// 1.13c D2DDraw+0x0407A (__setmbcp_nolock): select `codepage`. A record in the CJK table clears the
// tables, flags each of its four lead-byte range groups, sets the multi-byte flag and the locale id
// and copies the record's three words; otherwise GetCPInfo decides: MaxCharSize < 2 selects the
// single-byte rule, a DBCS code page sets the lead-byte flag 0x04, 0x08 on every other byte, the
// DBCS multi-byte flag and its locale id. A failed GetCPInfo returns -1 only when the caller did not
// ask for a pseudo code page (0xCF214); otherwise (and for code page 0) setSBCS is selected. Every
// path that changes the selection ends in the table derivation at 0x3EDF.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2ddraw_sub_407A(uint32_t codepage, const D2DDrawMbcsEnv* env) {
    if (codepage != 0) {
        int32_t idx = -1;
        for (int32_t i = 0; i < 5; i++) {
            if (ld32(g_mbcs_table[i]) == codepage) { idx = i; break; }
        }
        if (idx >= 0) {
            std::memset(g_mbctype, 0, sizeof g_mbctype);
            uint32_t group = 0;
            const uint8_t* ent = g_mbcs_table[idx] + 0x10;
            do {
                uint8_t lo = ent[0];
                const uint8_t* p = ent;
                while (lo != 0 && p[1] != 0) {
                    uint8_t hi = p[1];
                    if (lo <= hi) {
                        uint8_t flag = g_mbcs_lead_flags[group];
                        for (uint32_t u = lo; u <= hi; u++) g_mbctype[u + 1] |= flag;
                    }
                    p += 2;
                    lo = *p;
                }
                group++;
                ent += 8;
            } while (group < 4);
            g_codepage = codepage;
            g_mb_cur_max = 1;
            g_locale_id = d2ddraw_sub_3E87(codepage);
            std::memcpy(g_cp_extra, g_mbcs_table[idx] + 4, 12);
            goto tail;
        }

        D2DDrawCpInfo cpinfo;
        if (env->get_cp_info(codepage, &cpinfo) == 1) {
            std::memset(g_mbctype, 0, sizeof g_mbctype);
            g_codepage = codepage;
            g_locale_id = 0;
            if (cpinfo.MaxCharSize < 2) {
                g_mb_cur_max = 0;
            } else {
                if (cpinfo.LeadByte[0] != 0) {
                    const uint8_t* p = cpinfo.LeadByte + 1;
                    for (;;) {
                        uint8_t hi = *p;
                        if (hi == 0) break;
                        for (uint32_t u = p[-1]; u <= hi; u++) g_mbctype[u + 1] |= 4;
                        uint8_t next_even = p[1];
                        p += 2;
                        if (next_even == 0) break;
                    }
                }
                for (uint32_t u = 1; u < 0xFF; u++) g_mbctype[u + 1] |= 8;
                g_locale_id = d2ddraw_sub_3E87(codepage);
                g_mb_cur_max = 1;
            }
            g_cp_extra[0] = g_cp_extra[1] = g_cp_extra[2] = 0;
            goto tail;
        }
        if (g_setmbcp_flag == 0) return -1;
    }
    d2ddraw_setSBCS();
tail:
    d2ddraw_sub_3EDF(env);
    return 0;
}

// 1.13c D2DDraw+0x0420E (__setmbcp): under lock 0xD, resolve the pseudo code pages (-2 GetOEMCP,
// -3 GetACP, -4 the saved OEM code page) and, when the selection actually changes, select it through
// 0x407A. Success snapshots the new state into a 0x220-byte block (the previous block is reused when
// its first word is zero, otherwise malloc'd) and installs it at 0x6F8D0288; a -1 result frees a
// fresh block. Unlock always runs. Returns 0 when the code page was already current.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2ddraw_setmbcp(int32_t codepage, const D2DDrawMbcsEnv* env) {
    int32_t ret = -1;
    uint8_t* mem;

    d2_fn_d2ddraw_1acc(0xD);
    g_setmbcp_flag = 0;
    if (codepage == -2) {
        g_setmbcp_flag = 1;
        codepage = (int32_t)env->get_oem_cp();
    } else if (codepage == -3) {
        g_setmbcp_flag = 1;
        codepage = (int32_t)env->get_acp();
    } else if (codepage == -4) {
        g_setmbcp_flag = 1;
        codepage = (int32_t)g_oem_cp;
    }
    if ((uint32_t)codepage == g_codepage) {
        ret = 0;
    } else {
        if (g_saved_mbcinfo == nullptr || *(uint32_t*)g_saved_mbcinfo != 0) {
            mem = (uint8_t*)d2ddraw_malloc(0x220);
        } else {
            mem = g_saved_mbcinfo;
        }
        if (mem != nullptr && (ret = d2ddraw_sub_407A((uint32_t)codepage, env)) == 0) {
            *(uint32_t*)(mem + 0x00) = 0;
            *(uint32_t*)(mem + 0x04) = g_codepage;
            *(uint32_t*)(mem + 0x08) = g_mb_cur_max;
            *(uint32_t*)(mem + 0x0C) = g_locale_id;
            std::memcpy(mem + 0x10, g_cp_extra, 10);
            std::memcpy(mem + 0x1C, g_mbctype, 0x101);
            g_saved_mbcinfo = mem;
            std::memcpy(mem + 0x11D, g_mbtolower, 0x100);
        }
        if (ret == -1 && mem != g_saved_mbcinfo) env->crt_free(mem);
    }
    d2ddraw_sub_4355();
    return ret;
}

// 1.13c D2DDraw+0x04355: __unlock(0xD), the guard __setmbcp took at 0x420E.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_4355(void) {
    d2ddraw_sub_1A17(0xD);
}

// 1.13c D2DDraw+0x0435E (___initmbctable): the CRT lazy initialiser -- select the ANSI code page
// (__setmbcp(-3)) once and remember it; always returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2ddraw_initmbctable(const D2DDrawMbcsEnv* env) {
    if (g_initmbc_done == 0) {
        d2ddraw_setmbcp(-3, env);
        g_initmbc_done = 1;
    }
    return 0;
}

// 1.13c D2DDraw+0x04380 (_memset): byte-fill below four bytes; otherwise align the destination with
// single bytes, replicate the fill into a word and store whole words, then the remaining 0..3 bytes.
// Zero size writes nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2ddraw_memset(void* dst, int val, size_t size) {
    if (size == 0) return dst;
    uint8_t* p = (uint8_t*)dst;
    uint32_t fill = (uint32_t)val & 0xFF;

    if (size > 3) {
        uint32_t lead = (uint32_t)(-(intptr_t)dst) & 3;
        size_t words_bytes = size;
        if (lead != 0) {
            words_bytes = size - lead;
            do {
                *p++ = (uint8_t)val;
                lead--;
            } while (lead != 0);
        }
        fill = fill * 0x01010101u;
        size = words_bytes & 3;
        uint32_t words = (uint32_t)(words_bytes >> 2);
        if (words != 0) {
            for (; words != 0; words--) {
                *(uint32_t*)p = fill;
                p += 4;
            }
            if (size == 0) return dst;
        }
    }
    do {
        *p++ = (uint8_t)fill;
        size--;
    } while (size != 0);
    return dst;
}

// 1.13c D2DDraw+0x043F0 (_strcat): find the terminator of param_1 (byte scan to alignment, then the
// 0x7EFEFEFF/0x81010100 zero-byte trick) and copy param_2 there word-wise with a byte tail; a
// terminator anywhere in a source word picks the smallest store that still writes its zeros.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t* d2ddraw_sub_43F0(uint32_t* param_1, uint32_t* param_2) {
    uint32_t* puVar3 = param_1;
    uint32_t* puVar5;
    uint32_t uVar1, uVar2, uVar4;

    while (((uintptr_t)puVar3 & 3) != 0) {
        uVar1 = *(uint8_t*)puVar3;
        puVar3 = (uint32_t*)((uintptr_t)puVar3 + 1);
        if ((uint8_t)uVar1 == 0) {
            puVar5 = (uint32_t*)((uintptr_t)puVar3 - 1);
            goto copy;
        }
    }
    for (;;) {
        puVar5 = puVar3;
        puVar3 = puVar5 + 1;
        uVar1 = *puVar5;
        if (((uVar1 ^ 0xFFFFFFFFu ^ (uVar1 + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
            if ((uint8_t)uVar1 == 0) goto copy;
            if ((uint8_t)(uVar1 >> 8) == 0) {
                puVar5 = (uint32_t*)((uintptr_t)puVar5 + 1);
                goto copy;
            }
            if ((uVar1 & 0x00FF0000u) == 0) {
                puVar5 = (uint32_t*)((uintptr_t)puVar5 + 2);
                goto copy;
            }
            if ((uVar1 & 0xFF000000u) == 0) goto copy;
        }
    }
copy:
    for (;;) {
        if (((uintptr_t)param_2 & 3) == 0) {
            for (;;) {
                uVar2 = *param_2;
                uVar4 = *param_2;
                param_2 = param_2 + 1;
                if (((uVar2 ^ 0xFFFFFFFFu ^ (uVar2 + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
                    if ((uint8_t)uVar4 == 0) {
                        *(uint8_t*)puVar5 = (uint8_t)uVar4;
                        return param_1;
                    }
                    if ((uint8_t)(uVar4 >> 8) == 0) {
                        *(uint16_t*)puVar5 = (uint16_t)uVar4;
                        return param_1;
                    }
                    if ((uVar4 & 0x00FF0000u) == 0) {
                        *(uint16_t*)puVar5 = (uint16_t)uVar4;
                        *(uint8_t*)((uintptr_t)puVar5 + 2) = 0;
                        return param_1;
                    }
                    if ((uVar4 & 0xFF000000u) == 0) {
                        *puVar5 = uVar4;
                        return param_1;
                    }
                }
                *puVar5 = uVar4;
                puVar5 = puVar5 + 1;
            }
        }
        uint8_t bVar1 = *(uint8_t*)param_2;
        uVar4 = bVar1;
        param_2 = (uint32_t*)((uintptr_t)param_2 + 1);
        if (bVar1 == 0) {
            *(uint8_t*)puVar5 = (uint8_t)uVar4;
            return param_1;
        }
        *(uint8_t*)puVar5 = bVar1;
        puVar5 = (uint32_t*)((uintptr_t)puVar5 + 1);
    }
}

// 1.13c D2DDraw+0x044E0 (_strlen): byte scan until the source is word-aligned, then whole words with
// the zero-byte trick; the first zero byte's offset inside the word selects the return.
extern "C"
EMSCRIPTEN_KEEPALIVE size_t d2ddraw_strlen(char* str) {
    uint32_t* p = (uint32_t*)str;
    while (((uintptr_t)p & 3) != 0) {
        uint8_t b = *(uint8_t*)p;
        p = (uint32_t*)((uintptr_t)p + 1);
        if (b == 0) goto found_unaligned;
    }
    for (;;) {
        uint32_t* p3;
        uint32_t uVar1;
        do {
            p3 = p;
            p = p3 + 1;
            uVar1 = *p3;
        } while (((uVar1 ^ 0xFFFFFFFFu ^ (uVar1 + 0x7EFEFEFFu)) & 0x81010100u) == 0);
        if ((uint8_t)uVar1 == 0) return (size_t)((intptr_t)p3 - (intptr_t)str);
        if ((uint8_t)(uVar1 >> 8) == 0) return (size_t)((intptr_t)p3 + (1 - (intptr_t)str));
        if ((uVar1 & 0x00FF0000u) == 0) return (size_t)((intptr_t)p3 + (2 - (intptr_t)str));
        if ((uVar1 & 0xFF000000u) == 0) goto found_unaligned;
    }
found_unaligned:
    return (size_t)((intptr_t)p + (-1 - (intptr_t)str));
}

// 1.13c D2DDraw+0x0456B (___crtMessageBoxA): lazily bind user32's MessageBoxA and its window helpers.
// On NT also bind GetUserObjectInformationA/GetProcessWindowStation; if the process window station is
// visible, use the active window (or its last active popup) as the owner, otherwise OR in the
// topmost/service-bit depending on the OS major version and pass NULL owner. A user32 that cannot be
// loaded yields 0 without a message box.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2ddraw_crtMessageBoxA(const char* text, const char* caption,
                                                    uint32_t type, const D2DDrawMsgBoxEnv* env) {
    uint32_t hwnd = 0;

    if (g_msgbox_a == nullptr) {
        void* hmod = env->load_library_a("user32.dll");
        if (hmod == nullptr) return 0;
        g_msgbox_a = env->get_proc_address(hmod, "MessageBoxA");
        if (g_msgbox_a == nullptr) return 0;
        g_getactivewindow = env->get_proc_address(hmod, "GetActiveWindow");
        g_getlastactivepopup = env->get_proc_address(hmod, "GetLastActivePopup");
        if (g_os_platform == 2) {
            g_getuserobjectinfo = env->get_proc_address(hmod, "GetUserObjectInformationA");
            if (g_getuserobjectinfo != nullptr)
                g_getprocesswindowstation = env->get_proc_address(hmod, "GetProcessWindowStation");
        }
    }

    uint8_t  objinfo[16] = {};
    uint32_t needed = 0;
    if (g_getprocesswindowstation == nullptr ||
        ((hwnd = env->get_process_window_station()) != 0 &&
         env->get_user_object_information_a(hwnd, 1, objinfo, 0xC, &needed) != 0 &&
         (objinfo[8] & 1) != 0)) {
        if (g_getactivewindow != nullptr &&
            (hwnd = env->get_active_window()) != 0 &&
            g_getlastactivepopup != nullptr) {
            hwnd = env->get_last_active_popup(hwnd);
        }
    } else if (g_os_version < 4) {
        type |= 0x40000;
    } else {
        type |= 0x200000;
    }
    return env->message_box_a(hwnd, text, caption, type);
}

// 1.13c D2DDraw+0x04670 (_strncpy): copy at most `count` bytes, zero-filling the rest of the
// destination once the source terminator is seen. A byte copy brings the source to alignment; the
// aligned body copies words whose zero-byte test selects a 1/2/3/4-byte store and then word-wise
// zero fill; the remainder below four bytes is copied byte-wise, a terminator zero-filling the rest.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2ddraw_strncpy(char* dest, char* src, size_t count) {
    if (count == 0) return dest;
    uint32_t* dst = (uint32_t*)dest;
    uint32_t uVar1, uVar2, uVar4;
    char cVar3;

    if (((uintptr_t)src & 3) != 0) {
        for (;;) {
            uVar1 = *(uint32_t*)src;
            src = (char*)((uintptr_t)src + 1);
            *(char*)dst = (char)uVar1;
            dst = (uint32_t*)((uintptr_t)dst + 1);
            if (--count == 0) return dest;
            if ((char)uVar1 == 0) {
                for (;;) {
                    if (((uintptr_t)dst & 3) == 0) {
                        uVar4 = (uint32_t)(count >> 2);
                        if (uVar4 == 0) goto zero_pad;
                        do {
                            *dst++ = 0;
                        } while (--uVar4 != 0);
                        count &= 3;
                        if (count != 0) goto zero_pad;
                        return dest;
                    }
                    *(char*)dst = 0;
                    dst = (uint32_t*)((uintptr_t)dst + 1);
                    if (--count == 0) return dest;
                }
            }
            if (((uintptr_t)src & 3) == 0) {
                uVar4 = (uint32_t)(count >> 2);
                if (uVar4 != 0) goto word_copy;
                goto byte_tail;
            }
        }
    }
    uVar4 = (uint32_t)(count >> 2);
    if (uVar4 != 0) {
word_copy:
        do {
            uVar1 = *(uint32_t*)src;
            uVar2 = *(uint32_t*)src;
            src = (char*)((uintptr_t)src + 4);
            if (((uVar1 ^ 0xFFFFFFFFu ^ (uVar1 + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
                if ((char)uVar2 == 0) {
                    *dst = 0;
                    goto zero_words;
                }
                if ((char)(uVar2 >> 8) == 0) {
                    *dst = uVar2 & 0xFFu;
                    goto zero_words;
                }
                if ((uVar2 & 0x00FF0000u) == 0) {
                    *dst = uVar2 & 0xFFFFu;
                    goto zero_words;
                }
                if ((uVar2 & 0xFF000000u) == 0) {
                    *dst = uVar2;
                    goto zero_words;
                }
            }
            *dst = uVar2;
            dst = dst + 1;
        } while (--uVar4 != 0);
        count &= 3;
        if (count == 0) return dest;
    }
byte_tail:
    do {
        cVar3 = *src;
        src = (char*)((uintptr_t)src + 1);
        *(char*)dst = cVar3;
        dst = (uint32_t*)((uintptr_t)dst + 1);
        if (cVar3 == 0) {
            while (--count != 0) {
                *(char*)dst = 0;
                dst = (uint32_t*)((uintptr_t)dst + 1);
            }
            return dest;
        }
    } while (--count != 0);
    return dest;

zero_words:
    for (;;) {
        dst = dst + 1;
        if (--uVar4 == 0) break;
        *dst = 0;
    }
    count &= 3;
    if (count == 0) return dest;
zero_pad:
    do {
        *(char*)dst = 0;
        dst = (uint32_t*)((uintptr_t)dst + 1);
    } while (--count != 0);
    return dest;
}
