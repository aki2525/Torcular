#include "AddressAttrHandler.h"

BOOL CAddressAttrHandler::ExportDataAttrs( HANDLE hFile )
{
TCHAR tsz[ MAX_PATH * 3 ];
DWORD dwWrite, dwWritten;
DWORD dwAddr0;
BOOL bResult = TRUE;
BOOL bSameType ;
PCTSTR pctszType;
DWORD dwAddr, dwNextAddr, dwRangeEnd;

	if ( !hFile )
		return FALSE;

	wsprintf( tsz, _T( "\r\nDATA :\r\n" ) );
	dwWrite = (DWORD)_tcslen( tsz ) * sizeof( TCHAR );
	WriteFile( hFile, tsz, dwWrite, &dwWritten, NULL );
	if ( dwWrite != dwWritten ) {
		//DispError();
		return FALSE;
	}

	dwAddr0 = 0;
	while ( dwAddr0 < _MAX_ADDRESS ) {
		dwAddr = dwAddr0;

		if ( !IsData( dwAddr ) ) {
			dwAddr0++;
			continue;
		}
		pctszType = _T( "DB" );
		if ( IsDW( dwAddr ) )
			pctszType = _T( "DW" );
		else if ( IsDC( dwAddr ) )
			pctszType = _T( "DC" );
		else if ( IsDB( dwAddr ) )
			pctszType = _T( "DB" );

		dwRangeEnd = dwAddr0;
		while ( ( dwRangeEnd + 1 ) < _MAX_ADDRESS ) {
			dwNextAddr = dwRangeEnd + 1;
			if ( !IsData( dwNextAddr ) )
				break;

			bSameType = FALSE;
			if ( ( _tcscmp( pctszType, _T( "DW" ) ) == 0 ) && IsDW( dwNextAddr ) )
				bSameType = TRUE;
			else if ( ( _tcscmp( pctszType, _T( "DC" ) ) == 0 ) && IsDC( dwNextAddr ) )
				bSameType = TRUE;
			else if ( ( _tcscmp( pctszType, _T( "DB" ) ) == 0 ) && IsDB( dwNextAddr ) )
				bSameType = TRUE;

			if ( !bSameType )
				break;
			dwRangeEnd++;
		}
		if ( dwAddr0 == dwRangeEnd ) {
			wsprintf( tsz, _T( "$%04X : %s\r\n" ), dwAddr0, pctszType );
		} else {
			wsprintf( tsz, _T( "$%04X-$%04X : %s\r\n" ), dwAddr0, dwRangeEnd, pctszType );
		}
		dwWrite = (DWORD)_tcslen( tsz ) * sizeof( TCHAR );
		WriteFile( hFile, tsz, dwWrite, &dwWritten, NULL );
		if ( dwWrite != dwWritten ) {
			//DispError();
			bResult = FALSE;
			break;
		}
		dwAddr0 = dwRangeEnd + 1;
	}

	return bResult;
}

BOOL CAddressAttrHandler::ImportDataAttrs( PCSTR pcszBuffer )
{
INT n, nLen;
BOOL bDigit;
BOOL bResult = FALSE, bDataSection = FALSE;
PCSTR p;
DWORD dwStart, dwEnd;
TCHAR tszType[ MAX_PATH ];
EAddressAttr eAttr;

	if ( !pcszBuffer )
		return FALSE;

	bResult = TRUE;
	p = pcszBuffer;
	while ( *p != '\0' ) {
		while ( ( *p == ' ' ) || ( *p == '\t' ) )
			p++;
		if ( ( *p == '\n' ) || ( *p == '\r' ) ) {
			p++;
			continue;
		}
		if ( *p == '\0' )
			break;

		if ( ( *p == ';' ) || ( *p == '#' ) ) {
			while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) )
				p++;
			continue;
		}

		if ( _tcsnicmp( p, _T( "LABEL" ), 5 ) == 0 ) {
			bDataSection = FALSE;
			while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) )
				p++;
			continue;
		} else if ( _tcsnicmp( p, _T( "DATA" ), 4 ) == 0 ) {
			bDataSection = TRUE;
			while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) )
				p++;
			continue;
		}
		if ( *p == '$' )
			p++;

		dwStart = 0;
		bDigit = FALSE;
		while ( *p != '\0' ) {
			n = -1;
			if ( ( *p >= '0' ) && ( *p <= '9' ) )
				n = *p - '0';
			else if ( ( *p >= 'a' ) && ( *p <= 'f' ) )
				n = *p - 'a' + 10;
			else if ( ( *p >= 'A' ) && ( *p <= 'F' ) )
				n = *p - 'A' + 10;

			if ( n >= 0 ) {
				dwStart = ( dwStart << 4 ) | n;
				bDigit = TRUE;
				p++;
			} else {
				break;
			}
		}
		if ( !bDigit ) {
			while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) )
				p++;
			continue;
		}

		dwEnd = dwStart;

		// has a '-'? ... this is area
		if ( *p == '-' ) {
			p++;
			if ( *p == '$' )
				p++;
			dwEnd = 0;
			while ( *p != '\0' ) {
				n = -1;
				if ( ( *p >= '0' ) && ( *p <= '9' ) )
					n = *p - '0';
				else if ( ( *p >= 'a' ) && ( *p <= 'f' ) )
					n = *p - 'a' + 10;
				else if ( ( *p >= 'A' ) && ( *p <= 'F' ) )
					n = *p - 'A' + 10;

				if ( n >= 0 ) {
					dwEnd = ( dwEnd << 4 ) | n;
					p++;
				} else {
					break;
				}
			}
		}

		// skip separator
		while ( ( *p == ' ' ) || ( *p == '\t' ) )
			p++;
		if ( *p == ':' )
			p++;
		while ( ( *p == ' ' ) || ( *p == '\t' ) )
			p++;

		// get a token(DB / DW / DC)
		ZeroMemory( tszType, sizeof( tszType ) );
		nLen = 0;
		while ( ( *p != '\0' ) && ( * p != ' ' ) && ( *p != '\t' ) && ( *p != '\r' ) && ( *p != '\n' ) ) {
			if ( nLen < sizeof( tszType ) - 1 ) {
				tszType[ nLen++ ] = *p;
			}
			p++;
		}
		tszType[ nLen ] = '\0';

		// regist
		eAttr = ATTR_NONE;
		if ( !_tcsicmp( tszType, _T( "DW" ) ) )
			eAttr = ATTR_DW;
		else if ( !_tcsicmp( tszType, _T( "DC" ) ) )
			eAttr = ATTR_DC;
		else if ( !_tcsicmp( tszType, _T( "DB" ) ) )
			eAttr = ATTR_DB;

		// in DATA Section or Token was attr
		if ( ( eAttr != ATTR_NONE ) || bDataSection ) {
			if ( eAttr == ATTR_NONE )
				eAttr = ATTR_DB;
			SetAttrRange( dwStart, dwEnd, eAttr );
		}
		while ( ( *p != '\0' ) && ( *p != '\n' ) && ( *p != '\r' ) )
			p++;
	}
	return bResult;
}

VOID CAddressAttrHandler::ClearAll()
{
	ZeroMemory( m_byAttr, sizeof( m_byAttr ) );
}

VOID CAddressAttrHandler::Init()
{
	ClearAll();
}

VOID CAddressAttrHandler::SetAttr( DWORD dwAddr, EAddressAttr attr )
{
	if ( dwAddr >= _MAX_ADDRESS )
		return;
	m_byAttr[ dwAddr ] = attr;
}

EAddressAttr CAddressAttrHandler::GetAttr( DWORD dwAddr )
{
	if ( dwAddr >= _MAX_ADDRESS )
		return ATTR_NONE;

	return (EAddressAttr)m_byAttr[ dwAddr ];
}

VOID CAddressAttrHandler::SetAttrRange( DWORD dwStartAddr, DWORD dwEndAddr, EAddressAttr attr )
{
DWORD dwAddr;

	if ( dwStartAddr >= _MAX_ADDRESS )
		return;
	if ( dwEndAddr >= _MAX_ADDRESS )
		return;
	if ( dwStartAddr > dwEndAddr )
		return;

	for ( dwAddr = dwStartAddr; dwAddr <= dwEndAddr; dwAddr++ ) {
		m_byAttr[ dwAddr ] = attr;
	}
}

BOOL CAddressAttrHandler::IsCode( DWORD dwAddr )
{
BOOL bResult = FALSE;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( GetAttr( dwAddr ) & ATTR_CODE )
		bResult = TRUE;
	return bResult;
}

BOOL CAddressAttrHandler::IsData( DWORD dwAddr )
{
BOOL bResult = FALSE;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( GetAttr( dwAddr ) & ATTR_ANY_DATA )
		bResult = TRUE;
	return bResult;
}

BOOL CAddressAttrHandler::IsDB( DWORD dwAddr )
{
BOOL bResult = FALSE;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( GetAttr( dwAddr ) & ATTR_DB )
		bResult = TRUE;
	return bResult;
}

BOOL CAddressAttrHandler::IsDW( DWORD dwAddr )
{
BOOL bResult = FALSE;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( GetAttr( dwAddr ) & ATTR_DW )
		bResult = TRUE;
	return bResult;
}

BOOL CAddressAttrHandler::IsDC( DWORD dwAddr )
{
BOOL bResult = FALSE;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( GetAttr( dwAddr ) & ATTR_DC )
		bResult = TRUE;
	return bResult;
}

BOOL CAddressAttrHandler::IsTail( DWORD dwAddr )
{
BOOL bResult = FALSE;

	if ( dwAddr >= _MAX_ADDRESS )
		return bResult;
	if ( GetAttr( dwAddr ) & ATTR_TAIL )
		bResult = TRUE;
	return bResult;
}

CAddressAttrHandler::CAddressAttrHandler()
{
	Init();
}

CAddressAttrHandler::~CAddressAttrHandler()
{
}




