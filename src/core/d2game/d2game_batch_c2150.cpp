// src/core/d2game/d2game_batch_c2150.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Alpha batch lane (OBJECTIVE
// v8.3): implementations + markers only, no spec, no vector. Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets. Register
// only arguments (Ghidra's in_AL / in_EAX / unaff_EDI / unaff_EBX / extraout_ECX) are explicit
// parameters and are named at each function. Unmapped callees are seams on D2GameBatchC2150Env in
// retail argument order; a seam whose decompile shows no arguments is taken to have none. The retail
// globals the decompiles name (0x6FD3A220 _pioinfo, 0x6FD3A214 _nfile, the 0x6FD39Fxx sbh block and
// 0x6FD31C3C) arrive as host-owned pointers, except where the 1.13c host model already fixes them in
// d2game_crt_lseek.cpp, which is reused here by declaration.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Retail addresses arrive as 32-bit integers in the decompile (unaff_EDI, in_EAX, piVar8, ...).
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8((const void*)(uintptr_t)a, o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void     wr8 (uint32_t a, uint32_t o, uint8_t v) { wr8((void*)(uintptr_t)a, o, v); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees and module globals the cached decompiles name but src/ does not implement.
struct D2GameBatchC2150Env {
    // ---- 0xC2150 ----
    void     (*ordinal10337)(uint32_t value);                       // 0xAB98 D2Common #10337
    // ---- 0x55840 ----
    uint32_t (*ordinal11153)(uint32_t handle, uint32_t zero);       // 0xA2100 D2Common #11153
    uint32_t (*ordinal10826)(uint32_t zero);                        // 0xA2DA D2Common #10826
    uint32_t (*ordinal10459)(uint32_t value);                       // 0xA2C2 D2Common #10459 (byte result)
    void     (*sub58620)(uint32_t seq);                             // 0x58620
    void     (*sub0FED0)(void);                                     // 0xFED0
    void     (*sub73720)(uint32_t unit);                            // 0x73720
    void     (*sub9B0A0)(uint32_t edi, uint32_t unit);              // 0x9B0A0
    void     (*sub0B2C0)(uint32_t unit);                            // 0xB2C0
    void     (*sub585C0)(uint32_t unit, uint32_t edi);              // 0x585C0
    uint32_t (*ordinal10920)(uint32_t unit);                        // 0xA4C0 D2Common #10920
    uint32_t (*ordinal10630)(void);                                 // 0xA556 D2Common #10630
    void     (*subCC2100)(uint32_t unit, uint32_t zero, int32_t value, uint32_t neg1); // 0xCC2100
    void     (*sub55090)(uint32_t edi, uint32_t unit, uint32_t one); // 0x55090
    const void* dataTables;                                         // *sgptDataTables_exref: +0xBC4 rows, +0xBC8 count
    // ---- 0x563C0 ----
    const uint32_t* guard6FD31C3C;                                  // DAT_6fd31c3c
    uint32_t (*sub76250)(uint32_t ebx, uint32_t p1, uint32_t p2, uint32_t* out); // 0x76250
    // ---- 0x9B630 ----
    uint32_t (*sub003A0)(uint32_t handle, uint32_t index);          // 0xD003A0
    uint32_t (*subDC840)(uint32_t handle, uint32_t unit, uint32_t inEax, uint32_t flag); // 0xCDC840
    void     (*subADDA0)(uint32_t inEax);                           // 0xCADDA0
    uint32_t (*ordinal11051)(void);                                 // 0xA8A4 D2Common #11051
    void     (*subAE090)(uint32_t inEax, uint32_t param1);          // 0xCAE090
    void     (*subADF70)(uint32_t param1);                          // 0xCADF70
    void     (*subAC810)(void);                                     // 0xCAC810
    void     (*subADF10)(uint32_t param1);                          // 0xCADF10
    void     (*subADEB0)(uint32_t param1);                          // 0xCADEB0
    void     (*subADE50)(uint32_t param1);                          // 0xCADE50
    void     (*subAD3D0)(void);                                     // 0xCAD3D0
    uint32_t (*ordinal10202)(void);                                 // Fog D2Common #10202
    void     (*subAADE0)(uint32_t ecx, uint32_t inEax, uint32_t param1, uint32_t flag, uint32_t value); // 0xCAADE0
    void     (*subAB0F0)(uint32_t param1, uint32_t zero);           // 0xCAB0F0
    // ---- 0x4E6E / 0x56BD / 0x8097 / 0x88DB CRT ----
    uint32_t (*getptd)(void);                                       // 0x3B93 __getptd
    int32_t  (*lseeki64lk)(uint32_t fd, int32_t lo, int32_t hi, uint32_t origin); // 0x4E77 __lseeki64_lk
    int32_t  (*writeFile)(uint32_t handle, const void* buf, uint32_t len,
                          uint32_t* written, uint32_t overlapped); // kernel32 WriteFile
    uint32_t (*getLastError)(void);                                 // kernel32 GetLastError
    void     (*dosmaperr)(uint32_t err);                            // 0x7832 __dosmaperr
    int32_t* (*errnoPtr)(void);                                     // 0x4E65 __getptd()->_terrno
    uint32_t* (*doserrnoPtr)(void);                                 // 0x4E6E __getptd()->_tdoserrno
    uint32_t* pioinfo;                     // 0x6FD3A220 _pioinfo, block addresses; ioinfo stride 0x24
    void     (*lock)(int32_t slot);                                 // 0x402B __lock
    void     (*unlock)(int32_t slot);                               // 0x8972 (0x28972, the finally)
    int32_t  (*crtInitCritSecAndSpinCount)(uint32_t cs, uint32_t spin); // 0x7DE3
    void     (*enterCriticalSection)(uint32_t cs);                  // kernel32 EnterCriticalSection
    uint32_t (*getOsfHandle)(uint32_t fd);                          // 0x40E4 __get_osfhandle
    uint32_t (*setFilePointer)(uint32_t handle, int32_t lo, uint32_t hiPtr,
                               uint32_t origin);                    // kernel32 SetFilePointer
    // ---- 0x5AE0 ----
    void     (*sub3F76)(uint32_t value);                            // 0x3F76 FUN_6fc23f76
    // ---- 0x8C1D / 0x8C48 sbh heap ----
    const uint32_t* listBase;              // 0x6FD39FBC DAT_6fd39fbc (region array address)
    uint32_t* listCount;                   // 0x6FD39FB8 DAT_6fd39fb8
    uint32_t* currentRegion;               // 0x6FD39FB4 DAT_6fd39fb4
    uint32_t* currentIndex;                // 0x6FD39FCC DAT_6fd39fcc
    uint32_t* savedBase;                   // 0x6FD39FC4 DAT_6fd39fc4
    uint32_t heapHandle;                   // 0x6FD3A20C DAT_6fd3a20c
    int32_t  (*virtualFree)(uint32_t addr, uint32_t size, uint32_t type); // kernel32 VirtualFree
    int32_t  (*heapFree)(uint32_t heap, uint32_t flags, uint32_t block);  // kernel32 HeapFree
    void     (*memmoveFn)(uint32_t dst, uint32_t src, uint32_t n);        // 0x2AD0 _memmove
    // ---- 0x5F4C ----
    void*    (*heapAlloc)(size_t size);                             // 0x5ED1 __heap_alloc
    int32_t  (*callnewh)(size_t size);                              // 0x9710 __callnewh
};

// Skill-state transition on the current game state's record. AL must be 1; the record at +0x10F4
// leads (one dereference) to a unit list walked by the +0xF4 link until the +0x00 id is 10, and that
// node's +0x18 sub-record has a one-shot flag pair at +2/+3. Only the flag byte at +3 triggers the
// D2Common #10337 call on the +0xC0 value, clearing +3 and setting +2.
// 1.13c D2Game+0xC2150
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_C2150(uint32_t param1, uint8_t inAL,
                                             const D2GameBatchC2150Env* env)
{
    if (inAL != 1) return;
    const uint32_t listHead = rd32(param1, 0x10F4);
    if (listHead == 0) return;
    uint32_t node = rd32(listHead, 0);
    if (node == 0) return;
    while (rd32(node, 0) != 10) {
        node = rd32(node, 0xF4);
        if (node == 0) return;
    }
    const uint32_t rec = rd32(node, 0x18);
    if (rd8(rec, 3) == 1) {
        env->ordinal10337(rd32(param1, 0xC0));
        wr8(rec, 3, 0);
        wr8(rec, 2, 1);
    }
}

// Client/unit creation for one game. param_1 is the caller's request record: +0x08 is the spawn
// selector (only < 7 proceeds) and +0x3D4 bit 0 is a set-once flag; +0x1AC byte is captured before
// the tail. EDI is the game state: +0x1C the unit handle, +0x70 a gate for bit 0x02000000 at
// result+0xC8, +0x90 the sequence counter, +0xBC an index table. The unit record gets +0 at 0, the
// selector at +4, the handle at +8, EDI at +0x80, bit 0x04000000 folded in at +0xC8, the
// D2Common #10459 byte at +0x18 (and its +0xBC table dword at +0x1C), +0x74 cleared and +0xC4
// OR'd with 0x10, +0xD0 set to 0xB. Afterwards the monster-stat record (D2Common #10920) has
// +0x74/+0x70 cleared and, when the pre-captured byte was 0 and the unit's +4 id is in range, the
// data-table row's +0xAC word (when non-zero and D2Common #10630 agrees) is sent through 0xCC2100
// and stored at stat+0x70. Both later calls take the game state first. *param_1 = unit; returns 0.
// 1.13c D2Game+0x55840
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_55840(uint32_t param1, uint32_t edi,
                                                 const D2GameBatchC2150Env* env)
{
    if (param1 == 0) d2game_fatal(0x206);

    const uint8_t selector = rd8(param1, 8);
    wr32(param1, 0x3D4, rd32(param1, 0x3D4) | 1);
    uint32_t unit = 0;
    if (selector < 7) {
        unit = env->ordinal11153(rd32(edi, 0x1C), 0);
        if (unit != 0) {
            wr32(unit, 0, 0);
            wr32(unit, 4, selector);
            wr32(unit, 8, rd32(edi, 0x1C));
            wr32(unit, 0x80, edi);
            uint32_t flags;
            if (rd32(edi, 0x70) == 0) flags = rd32(unit, 0xC8) & 0xFDFFFFFF;
            else                       flags = rd32(unit, 0xC8) | 0x02000000;
            wr32(unit, 0xC8, flags | 0x04000000);
            const uint8_t indexByte = (uint8_t)env->ordinal10459(env->ordinal10826(0));
            wr8(unit, 0x18, indexByte);
            wr32(unit, 0x1C, rd32(edi, 0xBC + (uint32_t)indexByte * 4));
            wr32(unit, 0x74, 0);
            wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x10);
            wr32(unit, 0xD0, 0xB);
            int32_t seq = (int32_t)rd32(edi, 0x90) + 1;
            if (seq == -1) seq = 1;
            wr32(edi, 0x90, (uint32_t)seq);
            env->sub58620((uint32_t)seq);
            env->sub0FED0();
        }
    }
    env->sub73720(unit);
    env->sub9B0A0(edi, unit);
    const uint8_t tailGate = rd8(param1, 0x1AC);
    env->sub0B2C0(unit);
    env->sub585C0(unit, edi);
    const uint32_t statList = env->ordinal10920(unit);
    wr32(statList, 0x74, 0);
    wr32(statList, 0x70, 0);
    if (tailGate == 0 && unit != 0) {
        const int32_t id = (int32_t)rd32(unit, 4);
        if (id >= 0 && id < (int32_t)rd32(env->dataTables, 0xBC8)) {
            const uint32_t row = (uint32_t)id * 0xC4 + rd32(env->dataTables, 0xBC4);
            if (row != 0 && rd16(row, 0xAC) != 0 && env->ordinal10630() != 0) {
                const int32_t value = (int16_t)rd16(row, 0xAC);
                env->subCC2100(unit, 0, value, 0xFFFFFFFF);
                wr32(statList, 0x70, (uint32_t)value);
            }
        }
    }
    env->sub55090(edi, unit, 1);
    wr32(param1, 0, unit);
    return 0;
}

// Guarded create: when the module guard dword is clear, 0x76250(EBX, param1, param2, &local) creates
// the record; a non-null local is then released through 0x55090(EBX, local, 0) and stored at *param3.
// With the guard set nothing is created: *param3 = 0 and the status 0xE is returned. param4..6 unused.
// 1.13c D2Game+0x563C0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_563C0(uint32_t param1, uint32_t param2, uint32_t outRecord,
                                                 uint32_t param4, uint32_t param5, uint32_t param6,
                                                 uint32_t ebx, const D2GameBatchC2150Env* env)
{
    (void)param4; (void)param5; (void)param6;
    if (*env->guard6FD31C3C == 0) {
        uint32_t created = 0;
        const uint32_t result = env->sub76250(ebx, param1, param2, &created);
        if (created != 0) env->sub55090(ebx, created, 0);
        wr32(outRecord, 0, created);
        return result;
    }
    wr32(outRecord, 0, 0);
    return 0xE;
}

// Monster AI dispatch on the acting unit (EAX) against the acting monster (param_1) and its own
// record (this). The monster's +0x3D4 bit 0 selects the cached +0x174 unit or, when clear, resolves
// it through 0xD003A0(+0x1A8, +0x170), clearing +0x174 on failure; the +0x1A8 handle then resolves
// the monster's target unit via +0x6C/+0x64. EAX's +0xC8 bits 2 and 4 are two direct attack modes,
// each gated on this == the resolved unit and a successful 0xCDC840. What follows is a chain of
// D2Common #11051 state probes; each non-zero probe with this == unit routes to its handler. The
// final block tests Fog D2Common #10202 three times, dispatching 0xCAADE0 with 0x100 / 0x200, and
// the third probe validates the EAX unit kind (+0x10: 0/1/2 allowed, and 2 rejected when this !=
// unit). Anything unhandled falls to 0xCAB0F0(param_1, 0); every path returns 1 except the explicit
// 0 returns of the third dispatch. ECX in is the register value 0xCAADE0 receives first.
// 1.13c D2Game+0x9B630
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_9B630(uint32_t self, uint32_t param1, uint32_t inEAX,
                                                 uint32_t ecxIn, const D2GameBatchC2150Env* env)
{
    if (param1 == 0) d2game_fatal(0xFB);

    uint32_t unit;
    if ((rd8(param1, 0x3D4) & 1) == 0) {
        if (rd32(param1, 0x174) == 0) {
            unit = 0;
        } else {
            unit = env->sub003A0(rd32(param1, 0x1A8), rd32(param1, 0x170));
            if (unit == 0) wr32(param1, 0x174, 0);
        }
    } else {
        unit = rd32(param1, 0x174);
    }
    const uint32_t handle = rd32(param1, 0x1A8);
    uint32_t target;
    if (rd8(unit, 0x6C) == 0) target = 0;
    else                      target = env->sub003A0(handle, rd32(unit, 0x64));

    if (inEAX != 0) {
        if ((rd32(inEAX, 0xC8) >> 2 & 1) != 0) {
            if (target != self) return 1;
            if (env->subDC840(handle, target, inEAX, 0) == 0) return 1;
            env->subADDA0(inEAX);
            return 1;
        }
        if ((rd32(inEAX, 0xC8) >> 4 & 1) != 0) {
            if (target != self) return 1;
            if (env->subDC840(handle, target, inEAX, 1) == 0) return 1;
            env->subADDA0(inEAX);
            return 1;
        }
    }

    uint32_t r = env->ordinal11051();
    if (r != 0 && self == unit) { env->subAE090(inEAX, param1); return 1; }
    r = env->ordinal11051();
    if (r == 0 || self != unit) {
        r = env->ordinal11051();
        if (r == 0) r = env->ordinal11051();
        if (r != 0 && self == unit) { env->subADF70(param1); return 1; }

        r = env->ordinal11051();
        if (r != 0 && self == unit) { env->subAC810(); return 1; }

        r = env->ordinal11051();
        if (r == 0) {
            r = env->ordinal11051();
            if (r == 0) {
                r = env->ordinal11051();
                if (r == 0) {
                    r = env->ordinal11051();
                    if (r == 0) {
                        r = env->ordinal11051();
                        if (r == 0) {
                            r = env->ordinal11051();
                            if (r != 0 && self == unit) { env->subADF10(param1); return 1; }
                            r = env->ordinal11051();
                            if (r != 0 && self == unit) { env->subADEB0(param1); return 1; }
                            r = env->ordinal11051();
                            if (r == 0) r = env->ordinal11051();
                            if (r != 0 && self == unit) { env->subADE50(param1); return 1; }
                            r = env->ordinal11051();
                            if (r != 0 && self == unit) { env->subAD3D0(); return 1; }
                            r = env->ordinal11051();
                            if (r != 0 && self == unit) { env->subAD3D0(); return 1; }
                            r = env->ordinal11051();
                            if (r != 0 && self == unit) { env->subAD3D0(); return 1; }
                            r = env->ordinal11051();
                            if (r == 0) {
                                r = env->ordinal11051();
                                if (r == 0) {
                                    r = env->ordinal11051();
                                    if (r == 0) {
                                        r = env->ordinal10202();
                                        if (r != 0) {
                                            if (self != unit &&
                                                (inEAX == 0 || rd32(inEAX, 0x10) != 1))
                                                return 0;
                                            env->subAADE0(ecxIn, inEAX, param1, 0x100,
                                                          env->ordinal10202());
                                            return 1;
                                        }
                                        r = env->ordinal10202();
                                        if (r != 0) {
                                            if (self != unit) {
                                                if (inEAX == 0) return 0;
                                                if (rd32(inEAX, 0x10) != 1) return 0;
                                            }
                                            env->subAADE0(ecxIn, inEAX, param1, 0x200,
                                                          env->ordinal10202());
                                            return 1;
                                        }
                                        r = env->ordinal10202();
                                        if (r == 0) return 0;
                                        const int32_t kind = (inEAX == 0) ? 0
                                                                          : (int32_t)rd32(inEAX, 0x10);
                                        if (self != unit) {
                                            if (kind == 0) return 0;
                                            if (kind == 2) return 0;
                                        }
                                        if (kind != 0 && kind != 1 && kind != 2) return 0;
                                        if (self == unit && env->ordinal10202() != 0) return 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    env->subAB0F0(param1, 0);
    return 1;
}

// _doserrno: the per-thread data block (0x3B93) plus the VS2003 _ptiddata offset 0x0C (the long
// after _terrno at 0x08). Returns the address, never the value.
// 1.13c D2Game+0x4E6E
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t* d2_fn_d2game_4E6E(const D2GameBatchC2150Env* env)
{
    return (uint32_t*)(uintptr_t)(env->getptd() + 0x0C);
}

// __write_nolock(fh, buf, count). The ioinfo record is _pioinfo[fh >> 5] + (fh & 0x1F) * 0x24:
// +0x00 the OS handle, +0x04 the flags. Flag 0x20 (FAPPEND) seeks to end first. Without 0x80 (FTEXT)
// one WriteFile; with it the buffer is copied 0x400 bytes at a time inserting '\r' before every '\n',
// and the write repeats while the produced length fits the written count and input remains. A
// non-zero total returns total - newlines. Otherwise a pending error maps to errno/doserrno (error 5
// -> 9 / 5, else __dosmaperr) and returns -1; with no pending error, flag 0x40 with a leading 0x1A
// byte returns 0, else errno 0x1C / doserrno 0 and -1.
// 1.13c D2Game+0x56BD
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_56BD(uint32_t fd, const char* buf, uint32_t count,
                                               const D2GameBatchC2150Env* env)
{
    if (count == 0) return 0;

    uint8_t* io = (uint8_t*)(uintptr_t)(env->pioinfo[(int32_t)fd >> 5] + (fd & 0x1F) * 0x24);
    if ((io[4] & 0x20) != 0) env->lseeki64lk(fd, 0, 0, 2);
    const uint32_t handle = rd32(io, 0);
    uint32_t total = 0, newlines = 0, written = 0, error = 0;

    if ((io[4] & 0x80) == 0) {
        if (!env->writeFile(handle, buf, count, &written, 0)) {
            error = env->getLastError();
        } else {
            total = written;
        }
    } else {
        const char* src = buf;
        uint8_t textBuf[0x402];   // retail writes up to 0x400 plus a '\r' pair past the boundary
        uint8_t* dst;
        do {
            uint32_t produced = 0;
            uint32_t offset = (uint32_t)(src - buf);
            dst = textBuf;
            do {
                if (count <= offset) break;
                const char c = *src++;
                ++offset;
                if (c == '\n') { ++newlines; *dst++ = '\r'; ++produced; }
                *dst++ = (uint8_t)c;
                ++produced;
            } while (produced < 0x400);
            if (!env->writeFile(handle, textBuf, (uint32_t)(dst - textBuf), &written, 0)) {
                error = env->getLastError();
                break;
            }
            total += written;
        } while (((uint32_t)(dst - textBuf) <= written) &&
                 ((uint32_t)(src - buf) < count));
    }

    if (total != 0) return (int32_t)(total - newlines);
    if (error != 0) {
        if (error == 5) {
            *env->errnoPtr() = 9;
            *env->doserrnoPtr() = 5;
            return -1;
        }
        env->dosmaperr(error);
        return -1;
    }
    if ((io[4] & 0x40) != 0 && *buf == '\x1A') return 0;
    *env->errnoPtr() = 0x1C;
    *env->doserrnoPtr() = 0;
    return -1;
}

// __lock_fhandle(fh): the ioinfo record is _pioinfo[fh >> 5] + (fh & 0x1F) * 0x24. Its +0x08 slot is
// created under __lock(10) when zero: __crtInitCritSecAndSpinCount(record + 0xC, 4000) failure returns
// 0 (the SEH unwind skips the unlock), otherwise the slot is incremented and __unlock(10) runs.
// EnterCriticalSection on record + 0xC always follows; returns 1.
// 1.13c D2Game+0x88DB
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_88DB(int32_t fileHandle, const D2GameBatchC2150Env* env)
{
    const uint32_t io = (uint32_t)(uintptr_t)(env->pioinfo[(int32_t)fileHandle >> 5] +
                                              (uint32_t)(fileHandle & 0x1F) * 0x24);
    if (rd32(io, 8) == 0) {
        env->lock(10);
        if (rd32(io, 8) == 0) {
            if (env->crtInitCritSecAndSpinCount(io + 0xC, 4000) == 0) return 0;
            wr32(io, 8, rd32(io, 8) + 1);
        }
        env->unlock(10);
    }
    env->enterCriticalSection(io + 0xC);
    return 1;
}

// __lseek_lk(fh, offset, origin): __get_osfhandle(fh) == -1 sets errno EBADF (9) and returns -1.
// Otherwise SetFilePointer(hFile, offset, NULL, origin); on -1 GetLastError decides: no error clears
// ioinfo +0x04 bit 0x02 (FEOFLAG), any other error goes through __dosmaperr and the result is -1.
// 1.13c D2Game+0x8097
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_8097(uint32_t fd, int32_t offset, uint32_t origin,
                                                const D2GameBatchC2150Env* env)
{
    const uint32_t hFile = env->getOsfHandle(fd);
    if (hFile == 0xFFFFFFFF) {
        *env->errnoPtr() = 9;
        return 0xFFFFFFFF;
    }
    uint32_t position = env->setFilePointer(hFile, offset, 0, origin);
    const uint32_t error = (position == 0xFFFFFFFF) ? env->getLastError() : 0;
    if (error == 0) {
        uint8_t* osfile = (uint8_t*)(uintptr_t)(env->pioinfo[(int32_t)fd >> 5] + 4 +
                                                (fd & 0x1F) * 0x24);
        *osfile &= 0xFD;
    } else {
        env->dosmaperr(error);
        position = 0xFFFFFFFF;
    }
    return position;
}

// Tail of the CRT lock/unlock pair: FUN_6fc23f76(4).
// 1.13c D2Game+0x5AE0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5AE0(const D2GameBatchC2150Env* env)
{
    env->sub3F76(4);
}

// ___sbh_find_block(ptr): walk the region array [0x6FD39FBC, 0x6FD39FBC + 0x6FD39FB8 * 0x14) in
// 0x14-byte steps; the first region whose +0x0C base makes (ptr - base) an unsigned value below
// 0x100000 is returned. Past the end returns 0.
// 1.13c D2Game+0x8C1D
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_8C1D(int32_t ptr, const D2GameBatchC2150Env* env)
{
    const uint32_t base = *env->listBase;
    uint32_t region = base;
    for (;;) {
        if (base + *env->listCount * 0x14 <= region) return 0;
        if ((uint32_t)(ptr - (int32_t)rd32(region, 0xC)) < 0x100000) break;
        region += 0x14;
    }
    return region;
}

// Clears one size bucket's bit and per-bucket count in one of the two bitmaps. param1 is the sbh
// record, uVar6 its +0x10 base and uVar14 the region index; buckets below 0x20 use the +0x44 / +0x00
// word pair, the rest the +0xC4 / +0x04 pair. A count that reaches zero clears the owner bitmap word.
static void sbh_clear_bucket(uint32_t param1, uint32_t uVar6, uint32_t uVar14, uint32_t bucket)
{
    const uint32_t off = bucket + 4 + uVar6;
    if (bucket < 0x20) {
        const uint32_t mask = ~(0x80000000u >> (bucket & 0x1F));
        const uint32_t map = uVar6 + 0x44 + uVar14 * 4;
        wr32(map, 0, rd32(map, 0) & mask);
        wr8(off, 0, (uint8_t)(rd8(off, 0) - 1));
        if (rd8(off, 0) == 0) wr32(param1, 0, rd32(param1, 0) & mask);
    } else {
        const uint32_t mask = ~(0x80000000u >> ((bucket - 0x20) & 0x1F));
        const uint32_t map = uVar6 + 0xC4 + uVar14 * 4;
        wr32(map, 0, rd32(map, 0) & mask);
        wr8(off, 0, (uint8_t)(rd8(off, 0) - 1));
        if (rd8(off, 0) == 0) wr32(param1, 4, rd32(param1, 4) & mask);
    }
}

// ___sbh_free_block(record, ptr): frees the block whose 4-byte header sits at ptr-4. The size word is
// header[0] - 1; an odd size is left alone (already free), an even one coalesces with the following
// block when that one is free, unlinking it from its bucket list and clearing its bitmap only when it
// was that bucket's sole entry. The previous block (ptr-8's word) is merged the same way when even,
// after which the merged block is inserted at the head of its size bucket (bucket = min(0x3F,
// size >> 4 - 1)), bumping the bucket count and setting the bitmap bit on the first entry. The
// region's free count at record+0x144 + index*0x204 is decremented; at zero the current sbh region
// [0x6FD39FB4] is released (VirtualFree of index*0x8000 + region[3], bitmap bit, map slot, count),
// and when its bitmap is all ones the region array entry itself is freed and the array memmoved down
// by 0x14, count decremented, param1 adjusted past the removed entry on a smaller address, and
// 0x6FD39FC4 set to the array base. The freed region/index become the current ones.
// 1.13c D2Game+0x8C48
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8C48(uint32_t param1, uint32_t param2,
                                            const D2GameBatchC2150Env* env)
{
    const uint32_t base10 = rd32(param1, 0x10);
    uint32_t block = param2 - 4;
    const uint32_t regionIndex = (param2 - rd32(param1, 0xC)) >> 0xF;
    const uint32_t freeCountPtr = regionIndex * 0x204 + 0x144 + base10;
    uint32_t size = rd32(block, 0) - 1;

    if ((size & 1) == 0) {
        uint32_t next = size + block;
        const uint32_t nextSize = rd32(next, 0);
        const uint32_t prevSize = rd32(param2 - 8, 0);

        if ((nextSize & 1) == 0) {
            uint32_t bucket = (uint32_t)(((int32_t)nextSize >> 4) - 1);
            if (bucket > 0x3F) bucket = 0x3F;
            if (rd32(next, 4) == rd32(next, 8)) sbh_clear_bucket(param1, base10, regionIndex, bucket);
            size += nextSize;
            wr32(rd32(next, 8) + 4, 0, rd32(next, 4));
            wr32(rd32(next, 4) + 8, 0, rd32(next, 8));
        }

        uint32_t newBucket = (uint32_t)(((int32_t)size >> 4) - 1);
        if (newBucket > 0x3F) newBucket = 0x3F;
        uint32_t prevBucket = param1;
        if ((prevSize & 1) == 0) {
            block -= prevSize;
            prevBucket = (uint32_t)(((int32_t)prevSize >> 4) - 1);
            if (prevBucket > 0x3F) prevBucket = 0x3F;
            size += prevSize;
            newBucket = (uint32_t)(((int32_t)size >> 4) - 1);
            if (newBucket > 0x3F) newBucket = 0x3F;
            if (prevBucket != newBucket) {
                if (rd32(block, 4) == rd32(block, 8))
                    sbh_clear_bucket(param1, base10, regionIndex, prevBucket);
                wr32(rd32(block, 8) + 4, 0, rd32(block, 4));
                wr32(rd32(block, 4) + 8, 0, rd32(block, 8));
            }
        }
        if ((prevSize & 1) != 0 || prevBucket != newBucket) {
            const uint32_t bucketHead = freeCountPtr + newBucket * 8;
            const uint32_t nextLink = rd32(bucketHead, 4);
            wr32(block, 8, bucketHead);
            wr32(block, 4, nextLink);
            wr32(bucketHead, 4, block);
            wr32(rd32(block, 4) + 8, 0, block);
            if (rd32(block, 4) == rd32(block, 8)) {
                const char count = (char)rd8(newBucket + base10 + 4, 0);
                wr8(newBucket + base10 + 4, 0, (uint8_t)(count + 1));
                const uint8_t bucket8 = (uint8_t)newBucket;
                if (newBucket < 0x20) {
                    if (count == 0) wr32(param1, 0, rd32(param1, 0) | (0x80000000u >> (bucket8 & 0x1F)));
                    const uint32_t map = base10 + 0x44 + regionIndex * 4;
                    wr32(map, 0, rd32(map, 0) | (0x80000000u >> (bucket8 & 0x1F)));
                } else {
                    if (count == 0) wr32(param1, 4, rd32(param1, 4) | (0x80000000u >> ((bucket8 - 0x20) & 0x1F)));
                    const uint32_t map = base10 + 0xC4 + regionIndex * 4;
                    wr32(map, 0, rd32(map, 0) | (0x80000000u >> ((bucket8 - 0x20) & 0x1F)));
                }
            }
        }
        wr32(block, 0, size);
        wr32(block + size - 4, 0, size);
        wr32(freeCountPtr, 0, rd32(freeCountPtr, 0) - 1);
        if (rd32(freeCountPtr, 0) == 0) {
            uint32_t current = *env->currentRegion;
            if (current != 0) {
                const uint32_t index = *env->currentIndex;
                env->virtualFree(rd32(current, 0xC) + index * 0x8000, 0x8000, 0x4000);
                wr32(current, 8, rd32(current, 8) | (0x80000000u >> (index & 0x1F)));
                wr32(rd32(current, 0x10) + 0xC4 + index * 4, 0, 0);
                const uint32_t control = rd32(current, 0x10) + 0x43;
                wr8(control, 0, (uint8_t)(rd8(control, 0) - 1));
                if (rd8(control, 0) == 0) wr32(current, 4, rd32(current, 4) & 0xFFFFFFFE);
                if (rd32(current, 8) == 0xFFFFFFFF) {
                    env->virtualFree(rd32(current, 0xC), 0, 0x8000);
                    env->heapFree(env->heapHandle, 0, rd32(current, 0x10));
                    const uint32_t count = *env->listCount;
                    const uint32_t arrayBase = *env->listBase;
                    const int32_t length = (int32_t)(count * 0x14 - current) - 0x14 + (int32_t)arrayBase;
                    env->memmoveFn(current, current + 0x14, (uint32_t)length);
                    *env->listCount = count - 1;
                    if (current < param1) param1 -= 0x14;
                    *env->savedBase = arrayBase;
                }
            }
            *env->currentRegion = param1;
            *env->currentIndex = regionIndex;
        }
    }
}

// __nh_malloc(size, nhFlag): below 0xFFFFFFE1, retry __heap_alloc(size) while it fails and nhFlag is
// non-zero and __callnewh(size) accepts the request. Returns the allocation or 0.
// 1.13c D2Game+0x5F4C
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_5F4C(size_t size, int32_t nhFlag,
                                             const D2GameBatchC2150Env* env)
{
    if (size < 0xFFFFFFE1u) {
        for (;;) {
            void* p = env->heapAlloc(size);
            if (p != 0) return p;
            if (nhFlag == 0) return 0;
            if (env->callnewh(size) == 0) break;
        }
    }
    return 0;
}
