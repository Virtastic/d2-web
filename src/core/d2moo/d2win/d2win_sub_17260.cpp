// 1.13c D2Win+0x17260 -- screenshot writer: pick the first unused "ScreenshotNNN.jpg" name, JPEG-encode a frame.
// Hidden register inputs ESI (frame width) and EBX (frame height) are explicit parameters. The file probe (Fog 10107),
// its close (Fog 10109) and the three ijl11 calls (init #2, write #5, free #3) are env seams; the 0x1390-dword encoder
// state is local, zeroed, with the property words the retail code fills (byte offsets below).
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

struct D2Win17260Env {
    int32_t* counter;                       // DAT_6f8ffba4, the screenshot number
    int32_t (*openFile)(const char* name);  // Fog 10107: returns -1 when the name is free
    void (*closeFile)(int32_t h);           // Fog 10109
    void (*ijlInit)(void* st);              // ijl11 #2
    void (*ijlWrite)(void* st, int32_t opt);// ijl11 #5
    int32_t (*ijlFree)(void* st);           // ijl11 #3
};

// returns ijlFree's result (EAX when the retail function returns)
int32_t d2win_sub_17260(int32_t frame, uint32_t width, uint32_t height, const D2Win17260Env& e) {  // 1.13c D2Win+0x17260
    char name[224];
    for (;;) {
        snprintf(name, sizeof name, "Screenshot%03d.jpg", (int)*e.counter);
        int32_t h = e.openFile(name);
        (*e.counter)++;
        if (h == -1) break;
        e.closeFile(h);
    }
    uint32_t st[0x1390];
    memset(st, 0, sizeof st);
    e.ijlInit(st);
    st[1] = (uint32_t)frame;                                   // +0x04 DIB bytes
    uint32_t w3 = width * 3;
    st[2] = width;                                             // +0x08
    st[3] = height;                                            // +0x0C
    st[4] = ((w3 + 3) & ~3u) - w3;                             // +0x10 row pad to 4 bytes
    st[6] = 1;                                                 // +0x18 colour model
    st[8] = (uint32_t)(uintptr_t)name;                         // +0x20 file name
    st[0x2C / 4] = width;                                      // +0x2C JPEG width
    st[0x30 / 4] = height;                                     // +0x30 JPEG height
    st[0x50 / 4] = 100;                                        // +0x50 quality
    e.ijlWrite(st, 8);
    return e.ijlFree(st);
}
