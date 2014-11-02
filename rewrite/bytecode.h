typedef struct
{
	char opcode;
	char name[64];
	char type;
	char free; // free cmd stack after calling?
} OpcodeInfo;

OpcodeInfo opcode[] =
{
	// type 1 (x)
	// type 2 (x aaaa)
	// type 3 (x aaaa bbbb)
	// type 4 (x aaaa bbbb cccc)
	// type 5 (x aaaa b)
	// type 6 (x aaaa bbbb c)
	// type 7 (special needs)
	// type 0 (nop)
	
	{ 0x00, "NOP", 0, 0 },
	{ 0x01, "", 2, 0 }, // label?
	{ 0x02, "Push", 7, 0 },
	{ 0x03, "", 2, 0 },
	{ 0x04, "", 2, 0 },
	{ 0x05, "", 1, 0 },
	{ 0x06, "", 1, 0 },
	{ 0x07, "", 3, 0 },
	{ 0x08, "", 7, 0 },
	{ 0x09, "", 1, 0 },
	{ 0x0a, "NOP", 0, 0 },
	{ 0x0b, "NOP", 0, 0 },
	{ 0x0c, "NOP", 0, 0 },
	{ 0x0d, "NOP", 0, 0 },
	{ 0x0e, "NOP", 0, 0 },
	{ 0x0f, "NOP", 0, 0 },
	
	{ 0x10, "", 2, 0 },
	{ 0x11, "", 2, 0 },
	{ 0x12, "Jump", 2, 0 },
	{ 0x13, "", 4, 0 },
	{ 0x14, "", 4, 0 },
	{ 0x15, "", 7, 0 },
	{ 0x16, "EndScript", 1, 1 },
	{ 0x17, "NOP", 0, 0 },
	{ 0x18, "NOP", 0, 0 },
	{ 0x19, "NOP", 0, 0 },
	{ 0x1a, "NOP", 0, 0 },
	{ 0x1b, "NOP", 0, 0 },
	{ 0x1c, "NOP", 0, 0 },
	{ 0x1d, "NOP", 0, 0 },
	{ 0x1e, "NOP", 0, 0 },
	{ 0x1f, "NOP", 0, 0 },
	
	{ 0x20, "Call2", 4, 1 },
	{ 0x21, "", 5, 0 },
	{ 0x22, "Calc", 6, 1 },
	{ 0x23, "NOP", 0, 0 },
	{ 0x24, "NOP", 0, 0 },
	{ 0x25, "NOP", 0, 0 },
	{ 0x26, "NOP", 0, 0 },
	{ 0x27, "NOP", 0, 0 },
	{ 0x28, "NOP", 0, 0 },
	{ 0x29, "NOP", 0, 0 },
	{ 0x2a, "NOP", 0, 0 },
	{ 0x2b, "NOP", 0, 0 },
	{ 0x2c, "NOP", 0, 0 },
	{ 0x2d, "NOP", 0, 0 },
	{ 0x2e, "NOP", 0, 0 },
	{ 0x2f, "NOP", 0, 0 },
	
	{ 0x30, "Call", 7, 1 },
	{ 0x31, "", 2, 0 },
	{ 0x32, "", 1, 0 }
};