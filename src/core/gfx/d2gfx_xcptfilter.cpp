#include <cstddef>
#include <cstdint>

struct D2GfxXcptAction {
    uint32_t exception_num;
    uint32_t signal;
    uint32_t handler;
};
static_assert(sizeof(D2GfxXcptAction) == 0x0c, "VC7.1 exception action record");

struct D2GfxXcptPtd {
    uint32_t action_table;   // _ptiddata._initaddr, retail x86 pointer
    uint32_t initarg;        // _ptiddata._initarg
    uint32_t signal_number;  // _ptiddata._pxcptacttab
};
static_assert(offsetof(D2GfxXcptPtd, action_table) == 0x00, "_ptiddata._initaddr");
static_assert(offsetof(D2GfxXcptPtd, initarg) == 0x04, "_ptiddata._initarg");
static_assert(offsetof(D2GfxXcptPtd, signal_number) == 0x08, "_ptiddata._pxcptacttab");

struct D2GfxXcptFilterEnv {
    D2GfxXcptPtd* (*getptd)(void);
    D2GfxXcptAction* (*resolve_action_table)(uint32_t retail_address);
    int32_t (*unhandled_exception_filter)(uint32_t exception_ptrs);
    void (*call_handler)(uint32_t handler, uint32_t first_arg, uint32_t second_arg);
    uint32_t table_length;  // DAT_6fa90224: number of 12-byte action records
    uint32_t zero_first;    // DAT_6fa90218: first record in the signal-8 disarm window
    uint32_t zero_count;    // DAT_6fa9021c: number of records in that window
};

// 1.13c D2Gfx+0x0348D (__XcptFilter): search the per-thread CRT action records for the exception.
// Missing actions delegate to UnhandledExceptionFilter; handler 5 disarms itself and returns 1,
// handler 1 returns -1, and other handlers are invoked after the retail PTD scratch fields and
// signal-8 action window are updated. The 1.13c code-to-signal mapping is retained verbatim.
extern "C" int32_t d2gfx_xcpt_filter(uint32_t exception_num, uint32_t exception_ptrs,
                                      const D2GfxXcptFilterEnv* env) {
    D2GfxXcptPtd* ptd = env->getptd();
    D2GfxXcptAction* table = env->resolve_action_table(ptd->action_table);
    D2GfxXcptAction* found = nullptr;
    for (uint32_t i = 0; i < env->table_length; ++i) {
        D2GfxXcptAction* record = table + i;
        if (record->exception_num == exception_num) {
            found = record;
            break;
        }
    }

    if (found == nullptr || found->handler == 0)
        return env->unhandled_exception_filter(exception_ptrs);
    uint32_t handler = found->handler;
    if (handler == 5) {
        found->handler = 0;
        return 1;
    }

    if (handler != 1) {
        uint32_t saved_initarg = ptd->initarg;
        ptd->initarg = exception_ptrs;
        if (found->signal == 8) {
            for (uint32_t i = env->zero_first; i < env->zero_first + env->zero_count; ++i)
                table[i].handler = 0;

            uint32_t saved_signal = ptd->signal_number;
            switch (found->exception_num) {
            case 0xC000008E: ptd->signal_number = 0x83; break;
            case 0xC0000090: ptd->signal_number = 0x81; break;
            case 0xC0000091: ptd->signal_number = 0x84; break;
            case 0xC0000093: ptd->signal_number = 0x85; break;
            case 0xC000008D: ptd->signal_number = 0x82; break;
            case 0xC000008F: ptd->signal_number = 0x86; break;
            case 0xC0000092: ptd->signal_number = 0x8A; break;
            default: break;
            }
            env->call_handler(handler, 8, ptd->signal_number);
            ptd->signal_number = saved_signal;
        } else {
            uint32_t signal = found->signal;
            found->handler = 0;
            env->call_handler(handler, signal, 0);
        }
        ptd->initarg = saved_initarg;
    }
    return -1;
}
