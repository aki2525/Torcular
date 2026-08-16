#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"

#define _MAX_LABEL ( MAX_PATH )
#define _MAX_ADDRESS ( 65536 )

enum LABEL_KIND {
	_KIND_NORMAL,
	_KIND_JUMP,
	_KIND_EXTENDED,
	_KIND_DIRECT,
	_KIND_INDEXED,
	_KIND_DW,
	_KIND_VECTOR,
	_KIND_MAX,
};

class CLabelHandler {
public:
	struct LabelNode;
	typedef LabelNode* PLabelNode;
	struct LabelNode {
		DWORD dwFromAddr;
		PLabelNode pNext;

		LabelNode( DWORD dwFrom, PLabelNode pNode = nullptr ) : dwFromAddr( dwFrom ), pNext( pNode ) {}
	};

	struct LabelInfo {
		BOOL bTarget;
		BOOL bUsed;	// Used in Pass2?( for test )
		TCHAR tszLabel[ _MAX_LABEL ];
		PLabelNode pLabelList;

		LabelInfo() : bTarget( FALSE ), bUsed( FALSE ), pLabelList( nullptr ) {
			tszLabel[ 0 ] = '\0';
		}
	};

public:
	CLabelHandler();
	~CLabelHandler();

	VOID Init();

// for pass 1
	VOID RegisterLabel( LABEL_KIND lkKind, DWORD dwAddr, DWORD dwFromAddr );
	VOID RegisterDWLabel( PBYTE pbyData, DWORD dwPC, DWORD dwAddr ); // for DW
	VOID RegisterVector( PBYTE pbyData, DWORD dwBaseAddr, DWORD dwVectorAddr, PTSTR ptszVectorName = nullptr );

	VOID SetLabelName( DWORD dwAddr, PTSTR ptszName );

// for pass 2
	BOOL PrintLabelIfExists( DWORD dwAddr );
	VOID PrintCrossReferenceTable( VOID );
	BOOL TouchUsedAddr( DWORD dwAddr );
	BOOL ViewReference( DWORD dwAddr );
	PTSTR GetLabel( DWORD dwAddr );
//
	PTSTR GetLabelName( DWORD dwAddr );
	BOOL hasLabel( DWORD dwAddr );
	BOOL ExportLabelsToFile( PTSTR ptszFilename );
	BOOL ImportLabelsFromFile( PTSTR ptszFilename );
private:
	LabelInfo m_labels[ _MAX_ADDRESS ];
};

