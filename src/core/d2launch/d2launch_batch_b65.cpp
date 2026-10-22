// src/core/d2launch/d2launch_batch_b65.cpp -- UNMAPPED 1.13c D2Launch.dll alpha-lane rows (batch b65).
// Retail 32-bit layouts; callees are seams. Skipped: SEH prolog/epilog (36DC,3717), printf/scanf cores
// (1D45,2517: CRT), __mtinit (48FB: direct Win32).
#include <cstdint>
#include <cstddef>
#include <initializer_list>

struct B65File { int32_t _ptr, _cnt, _base, _flag, _file; };   // VS2003 FILE

// Seams
static void (*b65_lock_file)(B65File*) = nullptr;               // 0x3350-area __lock_file
static void (*b65_unlock_file)(B65File*) = nullptr;             // 0x3940
static size_t (*b65_fread_lk)(void*, size_t, size_t, B65File*) = nullptr;
static int (*b65_fclose_lk)(B65File*) = nullptr;                // 0x1869
static int (*b65_crt_init)(uint32_t, int) = nullptr;            // __CRT_INIT@12
static int (*b65_dll_main)(uint32_t, int, uint32_t) = nullptr;  // DAT_6fa680c8
static int (*b65_dll_attach)(uint32_t) = nullptr;               // 0x9B70
static int32_t b65_dll_gate = 0;                                // DAT_6fa64e28
static int32_t b65_run_mode = 0, b65_run_mode2 = 0;             // DAT_6fa64e34 / e38
static void (*b65_ff_msgbanner)() = nullptr;                    // 0x166C
static void (*b65_nmsg_write)(int) = nullptr;                   // 0x53AC
static void (*b65_exit)(int) = nullptr;                         // PTR___exit
static void (*b65_write_char)(int32_t ch, B65File* f, int* charsout) = nullptr;  // 0x1CB7
static void (*b65_freefls)(void*) = nullptr;                    // 0x4785
static uint32_t b65_tls_index = 0xFFFFFFFFu;                    // DAT_6fa60560
static void* (*b65_fls_get)(uint32_t) = nullptr;                // DAT_6fa64f98
static int (*b65_fls_set)(uint32_t, void*) = nullptr;           // DAT_6fa64f9c

// 1.13c D2Launch+0x17E4
size_t b65_fread(void* dst, size_t elemSize, size_t count, B65File* f) {
    b65_lock_file(f);
    size_t r = b65_fread_lk(dst, elemSize, count, f);
    b65_unlock_file(f);   // FUN_6fa41826 (SEH-cleanup funclet)
    return r;
}

// 1.13c D2Launch+0x1826
// Funclet: unaff_EBP+0x14 is the FILE* arg of the enclosing _fread frame; explicit param here.
void b65_fread_unlock(B65File* file /* hidden: [EBP+0x14] */) { b65_unlock_file(file); }

// 1.13c D2Launch+0x18B5
int b65_fclose(B65File* f) {
    int r = -1;
    if ((f->_flag & 0x40) == 0) {
        b65_lock_file(f);
        r = b65_fclose_lk(f);
        b65_unlock_file(f);   // FUN_6fa418fe
    } else {
        f->_flag = 0;
    }
    return r;
}

// 1.13c D2Launch+0x1A87  (DLL entry)
int b65_dll_entry(uint32_t hinst, int reason, uint32_t reserved) {
    int r = 1;
    if (reason == 0 && b65_dll_gate == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (b65_dll_main) r = b65_dll_main(hinst, reason, reserved);
        if (r == 0 || b65_crt_init(hinst, reason) == 0) return 0;
    }
    r = b65_dll_attach(hinst);
    if (reason == 1 && r == 0) b65_crt_init(hinst, 0);
    if (reason == 0 || reason == 3) {
        if (b65_crt_init(hinst, reason) == 0) r = 0;
        if (r != 0 && b65_dll_main) r = b65_dll_main(hinst, reason, reserved);
    }
    return r;
}

// 1.13c D2Launch+0x1B6B
void b65_amsg_exit(int code) {
    if (b65_run_mode == 1 || (b65_run_mode == 0 && b65_run_mode2 == 1)) b65_ff_msgbanner();
    b65_nmsg_write(code);
    b65_exit(0xff);
}

// 1.13c D2Launch+0x1D0E
// Hidden register inputs: EAX -> charsout pointer, EDI -> stream; stack arg is the count.
void b65_write_string(int* charsout /* EAX */, B65File* stream /* EDI */, int count) {
    if ((stream->_flag & 0x40) == 0 || stream->_base != 0) {
        do {
            if (count < 1) return;
            --count;
            b65_write_char(0, stream, charsout);   // write_char consumes the next string char via regs
        } while (*charsout != -1);
    } else {
        *charsout += count;
    }
}

// 1.13c D2Launch+0x48CC
void b65_freeptd(void* ptd) {
    if (b65_tls_index != 0xFFFFFFFFu) {
        if (!ptd) ptd = b65_fls_get(b65_tls_index);
        b65_freefls(ptd);
        b65_fls_set(b65_tls_index, nullptr);
    }
}
