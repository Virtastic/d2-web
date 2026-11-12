#pragma once
#include <cstdint>
namespace d2client_sounds {
bool load();                // 1.13c D2Client+0x5DFD0
uint8_t* records(); int32_t recordCount();    // DAT 0x11C24C / 0x11C250
uint8_t* envTable(); int32_t envCount();   // DAT 0x11C254 / 0x11C258
}
