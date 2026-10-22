// src/core/d2launch/d2launch_batch_b57a.cpp -- UNMAPPED 1.13c D2Launch.dll alpha-lane rows (batch b57a),
// from build-wasm/ghidra-d2launch/decomp/0x<RVA>.c. Retail 32-bit FILE layout; callees are seams.
#include <cstdint>
#include <initializer_list>

struct B57aFile {            // VS2003 FILE, 0x20 bytes
    int32_t _ptr;            // char* in retail (32-bit)
    int32_t _cnt;
    int32_t _base;
    int32_t _flag;
    int32_t _file;
};

struct B57aPtd {             // _tiddata head
    uint32_t _tid, _thandle;
    uint8_t pad[0x14];
    uint32_t _holdrand;      // +0x1C
    uint8_t pad2[0x30];
    uint32_t _initaddr;      // +0x50
};

// Seams for the unmapped callees (0x43B8 _free, 0x5838 __write, 0x77C6 _commit, slot DAT_6fa64fa0 TlsFree, 0x3424 __mtdeletelocks).
static void (*b57a_free)(int32_t base) = nullptr;
static int32_t (*b57a_write)(int32_t fh, int32_t buf, uint32_t n) = nullptr;
static uint32_t (*b57a_commit)(int32_t fh) = nullptr;
static void (*b57a_tlsfree)(uint32_t idx) = nullptr;
static void (*b57a_deletelocks)() = nullptr;
static uint32_t b57a_tlsindex = 0xFFFFFFFFu;     // DAT_6fa60560
static const uint32_t b57a_initaddr = 0x6FA60590u; // &DAT_6fa60590

// 1.13c D2Launch+0x4547
extern "C" void d2launch_freebuf(B57aFile* f) {
    if ((f->_flag & 0x83) != 0 && (f->_flag & 8) != 0) {
        b57a_free(f->_base);
        f->_flag = (int32_t)((uint16_t)f->_flag & 0xfbf7);
        f->_ptr = 0;
        f->_base = 0;
        f->_cnt = 0;
    }
}

// 1.13c D2Launch+0x4572
extern "C" int d2launch_flush(B57aFile* f) {
    int r = 0;
    if (((uint8_t)f->_flag & 3) == 2 && (f->_flag & 0x108) != 0) {
        int32_t n = f->_ptr - f->_base;
        if (0 < n) {
            if ((uint32_t)b57a_write(f->_file, f->_base, (uint32_t)n) == (uint32_t)n) {
                if ((int8_t)f->_flag < 0) f->_flag &= 0xfffffffd;
            } else {
                f->_flag |= 0x20;
                r = -1;
            }
        }
    }
    f->_cnt = 0;
    f->_ptr = f->_base;
    return r;
}

// 1.13c D2Launch+0x45CF
extern "C" int d2launch_sub_45CF(B57aFile* f) {
    if (d2launch_flush(f) != 0) return -1;
    if ((f->_flag & 0x4000) != 0) return b57a_commit(f->_file) != 0 ? -1 : 0;
    return 0;
}

// 1.13c D2Launch+0x46E4
extern "C" void d2launch_mtterm() {
    if (b57a_tlsindex != 0xFFFFFFFFu) {
        b57a_tlsfree(b57a_tlsindex);
        b57a_tlsindex = 0xFFFFFFFFu;
    }
    b57a_deletelocks();
}

// 1.13c D2Launch+0x4701
extern "C" void d2launch_initptd(B57aPtd* p, void* /*locale*/) {
    p->_initaddr = b57a_initaddr;
    p->_holdrand = 1;
}
