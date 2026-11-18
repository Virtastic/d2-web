// D2sound n95 batch: tiny CRT thunks. Seams for the CRT lock calls (the wasm build has no lock table).
void d2s_unlock(int);
extern "C" { __attribute__((weak)) void (*d2sound_n95_unlock8)() = 0; }

// 1.13c D2SOUND+0x016D0
void D2Sound_n95_16D0(void) { if (d2sound_n95_unlock8) d2sound_n95_unlock8(); }

// 1.13c D2SOUND+0x01C27
void D2Sound_n95_1C27(void) { d2s_unlock(4); }

// 1.13c D2SOUND+0x0DFB2
void D2Sound_n95_DFB2(void) {}
