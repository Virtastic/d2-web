// The engine side of the table-teardown free seam (d2common_tblfree.h): Fog's pool free and linker free.
#include "d2common_tblfree.h"
namespace {
void EngineFree(void* p, int32_t nLine) { FOG_FreePool(nullptr, p, "DataTbls.cpp", nLine, 0); }
void EngineFreeLinker(D2TxtLinkStrc* p) { FOG_FreeLinker(p); }
const D2TblFreeSeam kEngine = { EngineFree, EngineFreeLinker };
}
const D2TblFreeSeam* DATATBLS_EngineTblFreeSeam() { return &kEngine; }
