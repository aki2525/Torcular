#pragma once

#include "framework.h"
#include "Torcular.h" // for DISP_ERROR, 
#include "LabelHandler.h" // for _MAX_ADDRESS

enum EAddressAttr : BYTE {
	ATTR_NONE = 0x00, // Undefind
	ATTR_CODE = 0x01, // Code
	ATTR_DATA = 0x02, // Data
	ATTR_DB = 0x04, // Byte data(FCB)
	ATTR_DW = 0x08, // Word data / Pointer(FDB)
	ATTR_DC = 0x10,  // String(FCC)
//
	ATTR_TAIL = 0x20,

	ATTR_DATA0 = ( ATTR_DB | ATTR_DW | ATTR_DC ),
	ATTR_ANY_DATA = ( ATTR_DATA | ATTR_DB | ATTR_DW | ATTR_DC ),
};

class CAddressAttrHandler
{
private:
	BYTE m_byAttr[ _MAX_ADDRESS ];

public:
	CAddressAttrHandler();
	virtual ~CAddressAttrHandler();

	BOOL ExportDataAttrs( HANDLE hFile );
	BOOL ImportDataAttrs( PCSTR pcszBuffer );
//
	VOID ClearAll();
	VOID Init();

	VOID SetAttr( DWORD dwAddr, EAddressAttr attr );
	EAddressAttr GetAttr( DWORD dwAddr );

	VOID SetAttrRange( DWORD dwStartAddr, DWORD dwEndAddr, EAddressAttr attr );

	BOOL IsCode( DWORD dwAddr );
	BOOL IsData( DWORD dwAddr );
	BOOL IsDB( DWORD dwAddr );
	BOOL IsDW( DWORD dwAddr );
	BOOL IsDC( DWORD dwAddr );
	BOOL IsTail( DWORD dwAddr );
};

