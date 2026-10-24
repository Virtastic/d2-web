#include <cstddef>
#include <cstdint>

struct D2CommonXcptAction {
    uint32_t exception_num;
    uint32_t signal;
    uint32_t handler;
};
static_assert(sizeof(D2CommonXcptAction) == 0x0c, "VC7.1 exception action record");

struct D2CommonXcptPtd {
    uint32_t action_table;       // _initaddr +0x54, retail x86 pointer
    uint32_t initarg;            // _initarg +0x58, retail x86 pointer
    uint32_t signal_number;           // _pxcptacttab +0x5C
};
static_assert(offsetof(D2CommonXcptPtd, action_table) == 0x00, "_ptiddata._initaddr");
static_assert(offsetof(D2CommonXcptPtd, initarg) == 0x04, "_ptiddata._initarg");
static_assert(offsetof(D2CommonXcptPtd, signal_number) == 0x08, "_ptiddata._pxcptacttab");
static_assert(sizeof(D2CommonXcptPtd) == 0x0c, "three consecutive retail CRT pointer/word slots");

struct D2CommonXcptFilterEnv {
    D2CommonXcptPtd* (*getptd)();
    D2CommonXcptAction* (*resolve_action_table)(uint32_t retail_address);
    int32_t (*unhandled_exception_filter)(uint32_t exception_ptrs);
    void (*call_handler)(uint32_t handler, uint32_t signal, uint32_t exception_code);
    uint32_t table_length;       // DAT_6fde97a4, count of 12-byte action records
    uint32_t zero_first;         // DAT_6fde9798
    uint32_t zero_count;         // DAT_6fde979c
};

// 1.13c D2Common+0x05B0D (__XcptFilter): find the current thread's CRT exception action by
// exception code. Missing actions and null handlers delegate to UnhandledExceptionFilter; handler
// 5 disarms its record and returns 1; handler 1 returns -1. Other handlers receive their recorded
// signal, with the VC7.1 signal-8 path first clearing a configured action range and translating the
// Windows exception code to the CRT signal number. ptd scratch fields are restored after callbacks.
extern "C" int32_t d2common_5b0d_xcptfilter(uint32_t exception_num, uint32_t exception_ptrs,
                                  const D2CommonXcptFilterEnv* env) {
    D2CommonXcptPtd* ptd = env->getptd();
    D2CommonXcptAction* found = nullptr;
    D2CommonXcptAction* table = env->resolve_action_table(ptd->action_table);
    for (uint32_t i = 0; i < env->table_length; ++i) {
        D2CommonXcptAction* rec = table + i;
        if (rec->exception_num == exception_num) {
            found = rec;
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
        uint32_t saved_arg = ptd->initarg;
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
            uint32_t handler = found->handler;
            found->handler = 0;
            env->call_handler(handler, found->signal, 0);
        }
        ptd->initarg = saved_arg;
    }
    return -1;
}
