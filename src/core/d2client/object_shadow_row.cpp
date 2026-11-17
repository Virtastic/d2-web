// Browser export of the Objects.txt row the client's object-shadow gate reads: BlocksLight[mode] (+0x118, 8 bytes) and
// DrawUnder (+0x1B7) of the loaded Objects table. 1.13c D2Client sets the unit's no-shadow bit when BlocksLight[mode]
// is 0 and the shadow draw returns on it; the row itself is the engine's table.
#include <cstdint>
#include <emscripten.h>
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/DataTbls/ObjectsTbls.h"

// -1: no table or no such class. Else DrawUnder << 1 | BlocksLight[mode & 7].
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_object_shadow_row(int classId, int mode) {
    if (!sgptDataTables || !sgptDataTables->pObjectsTxt || classId < 0 || classId >= sgptDataTables->nObjectsTxtRecordCount) return -1;
    const D2ObjectsTxt& r = sgptDataTables->pObjectsTxt[classId];
    return int(r.nDrawUnder) << 1 | int(r.nBlocksLight[mode & 7] ? 1 : 0);
}
