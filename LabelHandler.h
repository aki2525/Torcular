#pragma once

#include "framework.h"

#define _MAX_LABEL ( MAX_PATH )
#define _MAX_COMMENT ( MAX_PATH )
#define _MAX_ADDRESS ( 65536 )

#define _SUPPORT_LABEL_ALIAS

// for 
#define LABEL_FLAG_NONE        0x0000
#define LABEL_FLAG_USERDEFINED 0x0001
#define LABEL_FLAG_EQU 0x8000  // for EQU

enum LABEL_KIND {
	_KIND_NORMAL,
	_KIND_JUMP,
	_KIND_EXTENDED,
	_KIND_DIRECT,
	_KIND_INDEXED,
	_KIND_DW,
	_KIND_VECTOR,
	_KIND_EQU,
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
		TCHAR tszComment[ _MAX_COMMENT ];
		BOOL bIsEqu;
		PLabelNameNode pNext;

		LabelNameNode( PCTSTR ptszName, PCTSTR ptszComment = nullptr, PLabelNameNode pNextNode = nullptr, BOOL bEqu = FALSE ) : pNext( pNextNode ), bIsEqu( bEqu ) {
			if ( ptszName ) {
				//_tcsncpy( tszName, ptszName, _MAX_LABEL - 1 );
				//tszName[ _MAX_LABEL - 1 ] = _T( '\0' );
				_tcsncpy_s( tszName, _MAX_LABEL, ptszName, _TRUNCATE );
			} else {
				tszName[ 0 ] = _T( '\0' );
			}
			if ( ptszComment ) {
				_tcsncpy_s( tszComment, _MAX_COMMENT, ptszComment, _TRUNCATE );
			} else {
				tszComment[ 0 ] = _T( '\0' );
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
		BOOL bUserDefined; // LABEL_FLAG_USERDEFINED, LABEL_FLAG_EQU, LABEL_FLAG_NONE
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


	VOID RegisterEqu( DWORD dwAddr, PCTSTR pctszEQUName = nullptr, PCTSTR pctszComment = nullptr, DWORD dwFlags = LABEL_FLAG_EQU );
	VOID RegisterVector( PBYTE pbyData, DWORD dwBaseAddr, DWORD dwVectorAddr, PCTSTR pctszVectorName = nullptr, PCTSTR pctszComment = nullptr );
	BOOL SetCommentByName( DWORD dwAddr, PCTSTR pctszName, PCTSTR pctszComment );
	PCTSTR GetCommentByName( DWORD dwAddr, PCTSTR pctszName );
// for pass 1
	VOID RegisterLabel( LABEL_KIND lkKind, DWORD dwAddr, DWORD dwFromAddr );
	VOID RegisterDWLabel( PBYTE pbyData, DWORD dwPC, DWORD dwAddr ); // for DW

	VOID SetLabelName( DWORD dwAddr, PCTSTR pctszName, PCTSTR pctszComment = nullptr );
	VOID SetEquName( DWORD dwAddr, PCTSTR pctszName, PCTSTR pctszComment = nullptr );
	BOOL IsEqu( DWORD dwAddr );
	VOID InitSystemRegisters( VOID );
	INT GetEquList( PLabelInfo pOutArray, INT iMaxCount );

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
	PTSTR GetLabelName( DWORD dwAddr, BOOL bGetEqu = FALSE );
	PTSTR GetEquName( DWORD dwAddr );

	BOOL hasLabel( DWORD dwAddr );
	BOOL hasName( DWORD dwAddr );
	BOOL ExportToStream( HANDLE hFile );
	BOOL ImportFromBuffer( PCSTR pcszBuffer );
private:
#ifdef _SUPPORT_LABEL_ALIAS
	BOOL AddLabelAlias( DWORD dwAddr, PCTSTR pctszLabel, PCTSTR pctszComment = nullptr, BOOL bAtHead = FALSE, BOOL bIsEqu = FALSE );
	BOOL ExportAddressLabels( HANDLE hFile, DWORD dwAddr, BOOL bEqu );
	VOID ProcessImportedLabel( DWORD dwAddr, PCSTR pcszBufName, PCTSTR pctszComment, BOOL bIsEqu );
#endif
	LabelInfo m_labels[ _MAX_ADDRESS ];
};

