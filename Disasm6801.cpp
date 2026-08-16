#include "Torcular.h"
#include "Disasm6801.h"
#include "LabelHandler.h"

OpcodeInfo g_tblOpcode[ 256 ] = {
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x00
    { MNEM_NOP, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x01
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x02
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x03
    { MNEM_LSRD, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x04
    { MNEM_ASLD, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x05
    { MNEM_TAP, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x06
    { MNEM_TPA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x07
    { MNEM_INX, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x08
    { MNEM_DEX, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x09
    { MNEM_CLV, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x0A
    { MNEM_SEV, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x0B
    { MNEM_CLC, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x0C
    { MNEM_SEC, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x0D
    { MNEM_CLI, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x0E
    { MNEM_SEI, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x0F
    { MNEM_SBA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x10
    { MNEM_CBA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x11
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x12
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x13
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x14
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x15
    { MNEM_TAB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x16
    { MNEM_TBA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x17
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x18
    { MNEM_DAA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x19
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x1A
    { MNEM_ABA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x1B
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x1C
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x1D
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x1E
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x1F
    { MNEM_BRA, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x20
    { MNEM_BRN, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x21
    { MNEM_BHI, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x22
    { MNEM_BLS, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x23
    { MNEM_BCC, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x24
    { MNEM_BCS, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x25
    { MNEM_BNE, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x26
    { MNEM_BEQ, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x27
    { MNEM_BVC, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x28
    { MNEM_BVS, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x29
    { MNEM_BPL, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x2A
    { MNEM_BMI, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x2B
    { MNEM_BGE, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x2C
    { MNEM_BLT, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x2D
    { MNEM_BGT, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x2E
    { MNEM_BLE, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x2F
    { MNEM_TSX, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x30
    { MNEM_INS, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x31
    { MNEM_PULA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x32
    { MNEM_PULB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x33
    { MNEM_DES, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x34
    { MNEM_TXS, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x35
    { MNEM_PSHA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x36
    { MNEM_PSHB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x37
    { MNEM_PULX, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x38
    { MNEM_RTS, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x39
    { MNEM_ABX, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x3A
    { MNEM_RTI, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x3B
    { MNEM_PSHX, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x3C
    { MNEM_MUL, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x3D
    { MNEM_WAI, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x3E
    { MNEM_SWI, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x3F
    { MNEM_NEGA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x40
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x41
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x42
    { MNEM_COMA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x43
    { MNEM_LSRA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x44
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x45
    { MNEM_RORA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x46
    { MNEM_ASRA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x47
    { MNEM_ASLA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x48
    { MNEM_ROLA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x49
    { MNEM_DECA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x4A
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x4B
    { MNEM_INCA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x4C
    { MNEM_TSTA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x4D
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x4E
    { MNEM_CLRA, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x4F
    { MNEM_NEGB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x50
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x51
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x52
    { MNEM_COMB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x53
    { MNEM_LSRB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x54
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x55
    { MNEM_RORB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x56
    { MNEM_ASRB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x57
    { MNEM_ASLB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x58
    { MNEM_ROLB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x59
    { MNEM_DECB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x5A
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x5B
    { MNEM_INCB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x5C
    { MNEM_TSTB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x5D
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x5E
    { MNEM_CLRB, 1, MODE_INHERENT, OPTYPE_NONE }, // 0x5F
    { MNEM_NEG, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x60
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x61
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x62
    { MNEM_COM, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x63
    { MNEM_LSR, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x64
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x65
    { MNEM_ROR, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x66
    { MNEM_ASR, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x67
    { MNEM_ASL, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x68
    { MNEM_ROL, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x69
    { MNEM_DEC, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x6A
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x6B
    { MNEM_INC, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x6C
    { MNEM_TST, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x6D
    { MNEM_JMP, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x6E
    { MNEM_CLR, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0x6F
    { MNEM_NEG, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x70
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x71
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x72
    { MNEM_COM, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x73
    { MNEM_LSR, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x74
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x75
    { MNEM_ROR, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x76
    { MNEM_ASR, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x77
    { MNEM_ASL, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x78
    { MNEM_ROL, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x79
    { MNEM_DEC, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x7A
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x7B
    { MNEM_INC, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x7C
    { MNEM_TST, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x7D
    { MNEM_JMP, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x7E
    { MNEM_CLR, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0x7F
    { MNEM_SUBA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x80
    { MNEM_CMPA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x81
    { MNEM_SBCA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x82
    { MNEM_SUBD, 3, MODE_IMMEDIATE, OPTYPE_IMM16 }, // 0x83
    { MNEM_ANDA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x84
    { MNEM_BITA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x85
    { MNEM_LDAA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x86
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x87
    { MNEM_EORA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x88
    { MNEM_ADCA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x89
    { MNEM_ORAA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x8A
    { MNEM_ADDA, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0x8B
    { MNEM_CPX, 3, MODE_IMMEDIATE, OPTYPE_IMM16 }, // 0x8C
    { MNEM_BSR, 2, MODE_RELATIVE, OPTYPE_REL8 }, // 0x8D
    { MNEM_LDS, 3, MODE_IMMEDIATE, OPTYPE_IMM16 }, // 0x8E
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0x8F
    { MNEM_SUBA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x90
    { MNEM_CMPA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x91
    { MNEM_SBCA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x92
    { MNEM_SUBD, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x93
    { MNEM_ANDA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x94
    { MNEM_BITA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x95
    { MNEM_LDAA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x96
    { MNEM_STAA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x97
    { MNEM_EORA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x98
    { MNEM_ADCA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x99
    { MNEM_ORAA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x9A
    { MNEM_ADDA, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x9B
    { MNEM_CPX, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x9C
    { MNEM_JSR, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x9D
    { MNEM_LDS, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x9E
    { MNEM_STS, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0x9F
    { MNEM_SUBA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA0
    { MNEM_CMPA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA1
    { MNEM_SBCA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA2
    { MNEM_SUBD, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA3
    { MNEM_ANDA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA4
    { MNEM_BITA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA5
    { MNEM_LDAA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA6
    { MNEM_STAA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA7
    { MNEM_EORA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA8
    { MNEM_ADCA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xA9
    { MNEM_ORAA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xAA
    { MNEM_ADDA, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xAB
    { MNEM_CPX, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xAC
    { MNEM_JSR, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xAD
    { MNEM_LDS, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xAE
    { MNEM_STS, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xAF
    { MNEM_SUBA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB0
    { MNEM_CMPA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB1
    { MNEM_SBCA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB2
    { MNEM_SUBD, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB3
    { MNEM_ANDA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB4
    { MNEM_BITA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB5
    { MNEM_LDAA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB6
    { MNEM_STAA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB7
    { MNEM_EORA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB8
    { MNEM_ADCA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xB9
    { MNEM_ORAA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xBA
    { MNEM_ADDA, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xBB
    { MNEM_CPX, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xBC
    { MNEM_JSR, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xBD
    { MNEM_LDS, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xBE
    { MNEM_STS, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xBF
    { MNEM_SUBB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC0
    { MNEM_CMPB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC1
    { MNEM_SBCB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC2
    { MNEM_ADDD, 3, MODE_IMMEDIATE, OPTYPE_IMM16 }, // 0xC3
    { MNEM_ANDB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC4
    { MNEM_BITB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC5
    { MNEM_LDAB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC6
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0xC7
    { MNEM_EORB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC8
    { MNEM_ADCB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xC9
    { MNEM_ORAB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xCA
    { MNEM_ADDB, 2, MODE_IMMEDIATE, OPTYPE_IMM8 }, // 0xCB
    { MNEM_LDD, 3, MODE_IMMEDIATE, OPTYPE_IMM16 }, // 0xCC
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0xCD
    { MNEM_LDX, 3, MODE_IMMEDIATE, OPTYPE_IMM16 }, // 0xCE
    { MNEM_INVALID, 1, MODE_INVALID, OPTYPE_NONE }, // 0xCF
    { MNEM_SUBB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD0
    { MNEM_CMPB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD1
    { MNEM_SBCB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD2
    { MNEM_ADDD, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD3
    { MNEM_ANDB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD4
    { MNEM_BITB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD5
    { MNEM_LDAB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD6
    { MNEM_STAB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD7
    { MNEM_EORB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD8
    { MNEM_ADCB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xD9
    { MNEM_ORAB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xDA
    { MNEM_ADDB, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xDB
    { MNEM_LDD, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xDC
    { MNEM_STD, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xDD
    { MNEM_LDX, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xDE
    { MNEM_STX, 2, MODE_DIRECT, OPTYPE_DIRECT8 }, // 0xDF
    { MNEM_SUBB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE0
    { MNEM_CMPB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE1
    { MNEM_SBCB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE2
    { MNEM_ADDD, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE3
    { MNEM_ANDB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE4
    { MNEM_BITB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE5
    { MNEM_LDAB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE6
    { MNEM_STAB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE7
    { MNEM_EORB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE8
    { MNEM_ADCB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xE9
    { MNEM_ORAB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xEA
    { MNEM_ADDB, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xEB
    { MNEM_LDD, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xEC
    { MNEM_STD, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xED
    { MNEM_LDX, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xEE
    { MNEM_STX, 2, MODE_INDEXED, OPTYPE_INDEX_X }, // 0xEF
    { MNEM_SUBB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF0
    { MNEM_CMPB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF1
    { MNEM_SBCB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF2
    { MNEM_ADDD, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF3
    { MNEM_ANDB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF4
    { MNEM_BITB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF5
    { MNEM_LDAB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF6
    { MNEM_STAB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF7
    { MNEM_EORB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF8
    { MNEM_ADCB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xF9
    { MNEM_ORAB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xFA
    { MNEM_ADDB, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xFB
    { MNEM_LDD, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xFC
    { MNEM_STD, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xFD
    { MNEM_LDX, 3, MODE_EXTENDED, OPTYPE_ADDR16 }, // 0xFE
    { MNEM_STX, 3, MODE_EXTENDED, OPTYPE_ADDR16 }  // 0xFF
};

PCTSTR CDisasm6801::GetMnemonicStr( MnemonicID Id )
{
	switch ( Id ) {
	case MNEM_NOP:
		return "NOP";
	case MNEM_LSRD:
		return "LSRD";
	case MNEM_ASLD:
		return "ASLD";
	case MNEM_TAP:
		return "TAP";
	case MNEM_TPA:
		return "TPA";
	case MNEM_INX:
		return "INX";
	case MNEM_DEX:
		return "DEX";
	case MNEM_CLV:
		return "CLV";
	case MNEM_SEV:
		return "SEV";
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
	case MNEM_CLRA:
		return "CLRA";
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
	case MNEM_CLRB:
		return "CLRB";
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
	case MNEM_STAA:
		return "STAA";
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
	case MNEM_LDX:
		return "LDX";
	case MNEM_STAB:
		return "STAB";
	case MNEM_STD:
		return "STD";
	case MNEM_STX:
		return "STX";
	case MNEM_INVALID:
	default:
		return nullptr;
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
	if ( bResult ) {
		if ( m_bViewCrossReference )
			m_pLabelHandler->PrintCrossReferenceTable();
	}

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
				hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwSizeLo + 16 );
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

//VOID EntryLabel( DWORD dwLabel, DWORD dwUse )
//{
//TCHAR tsz[ MAX_PATH ];
//
//	wsprintf( tsz, _T( "L%04X : %04X\r\n" ), dwLabel, dwUse );
//	WriteFile( tsz );
//}

BOOL CDisasm6801::DoPass1( VOID )
{
BOOL bResult = FALSE;
CHAR scOfs;
BYTE byOpcode, byType, byMneId;
PBYTE pbyData;
DWORD dwAddr, dwLength, dwCurAddress, dwTmp;
LABEL_KIND bKind;
POpcodeInfo pInfo;

	if ( !m_hBin )
		return bResult;
	pbyData = (PBYTE)GlobalLock( m_hBin );
	if ( pbyData ) {
		//m_dwPC = m_dwStartAddress;
		dwAddr = 0;
		while ( dwAddr < m_dwSizeBin ) {
			dwCurAddress = ( dwAddr + m_dwStartAddress ) & 0xffff;
			byOpcode = pbyData[ dwAddr ];
	        pInfo = &g_tblOpcode[ byOpcode ];
			dwLength = pInfo->byLength;
			if ( pInfo->byMnemonicId == MNEM_INVALID ) {
				dwAddr += dwLength;
				continue;
			}
			byType = pInfo->byType;
			byMneId = pInfo->byMnemonicId;
			bKind = _KIND_NORMAL;
			switch( byType ) {
			case OPTYPE_REL8:
				scOfs = /*( signed char )*/(CHAR)pbyData[ dwAddr + 1 ];
				dwTmp = (DWORD)( (LONG)dwCurAddress + (LONG)dwLength + (LONG)scOfs ) & 0xffff;
				if ( ( byMneId == MNEM_JMP ) || ( byMneId == MNEM_JSR ) )
					bKind = _KIND_JUMP;
				m_pLabelHandler->RegisterLabel( bKind, dwTmp, dwCurAddress );
				break;
			case OPTYPE_ADDR16:
				bKind = _KIND_EXTENDED;
				dwTmp = ( DWORD )( ( (DWORD)pbyData[ dwAddr + 1 ] << 8 ) | (DWORD)pbyData[ dwAddr + 2 ] );
				if ( ( byMneId == MNEM_JMP ) || ( byMneId == MNEM_JSR ) )
					bKind = _KIND_JUMP;
				m_pLabelHandler->RegisterLabel( bKind, dwTmp, dwCurAddress );
				break;
			case OPTYPE_DIRECT8:
				dwTmp = ( DWORD )pbyData[ dwAddr + 1 ];
				if ( ( byMneId == MNEM_JMP ) || ( byMneId == MNEM_JSR ) )
					bKind = _KIND_JUMP;
				m_pLabelHandler->RegisterLabel( bKind, dwTmp, dwCurAddress );
				break;
			case OPTYPE_INDEX_X:
			case OPTYPE_IMM8:
			case OPTYPE_IMM16:
			case OPTYPE_NONE:
			default:
				break;
			}
			dwAddr += dwLength;
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
BYTE byOpcode, byType;
CHAR scOfs;
PBYTE pbyData;
DWORD i, dwAddr, dwCurAddress, dwLength, dwTmp;
TCHAR tsz[ MAX_PATH * 3 ], tszTmp[ MAX_PATH ], tszAddress[ MAX_PATH ], tszMachineCode[ MAX_PATH ], tszOperand[ MAX_PATH ];
PTSTR ptszLabel;
PCTSTR pctszMnemonic;
POpcodeInfo pInfo;

	if ( !m_hBin )
		return bResult;
	ZeroMemory( tszOperand, sizeof( tszOperand ) );

	pbyData = (PBYTE)GlobalLock( m_hBin );
	if ( pbyData ) {
		//m_dwPC = m_dwStartAddress;
		dwAddr = 0;
		while ( dwAddr < m_dwSizeBin ) {
			_tcscpy( tszOperand, _T( "" ) );
			_tcscpy( tszAddress, _T( "" ) );
			_tcscpy( tszMachineCode, _T( "" ) );
			ptszLabel = nullptr;
			byOpcode = pbyData[ dwAddr ];
	        pInfo = &g_tblOpcode[ byOpcode ];

			dwLength = pInfo->byLength;
			dwCurAddress = ( dwAddr + m_dwStartAddress ) & 0xffff;
			m_pLabelHandler->TouchUsedAddr( dwCurAddress );
			if ( !m_bNoPass2 ) {
				if ( m_bViewReferencedFrom )
					m_pLabelHandler->ViewReference( dwCurAddress );
				if ( m_bViewAddress ) {
					wsprintf( tszAddress, _T( "%04X  " ), dwCurAddress );
				}
				if ( m_bViewMachineCode ) {
					for ( i = 0; i < dwLength; i++ ) {
						wsprintf( tszTmp, _T( "%02X " ), pbyData[ dwAddr + i ] );
						_tcscat( tszMachineCode, tszTmp );
					}
				}
				ptszLabel = m_pLabelHandler->GetLabel( dwCurAddress );
				pctszMnemonic = GetMnemonicStr( (MnemonicID)pInfo->byMnemonicId );
				if ( pctszMnemonic ) {
					byType = pInfo->byType;
					switch ( byType ) {
					case OPTYPE_NONE:
						break;
					case OPTYPE_IMM8:
						wsprintf( tszOperand, _T( "#$%02X" ), pbyData[ dwAddr + 1 ] );
						break;
					case OPTYPE_IMM16:
						wsprintf( tszOperand, _T( "#$%04X" ), ( (WORD)pbyData[ dwAddr + 1 ] << 8 ) | (WORD)pbyData[ dwAddr + 2 ] );
						break;
					case OPTYPE_DIRECT8:
						dwTmp = (DWORD)pbyData[ dwAddr + 1 ] & 0xFFFF;
						m_pLabelHandler->TouchUsedAddr( dwTmp );
						if ( m_pLabelHandler->hasLabel( dwTmp ) ) {
							wsprintf( tszOperand, _T( "%s" ), m_pLabelHandler->GetLabelName( dwTmp ) );
						} else {
							wsprintf( tszOperand, _T( "$%02X" ), dwTmp );
						}
						break;
					case OPTYPE_ADDR16:
						dwTmp = ( ( (WORD)pbyData[ dwAddr + 1 ] << 8 ) | (WORD)pbyData[ dwAddr + 2 ] ) & 0xFFFF;
						m_pLabelHandler->TouchUsedAddr( dwTmp );
						if ( m_pLabelHandler->hasLabel( dwTmp ) ) {
							wsprintf( tszOperand, "%s", m_pLabelHandler->GetLabelName( dwTmp ) );
						} else {
							wsprintf( tszOperand, "$%04X", dwTmp );
						}
						break;
					case OPTYPE_REL8:
						scOfs = /*(signed char)*/(CHAR)pbyData[ dwAddr + 1 ];
						dwTmp = (DWORD)( (LONG)dwCurAddress + (LONG)dwLength + (LONG)scOfs ) & 0xFFFF;
						m_pLabelHandler->TouchUsedAddr( dwTmp );
						if ( m_pLabelHandler->hasLabel( dwTmp ) ) {
							wsprintf( tszOperand, _T( "%s" ), m_pLabelHandler->GetLabelName( dwTmp ) );
						} else {
							wsprintf( tszOperand, _T( "$%04X" ), dwTmp );
						}
						break;
					case OPTYPE_INDEX_X:
						wsprintf( tszOperand, _T( "$%02X,X" ), pbyData[ dwAddr + 1 ] );
						break;
					default:
						DebugBreak();
						break;
					}
				}
				FillMemory( tsz, sizeof( tsz ) - 1, ' ' );
				tsz[ _countof( tsz ) - 1 ] = '\0';
				CopyMemory( tsz, tszAddress, _tcslen( tszAddress ) );
				CopyMemory( tsz + 6, tszMachineCode, _tcslen( tszMachineCode ) );
				if ( ptszLabel ) {
					CopyMemory( tsz + 6 + 3 * 3 + 2, ptszLabel, _tcslen( ptszLabel ) );
					tsz[ 6 + 3 * 3 + 2 + _tcslen( ptszLabel ) ] = ':';
				}
				if ( pctszMnemonic ) {
					if ( ptszLabel ) {
						if ( _tcslen( ptszLabel ) > 12 ) {
							CutLastSpace( tsz, _countof( tsz ) );
							_tcscat( tsz, _T( "\r\n" ) );
							ConvertToUseTab( tsz, m_uiTab );
							WriteString( tsz );
							FillMemory( tsz, sizeof( tsz ) - 1, ' ' );
							tsz[ sizeof( tsz ) - 1 ] = '\0';
						}
					}
					CopyMemory( tsz + 6 + 3 * 3 + 2 + 12 + 3, pctszMnemonic, _tcslen( pctszMnemonic ) );
					CopyMemory( tsz + 6 + 3 * 3 + 2 + 12 + 3 + 8, tszOperand, _tcslen( tszOperand ) );
				} else {
					_tcscpy( tsz + 6 + 3 * 3 + 2 + 12 + 3, _T( "???" ) );
				}
				CutLastSpace( tsz, _countof( tsz ) );
				_tcscat( tsz, _T( "\r\n" ) );
				ConvertToUseTab( tsz, m_uiTab );
				WriteString( tsz );
			}
			dwAddr += dwLength;
		}
		GlobalUnlock( m_hBin );
	}
	return bResult;
}

VOID CDisasm6801::WriteToFile( PTSTR ptszStr )
{
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
//
	m_bViewCrossReference = TRUE;
	m_bNoPass2 = FALSE;
	m_bViewReferencedFrom = TRUE;
	m_bViewMachineCode = TRUE;
	m_bViewAddress = TRUE;
	m_uiTab = 4;
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

