// 1.13c Storm+0x11630: tear down an SNet connection record. EDI = the record, EBX = the list holder that 0x115B0 drains (EBX survives
// every call, so all thirteen drains see the same holder). Retail runs the drain four times three, then once more, resets the record's
// list head through 0x52D0 (EDI pushed) and frees the record as AUCONNREC (line -2). The drain and the reset come in as seams.
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
extern "C" void Storm13c_11630(uint32_t conn, uint32_t holder, void (*drain)(uint32_t), void (*resetHead)(uint32_t)) {
    for (int32_t i = 4; i != 0; --i) { drain(holder); drain(holder); drain(holder); }
    drain(holder);
    resetHead(conn);
    Storm_Sub1B960((void*)(uintptr_t)conn, "__AUCONNREC__", -2);
}
