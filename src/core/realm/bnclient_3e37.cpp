// 1.13c BNClient+0x03E37 (356 B). __XcptFilter(num, pxcptinfo): the VC7.1 CRT exception filter.
// Own file. The rule is a pure function of the exception number, the EXCEPTION_POINTERS word, the per-thread
// _ptiddata fields it reads and writes, and the table globals; the external effects (UnhandledExceptionFilter
// and the registered action) are seams, so the rule runs without a process.
#include <cstdint>

// The _ptiddata fields this rule touches (retail layout names): _initaddr, _initarg, _pxcptacttab.
struct BnPtd {
    uint32_t* initaddr;      // table of 3-dword entries: { exception number, action type, registered action }
    uint32_t  initarg;       // _initarg: holds the EXCEPTION_POINTERS word while an action runs
    uint32_t  pxcptacttab;   // _pxcptacttab: the signal code handed to a type-8 action
};

// Retail globals: DAT_6ff3d55c = table count, DAT_6ff3d550 = first entry to clear, DAT_6ff3d554 = clear count.
struct BnXcptGlobals {
    uint32_t count;
    uint32_t clear_first;
    uint32_t clear_n;
};

struct BnXcptSeams {
    int32_t (*unhandled_filter)(uint32_t pxcptinfo);     // UnhandledExceptionFilter (kernel32)
    void    (*call1)(uint32_t fn, uint32_t a);           // (*action)(type)
    void    (*call2)(uint32_t fn, uint32_t a, uint32_t b); // (*action)(8, signal code)
};

// Retail order:
//  1. scan entries (stride 3 dwords) for num, bounded by the table count; no match -> null;
//  2. no entry, or its action is NULL -> UnhandledExceptionFilter(pxcptinfo);
//  3. action == 5 -> clear it, return 1; action == 1 -> return -1;
//  4. otherwise _initarg = pxcptinfo; type 8 clears the action slots of entries
//     [clear_first, clear_first+clear_n), maps the NT status to a signal code (no default: the
//     saved _pxcptacttab stays) and calls (action, 8, code), then restores _pxcptacttab;
//     any other type clears the entry's action and calls (action, type). Restore _initarg; return -1.
extern "C" int32_t bn_xcpt_filter_3e37_seam(uint32_t num, uint32_t pxcptinfo, BnPtd* ptd,
                                            const BnXcptGlobals* g, const BnXcptSeams* s) {
    uint32_t* base = ptd->initaddr;
    uint32_t* e = base;
    uint32_t* end = base + g->count * 3;
    do {
        if (e[0] == num) break;
        e += 3;
    } while (e < end);
    if (e >= end || e[0] != num) e = nullptr;

    if (e == nullptr || e[2] == 0) return s->unhandled_filter(pxcptinfo);
    if (e[2] == 5) { e[2] = 0; return 1; }
    if (e[2] == 1) return -1;

    uint32_t fn = e[2];
    uint32_t saved_arg = ptd->initarg;
    ptd->initarg = pxcptinfo;
    if (e[1] == 8) {
        for (uint32_t i = g->clear_first; i < g->clear_first + g->clear_n; i++) base[i * 3 + 2] = 0;
        uint32_t saved_sig = ptd->pxcptacttab;
        switch (num) {
            case 0xc000008eu: ptd->pxcptacttab = 0x83; break;   // divide by zero
            case 0xc0000090u: ptd->pxcptacttab = 0x81; break;   // invalid operation
            case 0xc0000091u: ptd->pxcptacttab = 0x84; break;   // overflow
            case 0xc0000093u: ptd->pxcptacttab = 0x85; break;   // underflow
            case 0xc000008du: ptd->pxcptacttab = 0x82; break;   // denormal
            case 0xc000008fu: ptd->pxcptacttab = 0x86; break;   // inexact
            case 0xc0000092u: ptd->pxcptacttab = 0x8a; break;   // stack overflow
            default: break;
        }
        s->call2(fn, 8, ptd->pxcptacttab);
        ptd->pxcptacttab = saved_sig;
    } else {
        e[2] = 0;
        s->call1(fn, e[1]);
    }
    ptd->initarg = saved_arg;
    return -1;
}
