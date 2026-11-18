// 1.13c Storm+0x27020: SFile teardown (cdecl, no arguments, returns 1). Stops the stream thread when it is live, destroys
// the object at 0x53108, drains the four object lists and frees the three SFILE.CPP buffers.
#include "storm_27020.h"

extern "C" uint32_t Storm13c_27020(Storm27020Mod* m, const Storm27020Seam* s) {
    static const char kDelete[] = "delete";
    static const char kSfile[] = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";
    const uint32_t kNoLine = 0xFFFFFFFFu;

    s->sub26F30();
    if (m->thread) {
        m->stopFlag = 1;
        s->setEvent(m->event);
        s->waitThread(m->thread, 1000);
        if (m->event) { s->closeHandle(m->event); m->event = 0; }
        if (m->thread) { s->closeHandle(m->thread); m->thread = 0; }
    }
    if (m->obj) {
        s->dtor4040(m->obj);
        s->smemFree(m->obj, kDelete, kNoLine, 0);
        m->obj = 0;
    }
    // each list: while the head reads as a positive dword, destroy it (the destructor unlinks it in the engine)
    while ((int32_t)m->headDtor4040 > 0) {
        uint32_t o = m->headDtor4040;
        s->dtor4040(o);
        s->smemFree(o, kDelete, kNoLine, 0);
    }
    while ((int32_t)m->headAudio > 0) {
        uint32_t o = m->headAudio;
        s->unlog("AUDIOSTREAM", 0);
        s->dtor3FC0(o);
        s->smemFree(o, kDelete, kNoLine, 0);
    }
    while ((int32_t)m->headHsFile > 0) {
        uint32_t o = m->headHsFile;
        s->unlog("HSFILE", o + 8);
        s->dtor3EF0(o);
        s->smemFree(o, kDelete, kNoLine, 0);
    }
    while ((int32_t)m->headHsArchive > 0) {
        uint32_t o = m->headHsArchive;
        s->unlog("HSARCHIVE", o + 8);
        s->dtor3E20(o);
        s->smemFree(o, kDelete, kNoLine, 0);
    }
    if (m->heap0) { s->smemFree(m->heap0, kSfile, 0xE94, 0); m->heap0 = 0; }
    if (m->heap1) { s->smemFree(m->heap1, kSfile, 0xE9A, 0); m->heap1 = 0; }
    if (m->heap2) { s->smemFree(m->heap2, kSfile, 0xEA0, 0); m->heap2 = 0; }
    return 1;
}
