#include "LabelHandler.h"
#include "Torcular.h" // for WriteString()

VOID CLabelHandler::RegisterLabel( LABEL_KIND lkKind, DWORD dwAddr, DWORD dwFromAddr )
{ // lkKind ... not implemnted, yet.
PLabelNode pPrev, pCurr, pNewNode;
#ifdef _SUPPORT_LABEL_ALIAS
TCHAR tszDefaultName[ _MAX_LABEL ];
#endif

	if ( dwAddr >= _MAX_ADDRESS )
		return;

	m_labels[ dwAddr ].bTarget = TRUE;

#ifndef _SUPPORT_LABEL_ALIAS
	if ( !_tcslen( m_labels[ dwAddr ].tszLabel ) ) {
		wsprintf( m_labels[ dwAddr ].tszLabel, "L%04X", dwAddr );
	}
#else
	if ( !m_labels[ dwAddr ].pAliasList ) {
		wsprintf( tszDefaultName, _T( "L%04X" ), dwAddr );
		AddLabelAlias( dwAddr, tszDefaultName, FALSE );
	}
#endif

	pPrev = NULL;
	pCurr = m_labels[ dwAddr ].pLabelList;

	while ( pCurr != NULL ) {
		if ( pCurr->dwFromAddr == dwFromAddr ) {
			return;
		}
		if ( pCurr->dwFromAddr > dwFromAddr ) {
			break;
		}
		pPrev = pCurr;
		pCurr = pCurr->pNext;
	}

	// insert new node
	pNewNode = new LabelNode( dwFromAddr, pCurr );
	if ( pPrev == NULL ) {
		m_labels[ dwAddr ].pLabelList = pNewNode;
	} else {
		pPrev->pNext = pNewNode;
	}
}

VOID CLabelHandler::RegisterDWLabel( PBYTE pbyData, DWORD dwPC, DWORD dwAddr )
{
DWORD dwTarget;

	if ( !pbyData )
		return;

	dwTarget = ( ( (DWORD)pbyData[ dwPC ] << 8 ) | (DWORD)pbyData[ dwPC + 1 ] );
	RegisterLabel( _KIND_DW, dwTarget, dwAddr );
}

VOID CLabelHandler::RegisterVector( PBYTE pbyData, DWORD dwBaseAddr, DWORD dwVectorAddr, PTSTR ptszVectorName )
{
DWORD dwTarget, dwOfs;

	if ( !pbyData )
		return;
    if ( dwVectorAddr < dwBaseAddr ) {
		return;
	}
	dwOfs = dwVectorAddr - dwBaseAddr;
	dwTarget = ( ( (DWORD)pbyData[ dwOfs ] << 8 ) | (DWORD)pbyData[ dwOfs + 1 ] );
	RegisterLabel( _KIND_VECTOR, dwTarget, dwVectorAddr );

	if ( ptszVectorName ) {
		if ( _tcslen( ptszVectorName ) ) {
			SetLabelName( dwTarget, ptszVectorName );
		}
	}
	//dwTarget = ( ( (DWORD)pbyData[ dwPC ] << 8 ) | (DWORD)pbyData[ dwPC + 1 ] );
	//RegisterLabel( dwTarget, XREF_FROM_NONE );
}

VOID CLabelHandler::SetLabelName( DWORD dwAddr, PTSTR ptszName )
{
	if ( dwAddr >= _MAX_ADDRESS )
		return;
	if ( !ptszName )
		return;
	if ( !_tcslen( ptszName ) )
		return;

#ifndef _SUPPORT_LABEL_ALIAS
	m_labels[ dwAddr ].bTarget = TRUE;
	ZeroMemory( m_labels[ dwAddr ].tszLabel, sizeof( m_labels[ dwAddr ].tszLabel ) );
	_tcsncpy( m_labels[ dwAddr ].tszLabel, ptszName, ( _MAX_LABEL - 1 ) );
#else
	AddLabelAlias( dwAddr, ptszName, TRUE );
#endif
}

PTSTR CLabelHandler::GetLabelName( DWORD dwAddr )
{
PTSTR ptsResult = nullptr;

	if ( dwAddr >= _MAX_ADDRESS )
		return nullptr;

	if ( m_labels[ dwAddr ].bTarget ) {
#ifndef _SUPPORT_LABEL_ALIAS
		ptsResult = m_labels[ dwAddr ].tszLabel;
#else
		if ( m_labels[ dwAddr ].pAliasList )
			ptsResult = m_labels[ dwAddr ].pAliasList->tszName;
#endif
	}
	return ptsResult;

}

BOOL CLabelHandler::hasLabel( DWORD dwAddr )
{
	if ( dwAddr >= _MAX_ADDRESS )
		return FALSE;
#ifdef _SUPPORT_LABEL_ALIAS
	if ( !m_labels[ dwAddr ].pAliasList )
		return FALSE;
#endif
	return m_labels[ dwAddr ].bTarget;
}

#ifdef _SUPPORT_LABEL_ALIAS
BOOL CLabelHandler::ExportAddressLabels( HANDLE hFile, DWORD dwAddr )
{
BOOL bResult = FALSE;
TCHAR tsz[ MAX_PATH * 3 ];
DWORD dwWrite, dwWritten;
PLabelNameNode pNode;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;

	bResult = TRUE;
	pNode = m_labels[ dwAddr ].pAliasList;
	while ( pNode != nullptr ) {
		if ( pNode->tszName[ 0 ] != _T( '\0' ) ) {
			wsprintf( tsz, _T( "$%04X : %s\r\n" ), dwAddr, pNode->tszName );
			dwWrite = (DWORD)_tcslen( tsz );
			if ( !WriteFile( hFile, tsz, dwWrite, &dwWritten, NULL ) ) {
				bResult = FALSE;
			}
			if ( dwWrite != dwWritten ) {
				bResult = FALSE;
			}
			if ( !bResult ) {
				DispError();
				break;
			}
		}
		pNode = pNode->pNext;
	}
	return bResult;
}
#endif

BOOL CLabelHandler::ExportLabelsToFile( PTSTR ptszFilename )
{
BOOL bResult = FALSE;
TCHAR tsz[ MAX_PATH * 3 ];
DWORD i, dwWrite, dwWritten;
HANDLE hFile;

	if ( !ptszFilename )
		return bResult;

	hFile = CreateFile( ptszFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE ) {
		DispError();
		hFile = nullptr;
	} else {
		bResult = TRUE;
		wsprintf( tsz, _T( "LABEL : \r\n" ) );
		dwWrite = (DWORD)_tcslen( tsz );
		WriteFile( hFile, tsz, dwWrite, &dwWritten, NULL );
		if ( dwWrite != dwWritten ) {
			DispError();
			bResult = FALSE;
		}
	}
	for ( i = 0; i < _MAX_ADDRESS; i++ ) {
		if ( !bResult )
			break;
		if ( m_labels[ i ].bTarget ) {
#ifndef _SUPPORT_LABEL_ALIAS
			wsprintf( tsz, "$%04X : %s\r\n", i, m_labels[ i ].tszLabel );
			dwWrite = (DWORD)_tcslen( tsz );
			WriteFile( hFile, tsz, dwWrite, &dwWritten, NULL );
			if ( dwWrite != dwWritten ) {
				DispError();
				bResult = FALSE;
			}
#else
			bResult = ExportAddressLabels( hFile, i );
#endif
		}
	}
	if ( hFile )
		CloseHandle( hFile );
	return bResult;
}

#ifdef _SUPPORT_LABEL_ALIAS
VOID CLabelHandler::ProcessImportedLabel( DWORD dwAddr, PCSTR pcszBufName )
{
	if ( dwAddr >= _MAX_ADDRESS )
		return;
	if ( !pcszBufName )
		return;
	if ( !pcszBufName[ 0 ] == '\0' )
		return;

#ifdef UNICODE
	TCHAR tszName[ _MAX_LABEL ];
	MultiByteToWideChar( CP_ACP, 0, pcszBufName, -1, tszName, _MAX_LABEL );
	AddLabelName( dwAddr, tszName, FALSE );
#else
	AddLabelAlias( dwAddr, pcszBufName, FALSE );
#endif
}
#endif

BOOL CLabelHandler::ImportLabelsFromFile( PTSTR ptszFilename )
{
INT iDigit, iLenName;
BOOL bResult = FALSE;
BOOL bDigit;
CHAR cBufName[ _MAX_LABEL ];
DWORD dwSizeLo, dwSizeHi, dwRead;
DWORD dwAddr;
PCHAR pBuffer = NULL, p;
HANDLE hFile;
HGLOBAL hGlobal = NULL;

	if ( !ptszFilename )
		return bResult;

	hFile = CreateFile( ptszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE ) {
		DispError();
		hFile = nullptr;
	} else {
		dwSizeLo = GetFileSize( hFile, &dwSizeHi );
		bResult = TRUE;
	}
	if ( bResult ) {
		if ( dwSizeLo ) {
			hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwSizeLo + 63 );
		}
		if ( hGlobal )
			pBuffer = (PCHAR)GlobalLock( hGlobal );
		if ( pBuffer ) {
			ReadFile( hFile, pBuffer, dwSizeLo, &dwRead, NULL );
			if ( dwRead != dwSizeLo ) {
				DispError();
				bResult = FALSE;
			}
			CloseHandle( hFile );
		}
		if ( bResult ) {
			p = pBuffer;
			while ( *p != '\0' ) {
				while ( ( *p == ' ' ) || ( *p == '\t' ) ) {
					p++;
				}
				if ( ( *p == '\n' ) || ( *p == '\r' ) ) {
					p++;
					continue;
				}
				if ( *p == '\0' ) {
					break;
				}
// Skip Comment lines
				if ( ( *p == ';' ) || ( *p == '#' ) ) {
					while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) ) {
						p++;
					}
					continue;
				}
				if ( *p == '$' ) {
					p++;
				}
				dwAddr = 0;
				bDigit = FALSE;
				while ( *p != '\0' ) {
					iDigit = -1;
					if ( ( *p >= '0' ) && (*p <= '9' ) )
						iDigit = *p - '0';
					else if ( ( *p >= 'a' ) && ( *p <= 'f' ) )
						iDigit = *p - 'a' + 10;
					else if ( ( *p >= 'A' ) && ( *p <= 'F' ) )
						iDigit = *p - 'A' + 10;
					if ( iDigit >= 0 ) {
						dwAddr = ( dwAddr << 4 ) | iDigit;
						bDigit = TRUE;
						p++;
					} else {
						break;
					}
				}
				if ( !bDigit ) {
					while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) ) {
						p++;
					}
					continue;
				}
				while ( ( *p == ' ' ) || ( *p == '\t' ) ) {
					p++;
				}
				if ( *p == ':' ) {
					p++;
				}
				while ( ( *p == ' ' ) || ( *p == '\t' ) ) {
					p++;
				}
//
				iLenName = 0;
				ZeroMemory( cBufName, sizeof( cBufName ) );
				while ( ( *p != '\0' ) && ( *p != ' ' ) && ( *p != '\t' ) && ( *p != '\r' ) && ( *p != '\n' ) ) {
					if ( iLenName < _MAX_LABEL ) {
						cBufName[ iLenName ] = *p;
						iLenName++;
					}
					p++;
				}
				cBufName[ iLenName ] = '\0';
				if ( iLenName > 0 && dwAddr < _MAX_ADDRESS ) {
					SetLabelName( dwAddr, cBufName );
				}
				while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) ) {
					p++;
				}
			}
		}
		if ( pBuffer )
			GlobalUnlock( hGlobal );
		if ( hGlobal )
			GlobalFree( hGlobal );
	}
	return bResult;
}

BOOL CLabelHandler::TouchUsedAddr( DWORD dwAddr )
{
BOOL bResult = FALSE;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( !m_labels[ dwAddr ].bTarget ) {
		return bResult;
	}
	bResult = TRUE;
	m_labels[ dwAddr ].bUsed = TRUE;

	return bResult;
}

BOOL CLabelHandler::ViewReference( DWORD dwAddr )
{ // If that address eferenced, view it.
BOOL bResult = FALSE;
TCHAR tsz[ MAX_PATH ];
PLabelNode pCurr;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( !m_labels[ dwAddr ].bTarget ) {
		return bResult;
	}
	if ( !m_labels[ dwAddr ].bUsed ) {
		return bResult;
	}
	bResult = TRUE;

	pCurr = m_labels[ dwAddr ].pLabelList;
	if ( pCurr != NULL ) {
		wsprintf( tsz, _T( "; Referenced from: " ) );
		WriteString( tsz );
		while ( pCurr != NULL ) {
			wsprintf( tsz, _T( "$%04X" ), pCurr->dwFromAddr );
			WriteString( tsz );
			pCurr = pCurr->pNext;
			if ( pCurr != NULL ) {
				wsprintf( tsz, _T( ", " ) );
				WriteString( tsz );
			}
		}
		wsprintf( tsz, _T( "\r\n" ) );
		WriteString( tsz );
	}
	return bResult;
}

PTSTR CLabelHandler::GetLabel( DWORD dwAddr )
{ // If a label is registered for that address, return it.
	if ( dwAddr >= _MAX_ADDRESS )
		return nullptr;
	if ( !m_labels[ dwAddr ].bTarget ) {
		return nullptr;
	}
	if ( !m_labels[ dwAddr ].bUsed ) {
		return nullptr;
	}
	return m_labels[ dwAddr ].tszLabel;
}

#ifdef _SUPPORT_LABEL_ALIAS
BOOL CLabelHandler::AddLabelAlias( DWORD dwAddr, PCTSTR pctszLabel, BOOL bAtHead )
{
BOOL bResult = FALSE;
TCHAR tszDefaultName[ _MAX_LABEL ];
PLabelInfo pInfo;
PLabelNameNode pCurr, pPrev, pCurrDel, pPrevDel, pNewNode;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( !pctszLabel )
		return bResult;
	if ( !_tcslen( pctszLabel ) )
		return bResult;

	pInfo = &m_labels[ dwAddr ];
	wsprintf( tszDefaultName, _T( "L%04X" ), dwAddr );
	if ( _tcscmp( pctszLabel, tszDefaultName ) ) {
		pCurrDel = pInfo->pAliasList;
		pPrevDel = nullptr;
		while ( pCurrDel != nullptr ) {
			if ( !_tcscmp( pCurrDel->tszName, tszDefaultName ) ) {
				if ( pPrevDel == nullptr ) {
					pInfo->pAliasList = pCurrDel->pNext;
				} else {
					pPrevDel->pNext = pCurrDel->pNext;
				}
				delete pCurrDel;
				break;
			}
			pPrevDel = pCurrDel;
			pCurrDel = pCurrDel->pNext;
		}
	}


	pCurr = pInfo->pAliasList;
	pPrev = nullptr;
	while ( pCurr != nullptr ) {
		if ( _tcscmp( pCurr->tszName, pctszLabel ) == 0 ) {
			bResult = TRUE;
			break;
		}
		pPrev = pCurr;
		pCurr = pCurr->pNext;
	}
	if ( !bResult ) {
		pNewNode = new LabelNameNode( pctszLabel );
		if ( pNewNode ) {
			bResult = TRUE;
			if ( bAtHead || pInfo->pAliasList == nullptr ) {
				pNewNode->pNext = pInfo->pAliasList;
				pInfo->pAliasList = pNewNode;
			} else {
				pPrev->pNext = pNewNode;
			}
			pInfo->bTarget = TRUE;
		}
	}
	return bResult;
}

CLabelHandler::PLabelNameNode CLabelHandler::GetLabelAliasList( DWORD dwAddr )
{
	if ( dwAddr >= _MAX_ADDRESS )
		return nullptr;
	if ( !m_labels[dwAddr].bTarget )
		return nullptr;

	m_labels[ dwAddr ].bUsed = TRUE;
	return m_labels[ dwAddr ].pAliasList;
}
#endif

BOOL CLabelHandler::PrintLabelIfExists( DWORD dwAddr )
{
BOOL bResult = FALSE;
TCHAR tsz[ MAX_PATH ];
PLabelNode pCurr;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( !m_labels[ dwAddr ].bTarget ) {
		return bResult;
	}
	bResult = TRUE;
	m_labels[ dwAddr ].bUsed = TRUE;

	pCurr = m_labels[ dwAddr ].pLabelList;
	if ( pCurr != NULL ) {
		wsprintf( tsz, _T( "; Referenced from: " ) );
		WriteString( tsz );
		while ( pCurr != NULL ) {
			wsprintf( tsz, _T( "$%04X" ), pCurr->dwFromAddr );
			WriteString( tsz );
			pCurr = pCurr->pNext;
			if ( pCurr != NULL ) {
				wsprintf( tsz, _T( ", " ) );
				WriteString( tsz );
			}
		}
		wsprintf( tsz, _T( "\r\n" ) );
		WriteString( tsz );
	}
	wsprintf( tsz, _T( "%s:\t" ), m_labels[ dwAddr ].tszLabel );
	WriteString( tsz );
	return bResult;
}

VOID CLabelHandler::PrintCrossReferenceTable( VOID )
{
LONG i, lCnt;
BOOL bFound = FALSE;
TCHAR tsz[ MAX_PATH * 3 ];
PLabelNode pCurr;

	wsprintf( tsz, _T( "\r\n========================================\r\n" ) );
	WriteString( tsz );
	wsprintf( tsz, _T( "\t\tCROSS REFERENCE TABLE\r\n" ) );
	WriteString( tsz );
	wsprintf( tsz, _T( "========================================\r\n" ) );
	WriteString( tsz );
#ifndef _SUPPORT_LABEL_ALIAS
	wsprintf( tsz, _T( "Label (Aliases)        Referenced From\r\n" ) );
	WriteString( tsz );
	wsprintf( tsz, _T( "----------------------------------------\r\n" ) );
#else
	wsprintf( tsz, _T( "Label (Aliases)                           Referenced From\r\n" ) );
	WriteString( tsz );
	wsprintf( tsz, _T( "------------------------------------------------------------------------\r\n" ) );
#endif
	WriteString( tsz );

	lCnt = 0;
	for ( i = 0; i < _MAX_ADDRESS; i++ ) {
		if ( m_labels[ i ].bTarget ) {
#ifndef _SUPPORT_LABEL_ALIAS
			lCnt++;
			bFound = TRUE;
			wsprintf( tsz, _T( "%-20s : " ), m_labels[ i ].tszLabel );
			WriteString( tsz );

			if ( !m_labels[ i ].bUsed ) {
				wsprintf( tsz, _T( " [WARNING: Unreached Address] " ) );
				WriteString( tsz );
			}

			pCurr = m_labels[ i ].pLabelList;
			if ( pCurr == NULL ) {
				wsprintf( tsz, _T( "(None)" ) );
				WriteString( tsz );
			} else {
				while ( pCurr != NULL ) {
					wsprintf( tsz, _T( "$%04X" ), pCurr->dwFromAddr );
					WriteString( tsz );
					pCurr = pCurr->pNext;
					if ( pCurr != NULL ) {
						wsprintf( tsz, _T( ", " ) );
						WriteString( tsz );
					}
				}
			}
			wsprintf( tsz, _T( "\r\n" ) );
			WriteString( tsz );
#else
			if ( m_labels[ i ].pAliasList != nullptr ) {
				lCnt++;
				bFound = TRUE;

				// 1. 代表ラベル名を出力
				PLabelNameNode pPrimary = m_labels[ i ].pAliasList;
				wsprintf( tsz, _T( "%-12s : " ), pPrimary->tszName );
				WriteString( tsz );

				// 2. 未到達警告の出力
				if ( !m_labels[ i ].bUsed ) {
					wsprintf( tsz, _T( " [WARNING: Unreached Address] " ) );
					WriteString( tsz );
				}

				// 3. 参照元 (XRef) アドレス一覧の出力
				pCurr = m_labels[ i ].pLabelList;
				if ( pCurr == NULL ) {
					wsprintf( tsz, _T( "(None)" ) );
					WriteString( tsz );
				} else {
					while ( pCurr != NULL ) {
						wsprintf( tsz, _T( "$%04X" ), pCurr->dwFromAddr );
						WriteString( tsz );
						pCurr = pCurr->pNext;
						if ( pCurr != NULL ) {
							wsprintf( tsz, _T( ", " ) );
							WriteString( tsz );
						}
					}
				}
				wsprintf( tsz, _T( "\r\n" ) );
				WriteString( tsz );

				// 4. エイリアスが存在する場合、インデントしてアドレスとエイリアス一覧を出力
				PLabelNameNode pAlias = pPrimary->pNext;
				if ( pAlias != nullptr ) {
					wsprintf( tsz, _T( "  [ =$%04X: " ), i );
					WriteString( tsz );
					while ( pAlias != nullptr ) {
						wsprintf( tsz, _T( "%s" ), pAlias->tszName );
						WriteString( tsz );
						pAlias = pAlias->pNext;
						if ( pAlias != nullptr ) {
							wsprintf( tsz, _T( ", " ) );
							WriteString( tsz );
						}
					}
					wsprintf( tsz, _T( " ]\r\n" ) );
					WriteString( tsz );
				}
			}
#endif
		}
	}

	if ( !bFound ) {
		wsprintf( tsz, _T( "(No cross references found)\r\n" ) );
		WriteString( tsz );
	}

	wsprintf( tsz, _T( "========================================\n\n" ) );
	WriteString( tsz );
	wsprintf( tsz, _T( "%d Label(s)\n\n" ), lCnt );
	WriteString( tsz );
}

CLabelHandler::CLabelHandler()
{
	Init();
}

CLabelHandler::~CLabelHandler()
{
	Init();
}

VOID CLabelHandler::Init( VOID )
{
ULONG i;
PLabelNode pNode, pTemp;

	for ( i = 0; i < _MAX_ADDRESS; ++i ) {
		pNode = m_labels[ i ].pLabelList;
		while ( pNode != NULL ) {
			pTemp = pNode->pNext;
			delete pNode;
			pNode = pTemp;
		}
		m_labels[ i ].pLabelList = nullptr;
		m_labels[ i ].bTarget = FALSE;
		m_labels[ i ].bUsed = FALSE;
		ZeroMemory( m_labels[ i ].tszLabel, sizeof( m_labels[ i ].tszLabel ) );
	}
}

