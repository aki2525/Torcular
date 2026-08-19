#pragma once

#include "framework.h"
#include "LabelHandler.h"
#include "AddressAttrHandler.h"

enum AddressingMode {
	MODE_INVALID = 0,
	MODE_INHERENT, // inherent ... opcode only
	MODE_IMMEDIATE, // immediate(#$xx)
	MODE_DIRECT, // direct($xx)
	MODE_INDEXED, // extended($xxxx)
	MODE_EXTENDED, // index($xx,X)
	MODE_RELATIVE // relative($xx)
};

enum OperandType {
	OPTYPE_NONE = 0,
	OPTYPE_IMM8,
	OPTYPE_IMM16,
	OPTYPE_DIRECT8,
	OPTYPE_INDEX_X,
	OPTYPE_ADDR16,
	OPTYPE_REL8
};

enum MnemonicID {
	MNEM_INVALID = 0, // 0x00
	MNEM_NOP, // 0x01
	MNEM_LSRD, // 0x04
	MNEM_ASLD, // 0x05
	MNEM_TAP, // 0x06
	MNEM_TPA, // 0x07
	MNEM_INX, // 0x08
	MNEM_DEX, // 0x09
	MNEM_CLV, // 0x0A
	MNEM_SEV, // 0x0B
	MNEM_CLC, // 0x0C
	MNEM_SEC, // 0x0D
	MNEM_CLI, // 0x0E
	MNEM_SEI, // 0x0F
	MNEM_SBA, // 0x10
	MNEM_CBA, // 0x11
	MNEM_TAB, // 0x16
	MNEM_TBA, // 0x17
// 0x18, XGDX ... not implemented in MC6801V0
	MNEM_DAA, // 0x19
// 01A, SLP ... not implemented in MC6801V0
	MNEM_ABA, // 0x1B
	MNEM_BRA, // 0x20
	MNEM_BRN, // 0x21
	MNEM_BHI, // 0x22
	MNEM_BLS, // 0x23
	MNEM_BCC, // 0x24
	MNEM_BCS, // 0x25
	MNEM_BNE, // 0x26
	MNEM_BEQ, // 0x27
	MNEM_BVC, // 0x28
	MNEM_BVS, // 0x29
	MNEM_BPL, // 0x2A
	MNEM_BMI, // 0x2B
	MNEM_BGE, // 0x2C
	MNEM_BLT, // 0x2D
	MNEM_BGT, // 0x2E
	MNEM_BLE, // 0x2F
	MNEM_TSX, // 0x30
	MNEM_INS, // 0x31
	MNEM_PULA, // 0x32
	MNEM_PULB, // 0x33
	MNEM_DES, // 0x34
	MNEM_TXS, // 0x35
	MNEM_PSHA, // 0x36
	MNEM_PSHB, // 0x37
	MNEM_PULX, // 0x38
	MNEM_RTS, // 0x39
	MNEM_ABX, // 0x3A
	MNEM_RTI, // 0x3B
	MNEM_PSHX, // 0x3C
	MNEM_MUL, // 0x3D
	MNEM_WAI, // 0x3E
	MNEM_SWI, // 0x3F
	MNEM_NEGA, // 0x40
	MNEM_COMA, // 0x43
	MNEM_LSRA, // 0x44
	MNEM_RORA, // 0x46
	MNEM_ASRA, // 0x47
	MNEM_ASLA, // 0x48
	MNEM_ROLA, // 0x49
	MNEM_DECA, // 0x4A
	MNEM_INCA, // 0x4C
	MNEM_TSTA, // 0x4D
	MNEM_CLRA, // 0x4F
	MNEM_NEGB, // 0x50
	MNEM_COMB, // 0x53
	MNEM_LSRB, // 0x54
	MNEM_RORB, // 0x56
	MNEM_ASRB, // 0x57
	MNEM_ASLB, // 0x58
	MNEM_ROLB, // 0x59
	MNEM_DECB, // 0x5A
	MNEM_INCB, // 0x5C
	MNEM_TSTB, // 0x5D
	MNEM_CLRB, // 0x5F
	MNEM_NEG, // 0x60
	MNEM_COM, // 0x63
	MNEM_LSR, // 0x64
	MNEM_ROR, // 0x66
	MNEM_ASR, // 0x67
	MNEM_ASL, // 0x68
	MNEM_ROL, // 0x69
	MNEM_DEC, // 0x6A
	MNEM_INC, // 0x6C
	MNEM_TST, // 0x6D
	MNEM_JMP, // 0x6E
	MNEM_CLR, // 0x6F
	MNEM_SUBA, // 0x80
	MNEM_CMPA, // 0x81
	MNEM_SBCA, // 0x82
	MNEM_SUBD, // 0x83
	MNEM_ANDA, // 0x84
	MNEM_BITA, // 0x85
	MNEM_LDAA, // 0x86
	MNEM_EORA, // 0x88
	MNEM_ADCA, // 0x89
	MNEM_ORAA, // 0x8A
	MNEM_ADDA, // 0x8B
	MNEM_CPX, // 0x8C
	MNEM_BSR, // 0x8D
	MNEM_LDS, // 0x8E
	MNEM_STAA, // 0x97
	MNEM_JSR, // 0x9D
	MNEM_STS, // 0x9F
	MNEM_SUBB, // 0xC0
	MNEM_CMPB, // 0xC1
	MNEM_SBCB, // 0xC2
	MNEM_ADDD, // 0xC3
	MNEM_ANDB, // 0xC4
	MNEM_BITB, // 0xC5
	MNEM_LDAB, // 0xC6
	MNEM_EORB, // 0xC8
	MNEM_ADCB, // 0xC9
	MNEM_ORAB, // 0xCA
	MNEM_ADDB, // 0xCB
	MNEM_LDD, // 0xCC
	MNEM_LDX, // 0xCE
	MNEM_STAB, // 0xD7
	MNEM_STD, // 0xDD
	MNEM_STX // 0xDF
};

typedef struct {
	BYTE byMnemonicId;
	BYTE byLength; // opcode bytes
	BYTE byMode; // addressing mode( 3bit )
	BYTE byType; // OperandType
} OpcodeInfo, *POpcodeInfo;


class CDisasm6801 {
public:
	CDisasm6801();
	~CDisasm6801();
//
	BOOL DoDisasm();
	BOOL SetBinFile( PTSTR ptBinFile );
	BOOL ReadBinFile( VOID );
	BOOL ReadLabelFile( VOID );
	BOOL DoPass1( VOID );
	BOOL CreateAsmFile( VOID );
	BOOL DoPass2( VOID );
	VOID WriteToFile( PTSTR ptszStr );
	VOID CloseFiles( VOID );
	VOID Init( VOID );
	BOOL Set6801Vector( VOID );

	BOOL ImportProject( PTSTR ptszFilename );
	BOOL ExportProject( PTSTR ptszFilename );

	PCTSTR GetMnemonicStr( MnemonicID Id );
	CLabelHandler* m_pLabelHandler;
	CAddressAttrHandler* m_pAttrHandler;
private:
	DWORD OutputDataDirective( PBYTE pbyData, DWORD dwAddr, DWORD dwCurAddress );
//
	TCHAR m_tszBinPath[ MAX_PATH * 2 ];
	HGLOBAL m_hBin;
	PBYTE m_pbyBin;
	DWORD m_dwSizeBin;
	//DWORD m_dwPC;
	//DWORD m_dwAdr; // in m_pbyBin
// Mode / Options
	BOOL m_bViewCrossReference;
	BOOL m_bNoPass2;
	BOOL m_bViewReferencedFrom;
	BOOL m_bViewMachineCode;
	UINT m_uiTab;
	BOOL m_bViewAddress;
//
	DWORD m_dwStartAddress;
};

