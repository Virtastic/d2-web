// Batch-lane stand-in. Closes the log: when the global log FILE* is non-null, flush via ordinal 10050,
// fclose it, leave then delete the log critical section, null the FILE*. Always returns 1.
// Sole caller 0x70F80. Windows-side pieces (FILE*, critical section, ordinal 10050) live behind the seam.
#include <cstdint>

struct D2Common12AA0Env
{
	uint32_t* pLogFile;		//0x00 global DAT_6fdf1470
	void (*flush)(void);		//0x04 Ordinal_10050
	void (*close)(uint32_t);	//0x08 _fclose(FILE*)
	void (*leave)(void);		//0x0C LeaveCriticalSection(&DAT_6fdf13e0)
	void (*del)(void);		//0x10 DeleteCriticalSection(&DAT_6fdf13e0)
};

// 1.13c D2Common+0x12AA0
extern "C" uint32_t D2Common_12AA0_LogClose(const D2Common12AA0Env* pEnv)
{
	if (*pEnv->pLogFile != 0)
	{
		pEnv->flush();
		pEnv->close(*pEnv->pLogFile);
		pEnv->leave();
		pEnv->del();
		*pEnv->pLogFile = 0;
	}
	return 1;
}
