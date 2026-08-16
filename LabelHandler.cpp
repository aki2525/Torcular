#include "LabelHandler.h"
#include "Torcular.h" // for AddMessage()

VOID CLabelHandler::RegisterLabel( DWORD dwAddr, DWORD dwFromAddr )
{
PLabelNode pPrev, pCurr, pNewNode;

	m_labels[ dwAddr ].bTarget = TRUE;

	if ( !_tcslen( m_labels[ dwAddr ].tszLabel ) ) {
		wsprintf( m_labels[ dwAddr ].tszLabel, "L%04X", dwAddr );
	}

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
	RegisterLabel( dwTarget, dwAddr );
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
	RegisterLabel( dwTarget, dwVectorAddr );

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
	m_labels[ dwAddr ].bTarget = TRUE;
	ZeroMemory( m_labels[ dwAddr ].tszLabel, sizeof( m_labels[ dwAddr ].tszLabel ) );
	_tcsncpy( m_labels[ dwAddr ].tszLabel, ptszName, ( _MAX_LABEL - 1 ) );
}

PTSTR CLabelHandler::GetLabelName( DWORD dwAddr )
{
PTSTR ptsResult = nullptr;

	if ( m_labels[ dwAddr ].bTarget ) {
		ptsResult = m_labels[ dwAddr ].tszLabel;
	}
	return ptsResult;
}

BOOL CLabelHandler::hasLabel( DWORD dwAddr )
{
	if ( dwAddr >= _MAX_ADDRESS )
		return FALSE;
	return m_labels[ dwAddr ].bTarget;
}

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
			wsprintf( tsz, "$%04X : %s\r\n", i, m_labels[ i ].tszLabel );
			dwWrite = (DWORD)_tcslen( tsz );
			WriteFile( hFile, tsz, dwWrite, &dwWritten, NULL );
			if ( dwWrite != dwWritten ) {
				DispError();
				bResult = FALSE;
			}
		}
	}
	if ( hFile )
		CloseHandle( hFile );
	return bResult;
}

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

	if ( ptszFilename )
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
		AddMessage( tsz );
		while ( pCurr != NULL ) {
			wsprintf( tsz, _T( "$%04X" ), pCurr->dwFromAddr );
			AddMessage( tsz );
			pCurr = pCurr->pNext;
			if ( pCurr != NULL ) {
				wsprintf( tsz, _T( ", " ) );
				AddMessage( tsz );
			}
		}
		wsprintf( tsz, _T( "\r\n" ) );
		AddMessage( tsz );
	}
	wsprintf( tsz, _T( "%s:\t" ), m_labels[ dwAddr ].tszLabel );
	AddMessage( tsz );
	return bResult;
}

VOID CLabelHandler::PrintCrossReferenceTable( VOID )
{
LONG i, lCnt;
BOOL bFound = FALSE;
TCHAR tsz[ MAX_PATH * 3 ];
PLabelNode pCurr;

	wsprintf( tsz, _T( "\r\n========================================\r\n" ) );
	AddMessage( tsz );
	wsprintf( tsz, _T( "       CROSS REFERENCE TABLE            \r\n" ) );
	AddMessage( tsz );
	wsprintf( tsz, _T( "========================================\r\n" ) );
	AddMessage( tsz );
	wsprintf( tsz, _T( "Label       Referenced From             \r\n" ) );
	AddMessage( tsz );
	wsprintf( tsz, _T( "----------------------------------------\r\n" ) );
	AddMessage( tsz );

	lCnt = 0;
	for ( i = 0; i < _MAX_ADDRESS; i++ ) {
		if ( m_labels[ i ].bTarget ) {
			lCnt++;
			bFound = TRUE;
			wsprintf( tsz, _T( "%-11s" ), m_labels[ i ].tszLabel );
			AddMessage( tsz );

			if ( !m_labels[ i ].bUsed ) {
				wsprintf( tsz, _T( " [WARNING: Unreached Address] " ) );
				AddMessage( tsz );
			}

			pCurr = m_labels[ i ].pLabelList;
			if ( pCurr == NULL ) {
				wsprintf( tsz, _T( "(None)" ) );
				AddMessage( tsz );
			} else {
				while ( pCurr != NULL ) {
					wsprintf( tsz, _T( "$%04X" ), pCurr->dwFromAddr );
					AddMessage( tsz );
					pCurr = pCurr->pNext;
					if ( pCurr != NULL ) {
						wsprintf( tsz, _T( ", " ) );
						AddMessage( tsz );
					}
				}
			}
			wsprintf( tsz, _T( "\n" ) );
			AddMessage( tsz );
		}
	}

	if ( !bFound ) {
		wsprintf( tsz, _T( "(No cross references found)\r\n" ) );
		AddMessage( tsz );
	}

	wsprintf( tsz, _T( "========================================\n\n" ) );
	AddMessage( tsz );
	wsprintf( tsz, _T( "%d Label(s)\n\n" ), lCnt );
	AddMessage( tsz );
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

