// Types shared by the D2Client 0x933E0..0x97770 inventory/cursor UI ports and their FN probes.
#pragma once
#include <cstdint>
namespace d2client_batch_933e0 {
struct Unit { int32_t type, classId, id_unused, id; };  // GUESS: only +0,+4,+0xC,+0x60 used below
struct Area { uint8_t w, h; uint8_t pad[2]; int32_t left; int32_t r8; int32_t top; uint8_t pad2[4]; uint8_t cellW, cellH; };
static_assert(sizeof(Area) == 0x18, "area");
struct Globals {   // D2Client globals by RVA (base 6fb40000)
 int32_t g_BCC28{}; void* hover{}; void* hoverPrev{};          // 0xBCC28 0xBCC38 0xBCC3C
 int32_t hx0{},hy0{},hx1{},hy1{},hcellX{},hcellY{};            // 0x90EA8 EAC EB0 EB4 EB8 EBC
 int32_t g_90EA4{}; int32_t cursorMode{}; int32_t g_BCB834{};  // 0x90EA4 0xBCB864 0xBCB834 (held item ptr token)
 int32_t g_BCB854{}, g_BCB858{}; uint8_t g_BCC34{}; int32_t g_BCC30{};
 uint8_t panelId{}; int32_t mouseX{}, mouseY{}; int32_t g_BCC88{};
 int32_t g_BCC58{}, g_BCC9C{}, g_BCC98{}, g_90EC0{};
};
struct Env {   // imported calls
 virtual ~Env() = default;
 [[noreturn]] virtual void fatal(int32_t msgId) = 0;          // Ordinal_10265+10024+_exit(-1)
 virtual uint32_t ownerKind(void*) = 0;                       // Ordinal_10584
 virtual Unit* itemAtCell(void* inv, uint32_t cx, uint32_t cy, int32_t* a, int32_t* b) = 0; // Ordinal_10801
 virtual void  itemSize(Unit*, uint8_t* w, uint8_t* h) = 0;   // Ordinal_10114
 virtual int32_t gridX(Unit*) = 0; virtual int32_t gridY(Unit*) = 0; // FUN 0x1630 / 0x1660
 virtual int32_t heldCount(void*) = 0;                        // Ordinal_10202 family
 virtual Unit* invItem(void* inv, int32_t slot) = 0;          // Ordinal_11139
 virtual Unit* invFirst(void* inv) = 0; virtual Unit* invNext(Unit*) = 0; // 10460 / 10464
 virtual void* itemRecord(Unit*) = 0;                         // Ordinal_11147
 virtual uint8_t footprint(Unit*, int32_t slot, int32_t* dx, int32_t* dy) = 0; // GUESS table 0xB0A98
 virtual void  drawMark(int32_t x, int32_t y) = 0;            // FUN 0x6B5A0 (1B5A0)
 virtual void  drawFootprint(Unit*, int32_t x, int32_t y) = 0;
 virtual bool  isUnidentified(Unit*) = 0; virtual bool isIdentified(Unit*) = 0; virtual bool flag800(Unit*) = 0;
 virtual int32_t unitKind(Unit*) = 0;
 virtual bool  worldDispatch(int32_t, int32_t, int32_t) = 0;  // FUN 0x47D60
 virtual void  sendPacket(const uint8_t*, unsigned) = 0;      // FUN 0x43E0 (AC43E0)
 virtual void  stateUpdate(int32_t, int32_t, int32_t) = 0;    // FUN 0xC2790
 virtual void  unitFind(int32_t kind, int32_t id, Unit** out) = 0;
 virtual void  notify(int32_t, int32_t) = 0;
 virtual bool  busy() = 0; virtual bool canSend() = 0;        // FUN 0x31CB0 / 0x423F0
 virtual void  clearCursor() = 0;                             // FUN 0x31CE0
 virtual void  swapSend(int32_t a, int32_t b) = 0;            // FUN 0xAC4910
 virtual void  dropSend() = 0;                                // FUN 0xAC4940
 virtual void  refresh() = 0;                                 // FUN 0x31E40 / 0x38A70
};
}
