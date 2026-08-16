#pragma once

#include "resource.h"
#include "Disasm6801.h"

VOID AddMessage( PTSTR ptszStr );
VOID WriteString( PTSTR ptszStr );
BOOL CutLastSpace( PTSTR ptszStr, INT iSize );
BOOL ConvertToUseTab( PTSTR ptszStr, UINT uiSizeTab );
VOID DispError( VOID );

