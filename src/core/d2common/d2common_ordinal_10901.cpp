// 1.13c D2Common+0x2CF80 ordinal 10901: real extern "C" entry for importers (D2Client+0xCE32 thunk).
#include "../d2moo/include/D2QuestRecord.h"
extern "C" void D2Common_Ordinal_10901(void* pMemPool, void* pQuestRecord) {
	QUESTRECORD_FreeRecord(pMemPool, (D2BitBufferStrc*)pQuestRecord);
}
