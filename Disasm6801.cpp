#include "Torcular.h"
#include "Disasm6801.h"
#include "LabelHandler.h"

OpcodeInfo g_tblOpcode[ 256 ] = {
	{ MNEM_UNKNOWN, 1, 0 }, // 0x00
	{ MNEM_NOP, 1, MODE_INHERENT | FL_VALID }, // 0x01
	{ MNEM_UNKNOWN, 1, 0 }, // 0x02
	{ MNEM_UNKNOWN, 1, 0 }, // 0x03
	{ MNEM_LSRD, 1, MODE_INHERENT | FL_VALID }, // 0x04
	{ MNEM_ASLD, 1, MODE_INHERENT | FL_VALID }, // 0x05
	{ MNEM_UNKNOWN, 1, 0 }, // 0x06
	{ MNEM_UNKNOWN, 1, 0 }, // 0x07
	{ MNEM_INX, 1, MODE_INHERENT | FL_VALID }, // 0x08
	{ MNEM_DEX, 1, MODE_INHERENT | FL_VALID }, // 0x09
	{ MNEM_CLRA, 1, MODE_INHERENT | FL_VALID }, // 0x0A
	{ MNEM_CLRB, 1, MODE_INHERENT | FL_VALID }, // 0x0B
	{ MNEM_CLC, 1, MODE_INHERENT | FL_VALID }, // 0x0C
	{ MNEM_SEC, 1, MODE_INHERENT | FL_VALID }, // 0x0D
	{ MNEM_CLI, 1, MODE_INHERENT | FL_VALID }, // 0x0E
	{ MNEM_SEI, 1, MODE_INHERENT | FL_VALID }, // 0x0F

	{ MNEM_SBA, 1, MODE_INHERENT | FL_VALID }, // 0x10
	{ MNEM_CBA, 1, MODE_INHERENT | FL_VALID }, // 0x11
	{ MNEM_UNKNOWN, 1, 0 }, // 0x12
	{ MNEM_UNKNOWN, 1, 0 }, // 0x13
	{ MNEM_UNKNOWN, 1, 0 }, // 0x14
	{ MNEM_UNKNOWN, 1, 0 }, // 0x15
	{ MNEM_TAB, 1, MODE_INHERENT | FL_VALID }, // 0x16
	{ MNEM_TBA, 1, MODE_INHERENT | FL_VALID }, // 0x17
	{ MNEM_XGDX, 1, MODE_INHERENT | FL_VALID }, // 0x18
	{ MNEM_DAA, 1, MODE_INHERENT | FL_VALID }, // 0x19
	{ MNEM_UNKNOWN, 1, 0 }, // 0x1A
	{ MNEM_ABA, 1, MODE_INHERENT | FL_VALID }, // 0x1B
	{ MNEM_UNKNOWN, 1, 0 }, // 0x1C
	{ MNEM_UNKNOWN, 1, 0 }, // 0x1D
	{ MNEM_UNKNOWN, 1, 0 }, // 0x1E
	{ MNEM_UNKNOWN, 1, 0 }, // 0x1F

	{ MNEM_BRA, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x20
	{ MNEM_BRN, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x21
	{ MNEM_BHI, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x22
	{ MNEM_BLS, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x23
	{ MNEM_BCC, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x24
	{ MNEM_BCS, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x25
	{ MNEM_BNE, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x26
	{ MNEM_BEQ, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x27
	{ MNEM_BVC, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x28
	{ MNEM_BVS, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x29
	{ MNEM_BPL, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x2A
	{ MNEM_BMI, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x2B
	{ MNEM_BGE, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x2C
	{ MNEM_BLT, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x2D
	{ MNEM_BGT, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x2E
	{ MNEM_BLE, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x2F

	{ MNEM_TSX, 1, MODE_INHERENT | FL_VALID }, // 0x30
	{ MNEM_INS, 1, MODE_INHERENT | FL_VALID }, // 0x31
	{ MNEM_PULA, 1, MODE_INHERENT | FL_VALID }, // 0x32
	{ MNEM_PULB, 1, MODE_INHERENT | FL_VALID }, // 0x33
	{ MNEM_DES, 1, MODE_INHERENT | FL_VALID }, // 0x34
	{ MNEM_TXS, 1, MODE_INHERENT | FL_VALID }, // 0x35
	{ MNEM_PSHA, 1, MODE_INHERENT | FL_VALID }, // 0x36
	{ MNEM_PSHB, 1, MODE_INHERENT | FL_VALID }, // 0x37
	{ MNEM_PULX, 1, MODE_INHERENT | FL_VALID }, // 0x38
	{ MNEM_RTS, 1, MODE_INHERENT | FL_VALID }, // 0x39
	{ MNEM_ABX, 1, MODE_INHERENT | FL_VALID }, // 0x3A
	{ MNEM_RTI, 1, MODE_INHERENT | FL_VALID }, // 0x3B
	{ MNEM_PSHX, 1, MODE_INHERENT | FL_VALID }, // 0x3C
	{ MNEM_MUL, 1, MODE_INHERENT | FL_VALID }, // 0x3D
	{ MNEM_WAI, 1, MODE_INHERENT | FL_VALID }, // 0x3E
	{ MNEM_SWI, 1, MODE_INHERENT | FL_VALID }, // 0x3F

	{ MNEM_NEGA, 1, MODE_INHERENT | FL_VALID }, // 0x40
	{ MNEM_UNKNOWN, 1, 0 }, // 0x41
	{ MNEM_UNKNOWN, 1, 0 }, // 0x42
	{ MNEM_COMA, 1, MODE_INHERENT | FL_VALID }, // 0x43
	{ MNEM_LSRA, 1, MODE_INHERENT | FL_VALID }, // 0x44
	{ MNEM_UNKNOWN, 1, 0 }, // 0x45
	{ MNEM_RORA, 1, MODE_INHERENT | FL_VALID }, // 0x46
	{ MNEM_ASRA, 1, MODE_INHERENT | FL_VALID }, // 0x47
	{ MNEM_ASLA, 1, MODE_INHERENT | FL_VALID }, // 0x48
	{ MNEM_ROLA, 1, MODE_INHERENT | FL_VALID }, // 0x49
	{ MNEM_DECA, 1, MODE_INHERENT | FL_VALID }, // 0x4A
	{ MNEM_UNKNOWN, 1, 0 }, // 0x4B
	{ MNEM_INCA, 1, MODE_INHERENT | FL_VALID }, // 0x4C
	{ MNEM_TSTA, 1, MODE_INHERENT | FL_VALID }, // 0x4D
	{ MNEM_UNKNOWN, 1, 0 }, // 0x4E
	{ MNEM_CLRA, 1, MODE_INHERENT | FL_VALID }, // 0x4F

	{ MNEM_NEGB, 1, MODE_INHERENT | FL_VALID }, // 0x50
	{ MNEM_UNKNOWN, 1, 0 }, // 0x51
	{ MNEM_UNKNOWN, 1, 0 }, // 0x52
	{ MNEM_COMB, 1, MODE_INHERENT | FL_VALID }, // 0x53
	{ MNEM_LSRB, 1, MODE_INHERENT | FL_VALID }, // 0x54
	{ MNEM_UNKNOWN, 1, 0 }, // 0x55
	{ MNEM_RORB, 1, MODE_INHERENT | FL_VALID }, // 0x56
	{ MNEM_ASRB, 1, MODE_INHERENT | FL_VALID }, // 0x57
	{ MNEM_ASLB, 1, MODE_INHERENT | FL_VALID }, // 0x58
	{ MNEM_ROLB, 1, MODE_INHERENT | FL_VALID }, // 0x59
	{ MNEM_DECB, 1, MODE_INHERENT | FL_VALID }, // 0x5A
	{ MNEM_UNKNOWN, 1, 0 }, // 0x5B
	{ MNEM_INCB, 1, MODE_INHERENT | FL_VALID }, // 0x5C
	{ MNEM_TSTB, 1, MODE_INHERENT | FL_VALID }, // 0x5D
	{ MNEM_UNKNOWN, 1, 0 }, // 0x5E
	{ MNEM_CLRB, 1, MODE_INHERENT | FL_VALID }, // 0x5F

	{ MNEM_NEG, 2, MODE_INDEXED | FL_VALID }, // 0x60
	{ MNEM_UNKNOWN, 1, 0 }, // 0x61
	{ MNEM_UNKNOWN, 1, 0 }, // 0x62
	{ MNEM_COM, 2, MODE_INDEXED | FL_VALID }, // 0x63
	{ MNEM_LSR, 2, MODE_INDEXED | FL_VALID }, // 0x64
	{ MNEM_UNKNOWN, 1, 0 }, // 0x65
	{ MNEM_ROR, 2, MODE_INDEXED | FL_VALID }, // 0x66
	{ MNEM_ASR, 2, MODE_INDEXED | FL_VALID }, // 0x67
	{ MNEM_ASL, 2, MODE_INDEXED | FL_VALID }, // 0x68
	{ MNEM_ROL, 2, MODE_INDEXED | FL_VALID }, // 0x69
	{ MNEM_DEC, 2, MODE_INDEXED | FL_VALID }, // 0x6A
	{ MNEM_UNKNOWN, 1, 0 }, // 0x6B
	{ MNEM_INC, 2, MODE_INDEXED | FL_VALID }, // 0x6C
	{ MNEM_TST, 2, MODE_INDEXED | FL_VALID }, // 0x6D
	{ MNEM_JMP, 2, MODE_INDEXED | FL_VALID }, // 0x6E
	{ MNEM_CLR, 2, MODE_INDEXED | FL_VALID }, // 0x6F

	{ MNEM_NEG, 3, MODE_EXTENDED | FL_VALID }, // 0x70
	{ MNEM_UNKNOWN, 1, 0 }, // 0x71
	{ MNEM_UNKNOWN, 1, 0 }, // 0x72
	{ MNEM_COM, 3, MODE_EXTENDED | FL_VALID }, // 0x73
	{ MNEM_LSR, 3, MODE_EXTENDED | FL_VALID }, // 0x74
	{ MNEM_UNKNOWN, 1, 0 }, // 0x75
	{ MNEM_ROR, 3, MODE_EXTENDED | FL_VALID }, // 0x76
	{ MNEM_ASR, 3, MODE_EXTENDED | FL_VALID }, // 0x77
	{ MNEM_ASL, 3, MODE_EXTENDED | FL_VALID }, // 0x78
	{ MNEM_ROL, 3, MODE_EXTENDED | FL_VALID }, // 0x79
	{ MNEM_DEC, 3, MODE_EXTENDED | FL_VALID }, // 0x7A
	{ MNEM_UNKNOWN, 1, 0 }, // 0x7B
	{ MNEM_INC, 3, MODE_EXTENDED | FL_VALID }, // 0x7C
	{ MNEM_TST, 3, MODE_EXTENDED | FL_VALID }, // 0x7D
	{ MNEM_JMP, 3, MODE_EXTENDED | FL_VALID | FL_JUMP }, // 0x7E
	{ MNEM_CLR, 3, MODE_EXTENDED | FL_VALID }, // 0x7F

	{ MNEM_SUBA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x80
	{ MNEM_CMPA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x81
	{ MNEM_SBCA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x82
	{ MNEM_SUBD, 3, MODE_IMMEDIATE | FL_VALID }, // 0x83
	{ MNEM_ANDA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x84
	{ MNEM_BITA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x85
	{ MNEM_LDAA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x86
	{ MNEM_UNKNOWN, 1, 0 }, // 0x87
	{ MNEM_EORA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x88
	{ MNEM_ADCA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x89
	{ MNEM_ORAA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x8A
	{ MNEM_ADDA, 2, MODE_IMMEDIATE | FL_VALID }, // 0x8B
	{ MNEM_CPX, 3, MODE_IMMEDIATE | FL_VALID }, // 0x8C
	{ MNEM_BSR, 2, MODE_RELATIVE | FL_VALID | FL_JUMP }, // 0x8D
	{ MNEM_LDS, 3, MODE_IMMEDIATE | FL_VALID }, // 0x8E
	{ MNEM_UNKNOWN, 1, 0 }, // 0x8F

	{ MNEM_SUBA, 2, MODE_DIRECT | FL_VALID }, // 0x90
	{ MNEM_CMPA, 2, MODE_DIRECT | FL_VALID }, // 0x91
	{ MNEM_SBCA, 2, MODE_DIRECT | FL_VALID }, // 0x92
	{ MNEM_SUBD, 2, MODE_DIRECT | FL_VALID }, // 0x93
	{ MNEM_ANDA, 2, MODE_DIRECT | FL_VALID }, // 0x94
	{ MNEM_BITA, 2, MODE_DIRECT | FL_VALID }, // 0x95
	{ MNEM_LDAA, 2, MODE_DIRECT | FL_VALID }, // 0x96
	{ MNEM_STAA, 2, MODE_DIRECT | FL_VALID }, // 0x97
	{ MNEM_EORA, 2, MODE_DIRECT | FL_VALID }, // 0x98
	{ MNEM_ADCA, 2, MODE_DIRECT | FL_VALID }, // 0x99
	{ MNEM_ORAA, 2, MODE_DIRECT | FL_VALID }, // 0x9A
	{ MNEM_ADDA, 2, MODE_DIRECT | FL_VALID }, // 0x9B
	{ MNEM_CPX, 2, MODE_DIRECT | FL_VALID }, // 0x9C
	{ MNEM_JSR, 2, MODE_DIRECT | FL_VALID | FL_JUMP }, // 0x9D
	{ MNEM_LDS, 2, MODE_DIRECT | FL_VALID }, // 0x9E
	{ MNEM_STS, 2, MODE_DIRECT | FL_VALID }, // 0x9F

	{ MNEM_SUBA, 2, MODE_INDEXED | FL_VALID }, // 0xA0
	{ MNEM_CMPA, 2, MODE_INDEXED | FL_VALID }, // 0xA1
	{ MNEM_SBCA, 2, MODE_INDEXED | FL_VALID }, // 0xA2
	{ MNEM_SUBD, 2, MODE_INDEXED | FL_VALID }, // 0xA3
	{ MNEM_ANDA, 2, MODE_INDEXED | FL_VALID }, // 0xA4
	{ MNEM_BITA, 2, MODE_INDEXED | FL_VALID }, // 0xA5
	{ MNEM_LDAA, 2, MODE_INDEXED | FL_VALID }, // 0xA6
	{ MNEM_STAA, 2, MODE_INDEXED | FL_VALID }, // 0xA7
	{ MNEM_EORA, 2, MODE_INDEXED | FL_VALID }, // 0xA8
	{ MNEM_ADCA, 2, MODE_INDEXED | FL_VALID }, // 0xA9
	{ MNEM_ORAA, 2, MODE_INDEXED | FL_VALID }, // 0xAA
	{ MNEM_ADDA, 2, MODE_INDEXED | FL_VALID }, // 0xAB
	{ MNEM_CPX, 2, MODE_INDEXED | FL_VALID }, // 0xAC
	{ MNEM_JSR, 2, MODE_INDEXED | FL_VALID }, // 0xAD
	{ MNEM_LDS, 2, MODE_INDEXED | FL_VALID }, // 0xAE
	{ MNEM_STS, 2, MODE_INDEXED | FL_VALID }, // 0xAF

	{ MNEM_SUBA, 3, MODE_EXTENDED | FL_VALID }, // 0xB0
	{ MNEM_CMPA, 3, MODE_EXTENDED | FL_VALID }, // 0xB1
	{ MNEM_SBCA, 3, MODE_EXTENDED | FL_VALID }, // 0xB2
	{ MNEM_SUBD, 3, MODE_EXTENDED | FL_VALID }, // 0xB3
	{ MNEM_ANDA, 3, MODE_EXTENDED | FL_VALID }, // 0xB4
	{ MNEM_BITA, 3, MODE_EXTENDED | FL_VALID }, // 0xB5
	{ MNEM_LDAA, 3, MODE_EXTENDED | FL_VALID }, // 0xB6
	{ MNEM_STAA, 3, MODE_EXTENDED | FL_VALID }, // 0xB7
	{ MNEM_EORA, 3, MODE_EXTENDED | FL_VALID }, // 0xB8
	{ MNEM_ADCA, 3, MODE_EXTENDED | FL_VALID }, // 0xB9
	{ MNEM_ORAA, 3, MODE_EXTENDED | FL_VALID }, // 0xBA
	{ MNEM_ADDA, 3, MODE_EXTENDED | FL_VALID }, // 0xBB
	{ MNEM_CPX, 3, MODE_EXTENDED | FL_VALID }, // 0xBC
	{ MNEM_JSR, 3, MODE_EXTENDED | FL_VALID | FL_JUMP }, // 0xBD
	{ MNEM_LDS, 3, MODE_EXTENDED | FL_VALID }, // 0xBE
	{ MNEM_STS, 3, MODE_EXTENDED | FL_VALID }, // 0xBF

	{ MNEM_SUBB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC0
	{ MNEM_CMPB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC1
	{ MNEM_SBCB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC2
	{ MNEM_ADDD, 3, MODE_IMMEDIATE | FL_VALID }, // 0xC3
	{ MNEM_ANDB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC4
	{ MNEM_BITB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC5
	{ MNEM_LDAB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC6
	{ MNEM_UNKNOWN, 1, 0 }, // 0xC7
	{ MNEM_EORB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC8
	{ MNEM_ADCB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xC9
	{ MNEM_ORAB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xCA
	{ MNEM_ADDB, 2, MODE_IMMEDIATE | FL_VALID }, // 0xCB
	{ MNEM_LDD, 3, MODE_IMMEDIATE | FL_VALID }, // 0xCC
	{ MNEM_UNKNOWN, 1, 0 }, // 0xCD
	{ MNEM_LDX, 3, MODE_IMMEDIATE | FL_VALID }, // 0xCE
	{ MNEM_UNKNOWN, 1, 0 }, // 0xCF

	{ MNEM_SUBB, 2, MODE_DIRECT | FL_VALID }, // 0xD0
	{ MNEM_CMPB, 2, MODE_DIRECT | FL_VALID }, // 0xD1
	{ MNEM_SBCB, 2, MODE_DIRECT | FL_VALID }, // 0xD2
	{ MNEM_ADDD, 2, MODE_DIRECT | FL_VALID }, // 0xD3
	{ MNEM_ANDB, 2, MODE_DIRECT | FL_VALID }, // 0xD4
	{ MNEM_BITB, 2, MODE_DIRECT | FL_VALID }, // 0xD5
	{ MNEM_LDAB, 2, MODE_DIRECT | FL_VALID }, // 0xD6
	{ MNEM_STAB, 2, MODE_DIRECT | FL_VALID }, // 0xD7
	{ MNEM_EORB, 2, MODE_DIRECT | FL_VALID }, // 0xD8
	{ MNEM_ADCB, 2, MODE_DIRECT | FL_VALID }, // 0xD9
	{ MNEM_ORAB, 2, MODE_DIRECT | FL_VALID }, // 0xDA
	{ MNEM_ADDB, 2, MODE_DIRECT | FL_VALID }, // 0xDB
	{ MNEM_LDD, 2, MODE_DIRECT | FL_VALID }, // 0xDC
	{ MNEM_STD, 2, MODE_DIRECT | FL_VALID }, // 0xDD
	{ MNEM_LDX, 2, MODE_DIRECT | FL_VALID }, // 0xDE
	{ MNEM_STX, 2, MODE_DIRECT | FL_VALID }, // 0xDF

	{ MNEM_SUBB, 2, MODE_INDEXED | FL_VALID }, // 0xE0
	{ MNEM_CMPB, 2, MODE_INDEXED | FL_VALID }, // 0xE1
	{ MNEM_SBCB, 2, MODE_INDEXED | FL_VALID }, // 0xE2
	{ MNEM_ADDD, 2, MODE_INDEXED | FL_VALID }, // 0xE3
	{ MNEM_ANDB, 2, MODE_INDEXED | FL_VALID }, // 0xE4
	{ MNEM_BITB, 2, MODE_INDEXED | FL_VALID }, // 0xE5
	{ MNEM_LDAB, 2, MODE_INDEXED | FL_VALID }, // 0xE6
	{ MNEM_STAB, 2, MODE_INDEXED | FL_VALID }, // 0xE7
	{ MNEM_EORB, 2, MODE_INDEXED | FL_VALID }, // 0xE8
	{ MNEM_ADCB, 2, MODE_INDEXED | FL_VALID }, // 0xE9
	{ MNEM_ORAB, 2, MODE_INDEXED | FL_VALID }, // 0xEA
	{ MNEM_ADDB, 2, MODE_INDEXED | FL_VALID }, // 0xEB
	{ MNEM_LDD, 2, MODE_INDEXED | FL_VALID }, // 0xEC
	{ MNEM_STD, 2, MODE_INDEXED | FL_VALID }, // 0xED
	{ MNEM_LDX, 2, MODE_INDEXED | FL_VALID }, // 0xEE
	{ MNEM_STX, 2, MODE_INDEXED | FL_VALID }, // 0xEF

	{ MNEM_SUBB, 3, MODE_EXTENDED | FL_VALID }, // 0xF0
	{ MNEM_CMPB, 3, MODE_EXTENDED | FL_VALID }, // 0xF1
	{ MNEM_SBCB, 3, MODE_EXTENDED | FL_VALID }, // 0xF2
	{ MNEM_ADDD, 3, MODE_EXTENDED | FL_VALID }, // 0xF3
	{ MNEM_ANDB, 3, MODE_EXTENDED | FL_VALID }, // 0xF4
	{ MNEM_BITB, 3, MODE_EXTENDED | FL_VALID }, // 0xF5
	{ MNEM_LDAB, 3, MODE_EXTENDED | FL_VALID }, // 0xF6
	{ MNEM_STAB, 3, MODE_EXTENDED | FL_VALID }, // 0xF7
	{ MNEM_EORB, 3, MODE_EXTENDED | FL_VALID }, // 0xF8
	{ MNEM_ADCB, 3, MODE_EXTENDED | FL_VALID }, // 0xF9
	{ MNEM_ORAB, 3, MODE_EXTENDED | FL_VALID }, // 0xFA
	{ MNEM_ADDB, 3, MODE_EXTENDED | FL_VALID }, // 0xFB
	{ MNEM_LDD, 3, MODE_EXTENDED | FL_VALID }, // 0xFC
	{ MNEM_STD, 3, MODE_EXTENDED | FL_VALID }, // 0xFD
	{ MNEM_LDX, 3, MODE_EXTENDED | FL_VALID }, // 0xFE
	{ MNEM_STX, 3, MODE_EXTENDED | FL_VALID }, // 0xFF
};

const char* get_mnemonic_string( MnemonicID id )
{
	switch ( id ) {
	case MNEM_NOP:
		return "NOP";
	case MNEM_LSRD:
		return "LSRD";
	case MNEM_ASLD:
		return "ASLD";
	case MNEM_INX:
		return "INX";
	case MNEM_DEX:
		return "DEX";
	case MNEM_CLRA:
		return "CLRA";
	case MNEM_CLRB:
		return "CLRB";
	case MNEM_CLC:
		return "CLC";
	case MNEM_SEC:
		return "SEC";
	case MNEM_CLI:
		return "CLI";
	case MNEM_SEI:
		return "SEI";
	case MNEM_SBA:
		return "SBA";
	case MNEM_CBA:
		return "CBA";
	case MNEM_TAB:
		return "TAB";
	case MNEM_TBA:
		return "TBA";
	case MNEM_XGDX:
		return "XGDX";
	case MNEM_DAA:
		return "DAA";
	case MNEM_ABA:
		return "ABA";
	case MNEM_BRA:
		return "BRA";
	case MNEM_BRN:
		return "BRN";
	case MNEM_BHI:
		return "BHI";
	case MNEM_BLS:
		return "BLS";
	case MNEM_BCC:
		return "BCC";
	case MNEM_BCS:
		return "BCS";
	case MNEM_BNE:
		return "BNE";
	case MNEM_BEQ:
		return "BEQ";
	case MNEM_BVC:
		return "BVC";
	case MNEM_BVS:
		return "BVS";
	case MNEM_BPL:
		return "BPL";
	case MNEM_BMI:
		return "BMI";
	case MNEM_BGE:
		return "BGE";
	case MNEM_BLT:
		return "BLT";
	case MNEM_BGT:
		return "BGT";
	case MNEM_BLE:
		return "BLE";
	case MNEM_TSX:
		return "TSX";
	case MNEM_INS:
		return "INS";
	case MNEM_PULA:
		return "PULA";
	case MNEM_PULB:
		return "PULB";
	case MNEM_DES:
		return "DES";
	case MNEM_TXS:
		return "TXS";
	case MNEM_PSHA:
		return "PSHA";
	case MNEM_PSHB:
		return "PSHB";
	case MNEM_PULX:
		return "PULX";
	case MNEM_RTS:
		return "RTS";
	case MNEM_ABX:
		return "ABX";
	case MNEM_RTI:
		return "RTI";
	case MNEM_PSHX:
		return "PSHX";
	case MNEM_MUL:
		return "MUL";
	case MNEM_WAI:
		return "WAI";
	case MNEM_SWI:
		return "SWI";
	case MNEM_NEGA:
		return "NEGA";
	case MNEM_COMA:
		return "COMA";
	case MNEM_LSRA:
		return "LSRA";
	case MNEM_RORA:
		return "RORA";
	case MNEM_ASRA:
		return "ASRA";
	case MNEM_ASLA:
		return "ASLA";
	case MNEM_ROLA:
		return "ROLA";
	case MNEM_DECA:
		return "DECA";
	case MNEM_INCA:
		return "INCA";
	case MNEM_TSTA:
		return "TSTA";
	case MNEM_NEGB:
		return "NEGB";
	case MNEM_COMB:
		return "COMB";
	case MNEM_LSRB:
		return "LSRB";
	case MNEM_RORB:
		return "RORB";
	case MNEM_ASRB:
		return "ASRB";
	case MNEM_ASLB:
		return "ASLB";
	case MNEM_ROLB:
		return "ROLB";
	case MNEM_DECB:
		return "DECB";
	case MNEM_INCB:
		return "INCB";
	case MNEM_TSTB:
		return "TSTB";
	case MNEM_NEG:
		return "NEG";
	case MNEM_COM:
		return "COM";
	case MNEM_LSR:
		return "LSR";
	case MNEM_ROR:
		return "ROR";
	case MNEM_ASR:
		return "ASR";
	case MNEM_ASL:
		return "ASL";
	case MNEM_ROL:
		return "ROL";
	case MNEM_DEC:
		return "DEC";
	case MNEM_INC:
		return "INC";
	case MNEM_TST:
		return "TST";
	case MNEM_JMP:
		return "JMP";
	case MNEM_CLR:
		return "CLR";
	case MNEM_SUBA:
		return "SUBA";
	case MNEM_CMPA:
		return "CMPA";
	case MNEM_SBCA:
		return "SBCA";
	case MNEM_SUBD:
		return "SUBD";
	case MNEM_ANDA:
		return "ANDA";
	case MNEM_BITA:
		return "BITA";
	case MNEM_LDAA:
		return "LDAA";
	case MNEM_STAA:
		return "STAA";
	case MNEM_EORA:
		return "EORA";
	case MNEM_ADCA:
		return "ADCA";
	case MNEM_ORAA:
		return "ORAA";
	case MNEM_ADDA:
		return "ADDA";
	case MNEM_CPX:
		return "CPX";
	case MNEM_BSR:
		return "BSR";
	case MNEM_LDS:
		return "LDS";
	case MNEM_JSR:
		return "JSR";
	case MNEM_STS:
		return "STS";
	case MNEM_SUBB:
		return "SUBB";
	case MNEM_CMPB:
		return "CMPB";
	case MNEM_SBCB:
		return "SBCB";
	case MNEM_ADDD:
		return "ADDD";
	case MNEM_ANDB:
		return "ANDB";
	case MNEM_BITB:
		return "BITB";
	case MNEM_LDAB:
		return "LDAB";
	case MNEM_STAB:
		return "STAB";
	case MNEM_EORB:
		return "EORB";
	case MNEM_ADCB:
		return "ADCB";
	case MNEM_ORAB:
		return "ORAB";
	case MNEM_ADDB:
		return "ADDB";
	case MNEM_LDD:
		return "LDD";
	case MNEM_STD:
		return "STD";
	case MNEM_LDX:
		return "LDX";
	case MNEM_STX:
		return "STX";
	case MNEM_UNKNOWN:
	default:
		return "???";
	}
}

BOOL CDisasm6801::Set6801Vector( VOID )
{	// set 6801 vectors
BOOL bResult = FALSE;
PBYTE pbyData = NULL;
TCHAR tsz[ MAX_PATH ];

	if ( !m_pLabelHandler )
		return bResult;
	if ( m_hBin )
		pbyData = (PBYTE)GlobalLock( m_hBin );
	if ( pbyData ) {
		bResult = TRUE;
		_tcscpy( tsz, _T( "RESET_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFFE, tsz );
		_tcscpy( tsz, _T( "NMI_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFFC, tsz );
		_tcscpy( tsz, _T( "SWI_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFFA, tsz );
		_tcscpy( tsz, _T( "IRQ1_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFF8, tsz );
		_tcscpy( tsz, _T( "ICF_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFF6, tsz );  // Input Capture
		_tcscpy( tsz, _T( "OCF_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFF4, tsz );  // Output Compare
		_tcscpy( tsz, _T( "TOF_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFF2, tsz );  // Timer Overflow
		_tcscpy( tsz, _T( "SCI_HANDLER" ) );
		m_pLabelHandler->RegisterVector( pbyData, m_dwStartAddress, 0xFFF0, tsz );  // Serial I/O
		GlobalUnlock( m_hBin );
	}
	return bResult;
}

BOOL CDisasm6801::DoDisasm( VOID )
{
BOOL bResult = FALSE;

	if ( m_pLabelHandler )
		bResult = TRUE;
	if ( bResult )
		bResult = ReadBinFile();
	if ( bResult ) {
		m_pLabelHandler->Init();
		bResult = Set6801Vector();
	}
	if ( bResult ) {
		DoPass1();
	}
	//CreateAsmFile();
	if ( bResult ) {
		DoPass2();
	}
	//CloseFiles();
	m_pLabelHandler->PrintCrossReferenceTable();

	return bResult;
}

BOOL CDisasm6801::SetBinFile( PTSTR ptszBinFile )
{
BOOL bResult = FALSE;
DWORD dwAttr;

	if ( ptszBinFile ) {
		_tcscpy( m_tszBinPath, ptszBinFile );
		dwAttr = GetFileAttributes( m_tszBinPath );
		if ( dwAttr != INVALID_FILE_ATTRIBUTES ) {
			if ( !( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) ) {
				bResult = TRUE;
			}
		}
	}
	return bResult;
}

BOOL CDisasm6801::ReadBinFile( VOID )
{
BOOL bResult = FALSE;
DWORD dwSizeLo, dwSizeHi, dwRead;
TCHAR tsz[ MAX_PATH ];
PVOID pGlobal = nullptr;
HANDLE hFile;
HGLOBAL hGlobal = nullptr;

	if ( _tcslen( m_tszBinPath ) ) {
		hFile = CreateFile( m_tszBinPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE  ) {
			DispError();
		} else {
			dwSizeLo = GetFileSize( hFile, &dwSizeHi );
			if ( !dwSizeHi ) {
				hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwSizeLo );
				if ( hGlobal )
					pGlobal = GlobalLock( hGlobal );
				ReadFile( hFile, pGlobal, dwSizeLo, &dwRead, NULL );
				if ( dwRead == dwSizeLo ) {
					m_hBin = hGlobal;
					m_dwSizeBin = dwRead;
					bResult = TRUE;
				} else {
					DispError();
				}
				GlobalUnlock( hGlobal );
			} else {
				wsprintf( tsz, _T( "Bin file too large" ) );
				AddMessage( tsz );
			}
			CloseHandle( hFile );
		}
	}
	return bResult;
}

BOOL CDisasm6801::ReadLabelFile( VOID )
{
BOOL bResult = FALSE;

	return bResult;
}

VOID EntryLabel( DWORD dwLabel, DWORD dwUse )
{
TCHAR tsz[ MAX_PATH ];

	wsprintf( tsz, _T( "L%04X : %04X\r\n" ), dwLabel, dwUse );
	AddMessage( tsz );
}

BOOL CDisasm6801::DoPass1( VOID )
{
BOOL bResult = FALSE;
CHAR scOfs;
BYTE byMode;
BYTE byOpcode;
PBYTE pbyData;
DWORD dwLength, dwAddr;

	if ( !m_hBin )
		return bResult;
	pbyData = (PBYTE)GlobalLock( m_hBin );
	if ( pbyData ) {
		m_dwPC = m_dwStartAddress;
		m_dwAdr = 0;
		while ( m_dwAdr < m_dwSizeBin ) {
			byOpcode = pbyData[ m_dwAdr ];
			dwLength = g_tblOpcode[ byOpcode ].byLength;
			if ( !isValid( &g_tblOpcode[ byOpcode ] ) ) {
				m_dwAdr += dwLength;
				continue;
			}
			byMode = GetMode( &g_tblOpcode[ byOpcode ] );
			if ( isJump( &g_tblOpcode[ byOpcode ] ) ) {
				//byMode = GetMode( &g_tblOpcode[ byOpcode ] );
				if ( byMode == MODE_RELATIVE ) {
					scOfs = /*( signed char )*/( CHAR )pbyData[ m_dwAdr + 1 ];
					dwAddr = ( DWORD )( m_dwStartAddress + m_dwAdr + dwLength + scOfs );
					m_pLabelHandler->RegisterLabel( dwAddr, m_dwAdr + m_dwStartAddress );
				} else if ( byMode == MODE_EXTENDED ) {
					dwAddr = ( DWORD )( ( (DWORD)pbyData[ m_dwAdr + 1 ] << 8 ) | pbyData[ m_dwAdr + 2 ] );
					m_pLabelHandler->RegisterLabel( dwAddr, m_dwAdr + m_dwStartAddress );
				} else if ( byMode == MODE_DIRECT ) {
					dwAddr = ( DWORD )pbyData[ m_dwAdr + 1 ];
					m_pLabelHandler->RegisterLabel( dwAddr, m_dwAdr + m_dwStartAddress );
				} else if ( byMode == MODE_INDEXED ) {
					// JMP 0,X ... cannot calc address
				}
			}
			m_dwAdr += dwLength;//g_tblOpcode[ byOpcode ].byLength;
		}
		GlobalUnlock( m_hBin );
	}

	return bResult;
}

BOOL CDisasm6801::CreateAsmFile( VOID )
{
BOOL bResult = FALSE;

	return bResult;
}

BOOL CDisasm6801::DoPass2( VOID )
{
BOOL bResult = FALSE;
BYTE byOpcode;
BOOL bDisp;
PBYTE pbyData;
DWORD dwLength;
TCHAR tsz[ MAX_PATH ];

	if ( !m_hBin )
		return bResult;
	pbyData = (PBYTE)GlobalLock( m_hBin );
	if ( pbyData ) {
		m_dwPC = m_dwStartAddress;
		m_dwAdr = 0;
		while ( m_dwAdr < m_dwSizeBin ) {
			byOpcode = pbyData[ m_dwAdr ];
			dwLength = g_tblOpcode[ byOpcode ].byLength;
			bDisp = m_pLabelHandler->PrintLabelIfExists( m_dwAdr + m_dwStartAddress );
			if ( bDisp )
				wsprintf( tsz, _T( "\t%s\r\n" ), get_mnemonic_string( (MnemonicID)g_tblOpcode[ byOpcode ].byMnemonicId ) );
			else
				wsprintf( tsz, _T( "\t\t%s\r\n" ), get_mnemonic_string( (MnemonicID)g_tblOpcode[ byOpcode ].byMnemonicId ) );
			AddMessage( tsz );
			m_dwAdr += dwLength;
		}
		GlobalUnlock( m_hBin );
	}

	return bResult;
}

VOID CDisasm6801::CloseFiles( VOID )
{
}

VOID CDisasm6801::Init( VOID )
{
	ZeroMemory( m_tszBinPath, sizeof( m_tszBinPath ) );
	m_pLabelHandler = new CLabelHandler;
	m_hBin = nullptr;
	m_pbyBin = nullptr;
	m_dwStartAddress = 0xf000;
}

CDisasm6801::CDisasm6801()
{
	Init();
}

CDisasm6801::~CDisasm6801()
{
	CloseFiles();
}

