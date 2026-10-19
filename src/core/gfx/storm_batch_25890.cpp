// src/core/gfx/storm_batch_25890.cpp -- the 1.13c Storm.dll 0x25890 row (SCmdProcess, export #475)
// transcribed from the cached decompile build-wasm/ghidra-storm/decomp/0x25890.c. Alpha lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Prior art used: D2MOO 1.10f reference/D2MOO/source/Storm/include/Storm.h names ordinal 475
// SCmdProcess with the same four arguments, `(const char **string, int a2, int a3, int a4)`; the
// 1.13c decompile below is the contract. The callees 0x257F0 (token dispatcher, also reached from
// 0x25290 "SCMD.CPP"), 0x3B3A0 (quote-aware tokenizer, SSTR) and 0x24CD0 (error-callback invoker)
// are unmapped rows and stay seams in retail argument order. The delimiters are the tokenizer's,
// " ,;\"\t\n\r\x1a".
//
// Walk: zero a 24-byte command entry, hand the string to 0x257F0, then scan the remaining
// argument-definition list and fail if any node still carries the pending-required mask
// (flags & 0x3000000) == 0x2000000; the 0x6FC459E4 head <= 0 test is retail's sign-guard on it.
#include <cstdint>
#include <cstring>

namespace {

int32_t g_lastError;              // 0x6FC45388  last Storm error, mirrored to SetLastError
uint32_t g_argListHead;           // 0x6FC459E4  SCMD argument-definition list head

// unmapped internal callees, retail argument order
void Storm_Sub3B3A0(char** string, char* out, char* size, const char* delims, uint32_t* quoted);
bool Storm_Sub257F0(char** string, int32_t* entry, uint32_t* listHead, void* callback,
                    void* errorHandler);
void Storm_Sub24CD0(void* errorHandler);

uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
void SetLastError(uint32_t e) { g_lastError = (int32_t)e; }

const char kDelims[] = " ,;\"\t\n\r\x1a";

}  // namespace

// 1.13c Storm+0x25890: SCmdProcess. A null string pointer is 0x57; a non-zero second argument
// skips the first token first. 0x257F0 consumes the string and advances the local list head; the
// scan from there fails (return 0) while any node still has the 0x2000000 required-argument flag,
// and a failure calls the a4 error handler. Success returns 1.
extern "C"
int32_t Storm_Sub25890(char** string, uint32_t param_2, void* callback, void* errorHandler) {
    if (string == nullptr) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    if (param_2 != 0) Storm_Sub3B3A0(string, nullptr, nullptr, kDelims, nullptr);
    void* const savedHandler = errorHandler;
    int32_t entry[6] = {0, 0, 0, 0, 0, 0};
    uint32_t head = ((int32_t)g_argListHead < 1) ? 0 : g_argListHead;
    if (!Storm_Sub257F0(string, entry, &head, callback, errorHandler)) return 0;
    int32_t ok = 1;
    for (;;) {
        if (head == 0 || ok == 0) break;
        if ((rd32((void*)(uintptr_t)(head + 8)) & 0x3000000) != 0x2000000) {
            head = rd32((void*)(uintptr_t)(head + 4));
            if ((int32_t)head > 0) continue;
            head = 0;
        } else {
            ok = 0;
        }
        break;
    }
    if (savedHandler != nullptr && ok == 0) Storm_Sub24CD0(savedHandler);
    return ok;
}
