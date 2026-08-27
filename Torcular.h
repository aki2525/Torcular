#pragma once

#include "framework.h"
#include "resource.h"

VOID AddMessage( PTSTR ptszStr );
VOID WriteString( PTSTR ptszStr );
BOOL CutLastSpace( PTSTR ptszStr, INT iSize );
BOOL ConvertToUseTab( PTSTR ptszStr, UINT uiSizeTab );
BOOL GetOptionViewWindow( VOID );
VOID SetOptionViewWindow( BOOL bOptionView );
BOOL GetFileWriteError( BOOL bViewError );
VOID CloseWriteFileHandle( HANDLE hFile );
VOID SetWriteFileHandle( HANDLE hFile );
BOOL WriteToFile( PTSTR ptszStr );
VOID DispError( VOID );
