#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"

#define _MAX_LABEL ( MAX_PATH )
#define _MAX_ADDRESS ( 65536 )

#define _SUPPORT_LABEL_ALIAS

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

#ifdef _SUPPORT_LABEL_ALIAS
	struct LabelNameNode;
	typedef LabelNameNode* PLabelNameNode;
	struct LabelNameNode {
		TCHAR tszName[ _MAX_LABEL ];
		PLabelNameNode pNext;

		LabelNameNode( PCTSTR ptszName, PLabelNameNode pNextNode = nullptr ) : pNext( pNextNode ) {
			if ( ptszName ) {
				//_tcsncpy( tszName, ptszName, _MAX_LABEL - 1 );
				//tszName[ _MAX_LABEL - 1 ] = _T( '\0' );
				_tcsncpy_s( tszName, _MAX_LABEL, ptszName, _TRUNCATE );
			} else {
				tszName[ 0 ] = _T( '\0' );
			}
		}
	};
#endif

	struct LabelInfo;
	typedef LabelInfo* PLabelInfo;
	struct LabelInfo {
		BOOL bTarget;
		BOOL bUsed;	// Used in Pass2?( for test )
		TCHAR tszLabel[ _MAX_LABEL ];
		BOOL bUserDefined;
		PLabelNode pLabelList;
#ifdef _SUPPORT_LABEL_ALIAS
		PLabelNameNode pAliasList;
#endif

#ifndef _SUPPORT_LABEL_ALIAS
		LabelInfo() : bTarget( FALSE ), bUsed( FALSE ), bUserDefined( FALSE ), pLabelList( nullptr ) {
#else
		LabelInfo() : bTarget( FALSE ), bUsed( FALSE ), bUserDefined( FALSE ), pAliasList( nullptr ), pLabelList( nullptr ) {
#endif
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
#ifdef _SUPPORT_LABEL_ALIAS
	PLabelNameNode GetLabelAliasList( DWORD dwAddr );
#endif
//
	PTSTR GetLabelName( DWORD dwAddr );
	BOOL hasLabel( DWORD dwAddr );
	BOOL ExportLabelsToFile( PTSTR ptszFilename );
	BOOL ImportLabelsFromFile( PTSTR ptszFilename );
private:
#ifdef _SUPPORT_LABEL_ALIAS
	BOOL AddLabelAlias( DWORD dwAddr, PCTSTR pctszLabel, BOOL bAtHead = FALSE );
#endif
	LabelInfo m_labels[ _MAX_ADDRESS ];
};

