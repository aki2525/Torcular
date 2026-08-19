#pragma once

#include "framework.h"
#include "resource.h"

VOID AddMessage( PTSTR ptszStr );
VOID WriteString( PTSTR ptszStr );
BOOL CutLastSpace( PTSTR ptszStr, INT iSize );
BOOL ConvertToUseTab( PTSTR ptszStr, UINT uiSizeTab );
VOID DispError( VOID );
