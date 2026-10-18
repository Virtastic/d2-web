// src/core/realm/bnclient_batch3_crt_unwind.cpp -- 1.13c BNClient.dll alpha-lane batch: the MSVC 2003
// strcpy leaf and the CRT file-handle unlock thunks. Every body is transcribed from the cached Ghidra
// decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (base 0x6FF20000). Host services retail
// reaches through imports (the CRT lock table, SMemFree at 0x801E) arrive as explicit seams after the
// retail arguments, in retail order, never invented behaviour.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c BNClient+0x06AB0 (7 B, callers 0x42D7/0x47FD/0x72CA): __cdecl strcpy. Copy src into dest: while
// the source is not dword-aligned copy single bytes (the first zero byte is stored and ends the copy).
// Once aligned load dwords and use the (v ^ ~0 ^ (v + 0x7EFEFEFF)) & 0x81010100 zero-byte test; on a hit
// store only the bytes up to and including the terminator (1, 2, 3 or 4 of them), otherwise store all
// four. Returns dest. All stores are little-endian, as on the retail image.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2_fn_bnclient_6ab0(char* dest, const char* src) {
    char* out = dest;
    while ((reinterpret_cast<uintptr_t>(src) & 3u) != 0) {
        const unsigned char b = static_cast<unsigned char>(*src);
        ++src;
        if (b == 0) {
            *out = '\0';
            return dest;
        }
        *out++ = static_cast<char>(b);
    }
    for (;;) {
        uint32_t word;
        std::memcpy(&word, src, sizeof word);
        src += 4;
        if (((word ^ 0xFFFFFFFFu ^ (word + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
            if ((word & 0x000000FFu) == 0) {
                *out = '\0';
                return dest;
            }
            if ((word & 0x0000FF00u) == 0) {
                std::memcpy(out, &word, 2);
                return dest;
            }
            if ((word & 0x00FF0000u) == 0) {
                std::memcpy(out, &word, 2);
                out[2] = '\0';
                return dest;
            }
            if ((word & 0xFF000000u) == 0) {
                std::memcpy(out, &word, 4);
                return dest;
            }
        }
        std::memcpy(out, &word, 4);
        out += 4;
    }
}

typedef void (*BnUnlockFhandleFn)(int32_t file_handle);   // 0x79AD __unlock_fhandle

// 1.13c BNClient+0x05C42 (8 B, caller 0x5BBB __lseek): tail thunk to __unlock_fhandle. The handle arrives
// in EBX on retail (Ghidra's unaff_EBX); here it is the explicit argument before the host seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_5c42(int32_t file_handle, BnUnlockFhandleFn unlock_fhandle) {
    unlock_fhandle(file_handle);
}

// 1.13c BNClient+0x05EA9 (8 B, caller 0x5E22 __write): the same tail thunk as 0x5C42, a second copy.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_5ea9(int32_t file_handle, BnUnlockFhandleFn unlock_fhandle) {
    unlock_fhandle(file_handle);
}

typedef void (*BnNewsTableFreeFn)(uint32_t ptr, const char* type, uint32_t line, uint32_t flags);
extern "C" void d2_fn_bnclient_1160(uint32_t self, BnNewsTableFreeFn smem_free);   // 0x01160

// 1.13c BNClient+0x185A0 (8 B, no callers in the image; referenced from .xdata D:0x1B220): SEH unwind
// thunk for the NewsTable destructor leaf 0x01160. `this` arrives in EAX on retail (Ghidra recovers no
// stack arguments); it and the SMemFree import are the explicit seam arguments here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_185a0(uint32_t self, BnNewsTableFreeFn smem_free) {
    d2_fn_bnclient_1160(self, smem_free);
}

// 1.13c BNClient+0x185CA (8 B, no callers in the image; referenced from .xdata D:0x1B24C): the same
// unwind thunk as 0x185A0 for the second destructor scope.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_185ca(uint32_t self, BnNewsTableFreeFn smem_free) {
    d2_fn_bnclient_1160(self, smem_free);
}
