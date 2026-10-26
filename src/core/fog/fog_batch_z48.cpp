// Fog batch z48: 0x1670-0x17D0 (pointer-array container) and 0x1A22-0x1BBA (CRT exit/init glue).
// Retail globals and callees are seams; 32-bit links are uint32_t.
#include <cstdint>

struct FogZ48Env {
    uint32_t (*alloc)(uint32_t size, const char* tag, int line, int flag);            // Fog ord 401
    uint32_t (*realloc_)(uint32_t p, uint32_t size, const char* tag, int line, int f); // ord 405
    void (*free_)(uint32_t p, const char* tag, int line, int flag);                   // ord 403
    void (*arr_release)(uint32_t* a);      // Fog+0x1520
    void (*arr_1400)(int a);               // Fog+0x1400
    void (*lock)(int n);                   // __lock (Fog+0x3CF4)
    void (*unlock)(int n);                 // Fog+0x3C3F
    void (*terminate_self)(uint32_t code); // GetCurrentProcess+TerminateProcess
    void (*exit_process)(uint32_t code);   // ExitProcess
    void (*cor_exit_process)(uint32_t code); // mscoree CorExitProcess if present, else null
};

// 1.13c FOG+0x01670 -- SEH wrapper around Fog+0x1400.
void fog_01670(const FogZ48Env* e, int a) { e->arr_1400(a); }

// 1.13c FOG+0x016B0 -- dtor: reset vtable (PTR_LAB_6ff78388), release array at +4.
// vtable store is a seam out-param (retail image address, not ours).
void fog_016B0(const FogZ48Env* e, uint32_t* self, uint32_t vtbl)
{
    self[0] = vtbl;
    if (self[1]) e->arr_release((uint32_t*)(uintptr_t)self[1]);
}

// 1.13c FOG+0x01750 -- EAX=this; free this+8 with tag "PAX" if set.
void fog_01750(const FogZ48Env* e, uint32_t* self)
{
    if (self[2]) e->free_(self[2], "PAX", -2, 0);
}

// 1.13c FOG+0x017D0 -- ESI=this, resize pointer array to `n` entries, copy min(old count, n).
// this: [0]=capacity [1]=count [2]=data (32-bit ptr, flat model via mem seam).
void fog_017D0(const FogZ48Env* e, uint32_t* self, uint32_t n, uint32_t* (*mem)(uint32_t))
{
    uint32_t old = self[2];
    self[0] = n;
    uint32_t p = e->realloc_(old, n * 4, "PAX", -2, 0x10);
    self[2] = p;
    if (p) return;
    p = e->alloc(n * 4, "PAX", -2, 0);
    self[2] = p;
    if (!old) return;
    uint32_t m = self[1] <= n ? self[1] : n;
    for (uint32_t i = 0; i < m; i++) {
        uint32_t* d = mem(p + i * 4);
        if (d) *d = *mem(old + i * 4);
    }
    e->free_(old, "PAX", -2, 0);
}

// 1.13c FOG+0x01A22 -- ___crtExitProcess
void fog_01A22(const FogZ48Env* e, uint32_t code)
{
    if (e->cor_exit_process) e->cor_exit_process(code);
    e->exit_process(code);
}

// 1.13c FOG+0x01A52 -- lock(8)
void fog_01A52(const FogZ48Env* e) { e->lock(8); }

// 1.13c FOG+0x01A5B -- unlock(8)
void fog_01A5B(const FogZ48Env* e) { e->unlock(8); }

typedef void (*FogInitFn)();

// 1.13c FOG+0x01A64 -- __initterm: EAX=first, param=end; call non-null entries.
void fog_01A64(FogInitFn* first, FogInitFn* end)
{
    for (; first < end; first++) if (*first) (*first)();
}

// 1.13c FOG+0x01A7C -- __cinit. Tables: DAT_6ff7e018..e030 (int-returning), e000..e014 (void).
// PARTIAL: _atexit(LAB_6ff53d69) registration is the atexit seam.
int fog_01A7C(int arg, void (*fpmath)(int), int (**itab)(), int (**itab_end)(),
              void (*atexit_reg)(), FogInitFn* vtab, FogInitFn* vtab_end)
{
    if (fpmath) fpmath(arg);
    int r = 0;
    for (; itab < itab_end; itab++) {
        if (r) return r;
        if (*itab) r = (*itab)();
    }
    if (r) return r;
    atexit_reg();
    fog_01A64(vtab, vtab_end);
    return 0;
}

struct FogDoexitState {
    int terminating_flag;      // DAT_6ff80244
    int doexit_flag;           // DAT_6ff80240
    uint8_t quick_flag;        // DAT_6ff8023c
    FogInitFn *atexit_begin, *atexit_cur; // DAT_6ffa8298 / DAT_6ffa8294
    FogInitFn *term_a, *term_a_end;       // DAT_6ff7e03c range
    FogInitFn *term_b, *term_b_end;       // DAT_6ff7e048 range
};

// 1.13c FOG+0x01B95 -- doexit epilogue: EBP+0x10 slot (locked flag) vs EDI (0): unlock(8) if nonzero.
// PARTIAL: register-carried operands; modelled as `locked` = the [ebp+0x10] value (EDI is 0).
void fog_01B95(const FogZ48Env* e, int locked)
{
    if (locked != 0) e->unlock(8);
}

// 1.13c FOG+0x01AE6 -- _doexit(code, quick, retain). term ranges' end addresses guessed from
// the adjacent table layout (term_a_end = 0x...e048 start of term_b).
void fog_01AE6(const FogZ48Env* e, FogDoexitState* s, uint32_t code, int quick, int retain)
{
    e->lock(8);
    if (s->terminating_flag == 1) e->terminate_self(code);
    s->doexit_flag = 1;
    s->quick_flag = (uint8_t)retain;
    if (quick == 0 && s->atexit_begin) {
        while (--s->atexit_cur, s->atexit_begin <= s->atexit_cur)
            if (*s->atexit_cur) (*s->atexit_cur)();
        fog_01A64(s->term_a, s->term_a_end);
    }
    fog_01A64(s->term_b, s->term_b_end);
    fog_01B95(e, 1);
    if (retain) return;
    s->terminating_flag = 1;
    fog_01A22(e, code);
}

// 1.13c FOG+0x01BBA -- __exit: doexit(code, 1, 0)
void fog_01BBA(const FogZ48Env* e, FogDoexitState* s, uint32_t code) { fog_01AE6(e, s, code, 1, 0); }
