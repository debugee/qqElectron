#include <windows.h>
#define BX_INSTR_STORE_OPCODE_BYTES
#define BX_STANDALONE_DECODER
#include "bochs.h"
#include "instr.h"

extern int fetchDecode32(const Bit8u *fetchPtr, bool is_32, bxInstruction_c *i, unsigned remainingInPage);
#if BX_SUPPORT_X86_64
extern int fetchDecode64(const Bit8u *fetchPtr, bxInstruction_c *i, unsigned remainingInPage);
#endif

int main()
{
    char disbuf[100] = {0};
    Bit8u instr[] = {0x67, 0x8b, 0x05, 0x12, 0x00, 0x00, 0x88};
    bxInstruction_c i = {0};
    bx_address cs_base = 0;
    bx_address ip = 0;
   // int ret = fetchDecode64(instr, &i, sizeof(instr));

    if (disbuf == disasm(instr, 1, 0, disbuf, &i, cs_base, ip, BX_DISASM_INTEL))
    {
        i.set_opcode_bytes(instr);
    }
	unsigned sib_base = i.sibBase();
    return 0;
}
