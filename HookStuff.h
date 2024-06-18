#ifndef __HOOKSTUFF_H__
#define __HOOKSTUFF_H__
#include <Windows.h>

typedef struct _HOOKINFO
{
	PVOID OldAddress;
	PVOID StubAddress;
	PVOID StubNotApiAddress;
	BOOL bAllocStub;
	BYTE  SavedCode[32];
	BYTE SavedOpCode[32];
}HOOK_INFO, *PHOOK_INFO;

PVOID GetNewAddress(PHOOK_INFO Info);
size_t GetFunctionLen(BYTE* Address, size_t nLenNeed);
BOOL HookByAddress(PVOID HookFunc, PHOOK_INFO Info, PVOID NewFunction, BOOL longJmp/*x64 only*/ = FALSE, BOOL bStub = FALSE, BOOL bFuncMiddle = FALSE, unsigned char opcodes[20] = nullptr);
BOOL HookAPIByName(char *ModuleName, char *FuncName, PHOOK_INFO Info, PVOID NewFunction);
VOID UnHookAPI(PHOOK_INFO Info);
VOID UnHook(PHOOK_INFO Info);

#endif
