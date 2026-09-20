// Torcular.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include <shobjidl.h>
#include <commdlg.h>
#include "Torcular.h"
#include "Disasm6801.h"

// options...
PCSTR OPT_HELP[] = { "help", "h", NULL };
PCSTR OPT_INPUT[] = { "input", NULL };

// Globals...
HINSTANCE g_hInstance;
TCHAR g_tszTitle[ MAX_PATH ];
TCHAR g_tszWindowClass[ MAX_PATH ];
HWND g_hwndView = NULL;
LONG g_lClientX, g_lClientY;
CDisasm6801* g_pThis = nullptr;

// Statics...
static HANDLE g_hToWrite = nullptr;
static BOOL g_bWriteError = FALSE;
static DWORD g_dwWriteError = 0;

// Option...
BOOL g_bViewToWindow = FALSE;

// Locals...
ATOM MyRegisterClass( HINSTANCE hInstance );
BOOL InitInstance( HINSTANCE hInstance, INT nCmdShow );
LRESULT CALLBACK WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK About( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL GetOption( VOID );
BOOL OpenBinaryFile( HWND hwnd );
BOOL MakeDisassemble( HWND hwnd );
BOOL MakeCrossReference( HWND hwnd );
BOOL MakeDump( HWND hwnd );
BOOL ImportProjectFile( HWND hwnd );
BOOL ExportProjectFile( HWND hwnd );
BOOL ChooseViewFont( HWND hwnd, HWND hView );

COMDLG_FILTERSPEC fpProjTypes[] = {
	{ L"Project file(*.prj68)", L"*.prj68" },
	{ L"All file(*.*)", L"*.*" }
};
COMDLG_FILTERSPEC fpBinTypes[] = {
	{ L"Binary file(*.bin)", L"*.bin" },
	{ L"Binary file(*.rom)", L"*.rom" },
	{ L"All file(*.*)", L"*.*" }
};
COMDLG_FILTERSPEC fpDisasmTypes[] = {
	{ L"Disasm file(*.disasm68)", L"*.disasm68" },
	{ L"All file(*.*)", L"*.*" }
};
COMDLG_FILTERSPEC fpDumpTypes[] = {
	{ L"dump file(*.dump68)", L"*.dump68" },
	{ L"dump file(*.dump)", L"*.dump" },
	{ L"dump file(*.dmp)", L"*.dmp" },
	{ L"All file(*.*)", L"*.*" }
};
COMDLG_FILTERSPEC fpXrefTypes[] = {
	{ L"Project file(*.xref68)", L"*.xref68" },
	{ L"All file(*.*)", L"*.*" }
};

INT APIENTRY wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ INT nCmdShow )
{
MSG msg;
HACCEL hAccelTable;
HRESULT hr;
HMODULE hRichEdit;
UNREFERENCED_PARAMETER( hPrevInstance );
UNREFERENCED_PARAMETER( pCmdLine );

	LoadString( hInstance, IDS_APP_TITLE, g_tszTitle, _countof( g_tszWindowClass ) );
	LoadString( hInstance, IDC_TORCULAR, g_tszWindowClass, _countof( g_tszWindowClass ) );
	MyRegisterClass( hInstance );

	hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );
	if ( FAILED( hr ) )
		return FALSE;

	hRichEdit = LoadLibrary( _T( "Msftedit.dll" ) );

	if ( !InitInstance( hInstance, nCmdShow ) ) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_TORCULAR ) );
	while( GetMessage( &msg, nullptr, 0, 0 ) ) {
		if ( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) ) {
			TranslateMessage(&msg );
			DispatchMessage( &msg );
		}
	}

	if ( hRichEdit )
		FreeLibrary( hRichEdit );
	CoUninitialize();

	return (INT)msg.wParam;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
WNDCLASSEX wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_TORCULAR ) );
	wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = MAKEINTRESOURCE( IDC_TORCULAR );
	wcex.lpszClassName = g_tszWindowClass;
	wcex.hIconSm = LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_SMALL ) );

	return RegisterClassEx( &wcex );
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance( HINSTANCE hInstance, INT nCmdShow )
{
HWND hWnd;

	g_hInstance = hInstance; // グローバル変数にインスタンス ハンドルを格納する

	InitCommonControls();
	hWnd = CreateWindow( g_tszWindowClass, g_tszTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr );

	if (! hWnd ) {
		return FALSE;
	}

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg ) {
	case WM_CREATE: {
	CHARFORMAT2 cf;

		g_hwndView = CreateWindowExW( 0, MSFTEDIT_CLASS/*RICHEDIT_CLASS*/, L"", ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWnd, ( HMENU )IDC_VIEW, g_hInstance, nullptr);
		g_pThis = new CDisasm6801;
		ZeroMemory( &cf, sizeof( cf ) );
		cf.cbSize = sizeof( cf );
		cf.dwMask = CFM_FACE | CFM_CHARSET | CFM_FACE;
		cf.bCharSet = SHIFTJIS_CHARSET;
		cf.bPitchAndFamily = FIXED_PITCH | FF_MODERN;
    
		//_tcscpy( cf.szFaceName, _T( "BIZ UDゴシック" ) );
		_tcscpy( cf.szFaceName, _T( "Consolas" ) );
		SendMessage( g_hwndView, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
		if ( g_pThis )
			PostMessage( hWnd, WM_COMMAND, IDM_HERE_WE_GO, 0 );
	}
		break;
	case WM_SIZE:
		g_lClientX = LOWORD( lParam );
		g_lClientY = HIWORD( lParam );
		MoveWindow( g_hwndView, 0, 0, g_lClientX, g_lClientY, TRUE );
		break;
	case WM_COMMAND: {
	INT wmId;
	BOOL bReady = FALSE;

		wmId = LOWORD( wParam );
		switch( wmId ) {
		case IDM_ABOUT:
			DialogBox( g_hInstance, MAKEINTRESOURCE( IDD_ABOUTBOX ), hWnd, About );
			break;
		case IDM_EXIT:
			DestroyWindow( hWnd );
			break;
		case IDM_HERE_WE_GO:
			bReady = GetOption();
			//if ( bReady )
			//	g_pThis->DoDisasm();
			if ( bReady )
				g_pThis->ReadBinFile();
			break;
		case IDM_OPEN_BINFILE:
			OpenBinaryFile( hWnd );
			break;
		case IDM_MAKE_DISASMFILE:
			MakeDisassemble( hWnd );
			break;
		case IDM_MAKE_REFFILE:
			MakeCrossReference( hWnd );
			break;
		case IDM_MAKE_DUMPFILE:
			MakeDump( hWnd );
			break;
		case IDM_IMPORT_PROJECT:
			ImportProjectFile( hWnd );
			break;
		case IDM_EXPORT_PROJECT:
			ExportProjectFile( hWnd );
			break;
		case IDM_SELECTFONT:
			ChooseViewFont( hWnd, g_hwndView );
			break;
		default:
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}
	}
	break;
	case WM_PAINT: {
	HDC hdc;
	PAINTSTRUCT ps;

		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0 );
		break;
	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

INT_PTR CALLBACK About( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
UNREFERENCED_PARAMETER( lParam );

	switch( uMsg ) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch( LOWORD( wParam ) ) {
		case IDOK:
		case IDCANCEL:
			EndDialog( hDlg, LOWORD( wParam ) );
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

VOID WriteString( PTSTR ptszStr )
{
	//if ( g_pThis )
	//	g_pThis->WriteToFile( ptszStr );
	WriteToFile( ptszStr );
	if ( g_bViewToWindow ) {
		AddMessage( ptszStr );
	}
}

VOID CloseWriteFileHandle( HANDLE hFile )
{
TCHAR tsz[ MAX_PATH ];

	if ( !g_hToWrite ) {
		_tcscpy( tsz, _T( "Warnings : write file note opened(internal error).\r\n" ) );
		AddMessage( tsz );
	} else {
		if ( g_hToWrite != hFile ) {
			_tcscpy( tsz, _T( "Warnings : write and close file handle was diffrent(internal error).\r\n" ) );
			AddMessage( tsz );
		}
		CloseHandle( g_hToWrite );
	}
	g_hToWrite = nullptr;
}

VOID SetWriteFileHandle( HANDLE hFile )
{
TCHAR tsz[ MAX_PATH ];

	if ( g_hToWrite ) {
		_tcscpy( tsz, _T( "Warnings : close writing file, yet?(internal error)\r\n" ) );
		AddMessage( tsz );
	}
	g_hToWrite = hFile;
	g_bWriteError = FALSE;
	g_dwWriteError = 0;
}

BOOL GetOptionViewWindow( VOID )
{
	return g_bViewToWindow;
}

VOID SetOptionViewWindow( BOOL bOptionView )
{
	g_bViewToWindow = bOptionView;
}

BOOL WriteToFile( PTSTR ptszStr )
{
BOOL bResult = FALSE;
DWORD dwWrite, dwWritten;

	if ( !ptszStr )
		return bResult;
	if ( !g_hToWrite )
		return bResult;
	if ( g_bWriteError )
		return bResult;

	dwWrite = (DWORD)_tcslen( ptszStr );
	bResult = WriteFile( g_hToWrite, ptszStr, dwWrite, &dwWritten, NULL );
	if ( dwWrite != dwWritten ) {
		bResult = FALSE;
	}
	if ( !bResult ) {
		g_bWriteError = TRUE;
		g_dwWriteError = GetLastError();
	}
	return bResult;
}

BOOL GetFileWriteError( BOOL bViewError )
{
PVOID pMsgBuf;

	if ( g_bWriteError ) {
		if ( bViewError ) {
			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, g_dwWriteError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (PTSTR)&pMsgBuf, 0, NULL );
			AddMessage( (PTSTR)pMsgBuf );
			LocalFree( pMsgBuf );
		}
	}
	return g_bWriteError;
}


VOID AddMessage( PCTSTR pctszStr )
{
CHARRANGE cr;

	if ( g_hwndView ) {
		//SendMessage( g_hwndView, WM_SETREDRAW, FALSE, 0 );
		cr.cpMin = -1;
		cr.cpMax = -1;
		SendMessage( g_hwndView, EM_EXSETSEL, 0, (LPARAM)&cr );
		SendMessage( g_hwndView, EM_REPLACESEL, FALSE, (LPARAM)pctszStr );
		//SendMessage( g_hwndView, WM_SETREDRAW, TRUE, 0 );
		SendMessage( g_hwndView, EM_SCROLLCARET, 0, 0 );
		//InvalidateRect( g_hwndView, NULL, TRUE );
	}
}

BOOL CutLastSpace( PTSTR ptszStr, INT iSize )
{
INT i, iLen;
BOOL bResult = FALSE;

	if ( !ptszStr )
		return bResult;

	iLen = (INT)_tcslen( ptszStr );

	for ( i = iLen - 1; i >= 0; i-- ) {
		if ( ( ptszStr[ i ] != _T( ' ' ) ) && ( ptszStr[ i ] != _T( '\t' ) ) ) {
			break;
		}
	}
	if ( iSize >= i + 1 )
		ptszStr[ i + 1 ] = '\0';

	return bResult;
}

BOOL ConvertToUseTab( PTSTR ptszStr, UINT uiSizeTab )
{
BOOL bResult = FALSE;
UINT uiColumn, uiCntSpc, uiNeed, uiUsed, uiNextTab;
PTSTR ptSrc, ptDst, ptTmp;

	if ( !ptszStr )
        return bResult;
	if ( !uiSizeTab )
        return bResult;

	bResult = TRUE;
	uiColumn = 0;
	ptSrc = ptszStr;
	ptDst = ptszStr;

	while( *ptSrc ) {
		if ( *ptSrc == _T( '\r' ) ) {
			*ptDst++ = *ptSrc++;
			continue;
		}
		if ( *ptSrc == _T( '\n' ) ) {
			*ptDst++ = *ptSrc++;
			continue;
		}
		if ( *ptSrc == _T( ' ' ) ) {
			ptTmp = ptSrc;
			uiCntSpc = 0;
			while ( *ptTmp == _T( ' ' ) ) {
				uiCntSpc++;
				ptTmp++;
			}
			uiUsed = 0;
			while ( uiUsed < uiCntSpc ) {
				uiNextTab = ( ( uiColumn / uiSizeTab ) + 1 ) * uiSizeTab;
				uiNeed = uiNextTab - uiColumn;
				if ( uiNeed <= ( uiCntSpc - uiUsed ) ) {
					*ptDst++ = _T( '\t' );
					uiColumn = uiNextTab;
					uiUsed += uiNeed;
				} else {
					break;
				}
			}
			while ( uiUsed < uiCntSpc ) {
				*ptDst++ = _T( ' ' );
				uiColumn++;
				uiUsed++;
			}
			ptSrc = ptTmp;
			continue;
		}
		if ( *ptSrc == _T( '\t' ) ) {
			*ptDst++ = _T( '\t' );
			uiColumn = ( ( uiColumn / uiSizeTab ) + 1 ) * uiSizeTab;
			ptSrc++;
			continue;
		}
		*ptDst++ = *ptSrc++;
		uiColumn++;
	}
	*ptDst = _T( '\0' );
	return bResult;

}

VOID DispError( VOID )
{
DWORD dwErr;
LPVOID pMsgBuf;

	dwErr = GetLastError();
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (PTSTR)&pMsgBuf, 0, NULL );
	AddMessage( (PTSTR)pMsgBuf );
	LocalFree( pMsgBuf );
}

//BOOL ExportDataAttrs( HANDLE hFile )
//{
//BOOL bResult = FALSE;
//
//	if ( g_pThis ) {
//		if ( g_pThis->m_pAttrHandler ) {
//			bResult = g_pThis->m_pAttrHandler->ExportDataAttrs( hFile );
//		}
//	}
//	return bResult;
//}
//
//BOOL ImportDataAttrs( PCHAR pBuffer )
//{
//BOOL bResult = FALSE;
//
//	if ( pBuffer ) {
//		if ( g_pThis ) {
//			if ( g_pThis->m_pAttrHandler ) {
//				bResult = g_pThis->m_pAttrHandler->ImportDataAttrs( pBuffer );
//			}
//		}
//	}
//	return bResult;
//}

BOOL OpenBinaryFile( HWND hwnd )
{
INT iRet;
BOOL bResult = FALSE;
TCHAR tszPath[ MAX_PATH ];
PWSTR pwszFilePath = NULL;
HRESULT hr;
IShellItem* pItem = NULL;
IFileOpenDialog* pFileOpen = NULL;

	if ( !g_pThis )
		return bResult;
	if ( !hwnd )
		return bResult;

	hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pFileOpen ) );
	if ( SUCCEEDED( hr ) ) {
		pFileOpen->SetFileTypes( _countof( fpBinTypes ), fpBinTypes );
		pFileOpen->SetFileTypeIndex( 1 );
		pFileOpen->SetTitle( L" Open binary file" );
		hr = pFileOpen->Show( hwnd );
		if ( SUCCEEDED( hr ) ) {
			hr = pFileOpen->GetResult( &pItem );
			if ( SUCCEEDED( hr ) ) {
				hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pwszFilePath );
				if ( SUCCEEDED( hr ) ) {
#ifndef _UNICODE
					iRet = WideCharToMultiByte( CP_ACP, 0, pwszFilePath, -1, tszPath, sizeof( tszPath ), NULL, NULL );
					if ( iRet > 0 ) {
						bResult = TRUE;
					}
#else
					_tcscpy( tszPath, pwszFilePath );
					bResult = TRUE;
#endif
					CoTaskMemFree( pwszFilePath );
				}
				pItem->Release();
			}
		}
		pFileOpen->Release();
	}
	if ( bResult ) {
		bResult = g_pThis->SetBinFile( tszPath );
		if ( bResult )
			bResult = g_pThis->ReadBinFile();
	}
	return bResult;
}

BOOL MakeDisassemble( HWND hwnd )
{
INT iRet;
UINT uiTab;
BOOL bResult = FALSE, bSw;
DWORD dwIdxSelect = 0;
TCHAR tsz[ MAX_PATH * 3 ], tszPath[ MAX_PATH ];
PWSTR pszFilePath = nullptr;
HANDLE hFile;
HRESULT hr;
IShellItem* pItem;
IFileSaveDialog* pFileSave = nullptr;
IFileDialogCustomize *pCustomize = nullptr;

	if ( !g_pThis ) {
		AddMessage( _T( "Internal Error\r\n" ) );
		return bResult;
	}
	//if ( !g_pThis->PrepareMakeCrossReference() )
	//	return bResult;

	hr = CoCreateInstance( CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pFileSave ) );
	if ( SUCCEEDED( hr ) ) {
		pFileSave->SetFileTypes( _countof( fpDisasmTypes ), fpDisasmTypes );
		pFileSave->SetFileTypeIndex( 1 );
		pFileSave->SetDefaultExtension( L"disasm68" );
		pFileSave->SetTitle( L"make Disassemble File" );
		hr = pFileSave->QueryInterface( IID_PPV_ARGS( &pCustomize ) );
		if ( SUCCEEDED( hr ) ) {
			pCustomize->AddCheckButton( IDC_VIEW_ADDRESS, L"View Address", g_pThis->GetViewAddress() );
			pCustomize->AddCheckButton( IDC_VIEW_MACHINECODE, L"View Machine code", g_pThis->GetViewMachineCode() );
			pCustomize->AddCheckButton( IDC_VIEW_COMMENT_LABEL, L"View Label Comment", g_pThis->GetViewLabelComment() );
			pCustomize->AddCheckButton( IDC_VIEW_COMMENT_EQU, L"View EQU Comment", g_pThis->GetViewEquComment() );
			pCustomize->AddCheckButton( IDC_VIEW_COLON_LABEL, L"View Label Colon", g_pThis->GetViewLabelColon() );
			pCustomize->AddCheckButton( IDC_VIEW_REFERENCEDFROM, L"View Referenced From", g_pThis->GetViewReferencedFrom() );
			pCustomize->AddCheckButton( IDC_VIEW_USE_DB, L"Use DB(means FCB)", g_pThis->GetViewAsDB() );
			pCustomize->AddCheckButton( IDC_VIEW_USE_DW, L"Use DW(means FDB)", g_pThis->GetViewAsDW() );
			pCustomize->AddCheckButton( IDC_VIEW_USE_DC, L"Use DC(means FCC)", g_pThis->GetViewAsDC() );
//
			pCustomize->StartVisualGroup( IDC_GROUP_ADDRESS, L"" );
			pCustomize->AddText( IDC_START_ADDRESS_CAPTION, L"Start Address : " );
			pCustomize->AddEditBox( IDC_START_ADDRESS, L"F000" );
			pCustomize->EndVisualGroup();
//
			pCustomize->StartVisualGroup( IDC_GROUP_TAB, L"" );
			pCustomize->AddText( IDC_TAB_CAPTION, L"Tab Size : " );
			pCustomize->AddComboBox( IDC_TAB_SIZE );
			pCustomize->AddControlItem( IDC_TAB_SIZE, 0, L"4" );
			pCustomize->AddControlItem( IDC_TAB_SIZE, 1, L"8" );
			uiTab = g_pThis->GetTab();
			if ( uiTab == 8 )
				pCustomize->SetSelectedControlItem( IDC_TAB_SIZE, 1 );
			else
				pCustomize->SetSelectedControlItem( IDC_TAB_SIZE, 0 );
			pCustomize->EndVisualGroup();
//
			pCustomize->Release();
		}
		hr = pFileSave->Show( hwnd );
		if ( SUCCEEDED( hr ) ) {
			hr = pFileSave->GetResult( &pItem );
			if ( SUCCEEDED( hr ) ) {
				hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );
#ifndef _UNICODE
				if ( SUCCEEDED( hr ) ) {
					iRet = WideCharToMultiByte( CP_ACP, 0, pszFilePath, -1, tszPath, sizeof( tszPath ), NULL, NULL );
					if ( iRet > 0 ) {
						bResult = TRUE;
					}
#else
					_tcscpy( tszPath, pszFilePath );
					bResult = TRUE;
#endif
					CoTaskMemFree( pszFilePath );
					pItem->Release();
				}
				pFileSave->QueryInterface( IID_PPV_ARGS( &pCustomize ) );
				if ( pCustomize ) {
					pCustomize->GetCheckButtonState( IDC_VIEW_ADDRESS, &bSw );
					g_pThis->SetViewAddress( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_MACHINECODE, &bSw );
					g_pThis->SetViewMachineCode( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_COMMENT_LABEL, &bSw );
					g_pThis->SetViewLabelComment( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_COMMENT_EQU, &bSw );
					g_pThis->SetViewEquComment( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_COLON_LABEL, &bSw );
					g_pThis->SetViewLabelColon( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_REFERENCEDFROM, &bSw );
					g_pThis->SetViewReferencedFrom( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_USE_DB, &bSw );
					g_pThis->SetViewAsDB( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_USE_DC, &bSw );
					g_pThis->SetViewAsDC( bSw );
					pCustomize->GetCheckButtonState( IDC_VIEW_USE_DW, &bSw );
					g_pThis->SetViewAsDW( bSw );

					pCustomize->GetSelectedControlItem( IDC_TAB_SIZE, &dwIdxSelect );
					switch( dwIdxSelect ) {
					case 0:
						g_pThis->SetTab( 4 );
						break;
					//case 1:
					default:
						g_pThis->SetTab( 8 );
						break;
					}
					pCustomize->Release();
				}
			}
		}
		pFileSave->Release();
	}
	if ( bResult ) {
		wsprintf( tsz, _T( "Make disassemble file to : %s ... " ), tszPath );
		AddMessage( tsz );
		hFile = CreateFile( tszPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE ) {
			AddMessage( _T( "ng\r\n" ) );
			DispError();
			return bResult;
		}
		AddMessage( _T( "\r\n" ) );
		g_hToWrite = hFile;
		bResult = g_pThis->DoDisasm();
		CloseHandle( hFile );
		if ( bResult )
			AddMessage( _T( "Succeeded.\r\n" ) );
		else
			AddMessage( _T( "Failed.\r\n" ) );
		g_hToWrite = nullptr;
	}
	AddMessage( _T( "\r\n" ) );

	return bResult;
}

BOOL MakeCrossReference( HWND hwnd )
{
INT iRet;
BOOL bResult = FALSE;
TCHAR tszPath[ MAX_PATH ];
PWSTR pszFilePath = nullptr;
HRESULT hr;
IShellItem* pItem;
IFileSaveDialog* pFileSave = nullptr;

	if ( !g_pThis ) {
		AddMessage( _T( "Internal Error\r\n" ) );
		return bResult;
	}
	if ( !g_pThis->PrepareMakeCrossReference() ) {
		AddMessage( _T( "Internal Error\r\n" ) );
		return bResult;
	}

	hr = CoCreateInstance( CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pFileSave ) );
	if ( SUCCEEDED( hr ) ) {
		pFileSave->SetFileTypes( _countof( fpXrefTypes ), fpXrefTypes );
		pFileSave->SetFileTypeIndex( 1 );
		pFileSave->SetDefaultExtension( L"xref68" );
		pFileSave->SetTitle( L"make Cross reference File" );

		hr = pFileSave->Show( hwnd );
		if ( SUCCEEDED( hr ) ) {
			hr = pFileSave->GetResult( &pItem );
			if ( SUCCEEDED( hr ) ) {
				hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );
#ifndef _UNICODE
				if ( SUCCEEDED( hr ) ) {
					iRet = WideCharToMultiByte( CP_ACP, 0, pszFilePath, -1, tszPath, sizeof( tszPath ), NULL, NULL );
					if ( iRet > 0 ) {
						bResult = TRUE;
					}
#else
					_tcscpy( tszPath, pszFilePath );
					bResult = TRUE;
#endif
					CoTaskMemFree( pszFilePath );
					pItem->Release();
				}
			}
		}
		pFileSave->Release();
	}
	if ( bResult ) {
		bResult = g_pThis->ExportCrossReferenceTable( tszPath );
	}
	if ( bResult )
		AddMessage( _T( "Succeeded.\r\n" ) );
	else
		AddMessage( _T( "Failed.\r\n" ) );
	AddMessage( _T( "\r\n" ) );

	return bResult;
}

BOOL MakeDump( HWND hwnd )
{
INT iRet;
BOOL bResult = FALSE;
TCHAR tszPath[ MAX_PATH ];
PWSTR pszFilePath = nullptr;
HRESULT hr;
IShellItem* pItem;
IFileSaveDialog* pFileSave = nullptr;

	if ( !g_pThis ) {
		AddMessage( _T( "Internal Error\r\n" ) );
		return bResult;
	}

	hr = CoCreateInstance( CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pFileSave ) );
	if ( SUCCEEDED( hr ) ) {
		pFileSave->SetFileTypes( _countof( fpDumpTypes ), fpDumpTypes );
		pFileSave->SetFileTypeIndex( 1 );
		pFileSave->SetDefaultExtension( L"dump68" );
		pFileSave->SetTitle( L"make binary dump File" );

		hr = pFileSave->Show( hwnd );
		if ( SUCCEEDED( hr ) ) {
			hr = pFileSave->GetResult( &pItem );
			if ( SUCCEEDED( hr ) ) {
				hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );
#ifndef _UNICODE
				if ( SUCCEEDED( hr ) ) {
					iRet = WideCharToMultiByte( CP_ACP, 0, pszFilePath, -1, tszPath, sizeof( tszPath ), NULL, NULL );
					if ( iRet > 0 ) {
						bResult = TRUE;
					}
#else
					_tcscpy( tszPath, pszFilePath );
					bResult = TRUE;
#endif
					CoTaskMemFree( pszFilePath );
					pItem->Release();
				}
			}
		}
		pFileSave->Release();
	}
	if ( bResult ) {
		bResult = g_pThis->DumpBinary( tszPath );
	}

	if ( bResult )
		AddMessage( _T( "Succeeded.\r\n" ) );
	else
		AddMessage( _T( "Failed.\r\n" ) );
	AddMessage( _T( "\r\n" ) );

	return bResult;
}

BOOL ImportProjectFile( HWND hwnd )
{
INT iRet;
BOOL bResult = FALSE;
TCHAR tszPath[ MAX_PATH ];
PWSTR pwszFilePath = NULL;
HRESULT hr;
IShellItem* pItem = NULL;
IFileOpenDialog* pFileOpen = NULL;

	if ( !g_pThis ) {
		AddMessage( _T( "Internal Error\r\n" ) );
		return bResult;
	}
	if ( !hwnd ) {
		AddMessage( _T( "Internal Error\r\n" ) );
		return bResult;
	}

	hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pFileOpen ) );
	if ( SUCCEEDED( hr ) ) {
		pFileOpen->SetFileTypes( _countof( fpProjTypes ), fpProjTypes );
		pFileOpen->SetFileTypeIndex( 1 );
		pFileOpen->SetTitle( L" Import Project file" );
		hr = pFileOpen->Show( hwnd );
		if ( SUCCEEDED( hr ) ) {
			hr = pFileOpen->GetResult( &pItem );
			if ( SUCCEEDED( hr ) ) {
				hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pwszFilePath );
				if ( SUCCEEDED( hr ) ) {
#ifndef _UNICODE
					iRet = WideCharToMultiByte( CP_ACP, 0, pwszFilePath, -1, tszPath, sizeof( tszPath ), NULL, NULL );
					if ( iRet > 0 ) {
						bResult = TRUE;
					}
#else
					_tcscpy( tszPath, pwszFilePath );
					bResult = TRUE;
#endif
					CoTaskMemFree( pwszFilePath );
				}
				pItem->Release();
			}
		}
		pFileOpen->Release();
	}
	if ( bResult ) {
		bResult = g_pThis->ImportProject( tszPath );
	}
	if ( bResult )
		AddMessage( _T( "Succeeded.\r\n" ) );
	else
		AddMessage( _T( "Failed.\r\n" ) );
	AddMessage( _T( "\r\n" ) );

	return bResult;
}

BOOL ExportProjectFile( HWND hwnd )
{
INT iRet;
TCHAR tszPath[ MAX_PATH ];
BOOL bResult = FALSE;
PWSTR pwszFilePath = nullptr;
HRESULT hr;
IShellItem* pItem;
IFileSaveDialog* pFileSave = nullptr;

	if ( !g_pThis )
		return bResult;
	if ( !hwnd )
		return bResult;

	hr = CoCreateInstance( CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pFileSave ) );
	if ( SUCCEEDED( hr ) ) {
		pFileSave->SetFileTypes( _countof( fpProjTypes ), fpProjTypes );
		pFileSave->SetFileTypeIndex( 1 );
		pFileSave->SetDefaultExtension( L"prj68" );
		pFileSave->SetTitle( L"Export Project File" );

		hr = pFileSave->Show( hwnd );
		if ( SUCCEEDED( hr ) ) {
			hr = pFileSave->GetResult( &pItem );
			if ( SUCCEEDED( hr ) ) {
				hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pwszFilePath );
#ifndef _UNICODE
				if ( SUCCEEDED( hr ) ) {
					iRet = WideCharToMultiByte( CP_ACP, 0, pwszFilePath, -1, tszPath, sizeof( tszPath ), NULL, NULL );
					if ( iRet > 0 ) {
						bResult = TRUE;
					}
#else
					_tcscpy( tszPath, pwszFilePath );
					bResult = TRUE;
#endif
					CoTaskMemFree( pwszFilePath );
					pItem->Release();
				}
			}
		}
		pFileSave->Release();
	}
	if ( bResult ) {
		bResult = g_pThis->ExportProject( tszPath );
	}
	if ( bResult )
		AddMessage( _T( "Succeeded.\r\n" ) );
	else
		AddMessage( _T( "Failed.\r\n" ) );
	AddMessage( _T( "\r\n" ) );

	return bResult;
}

BOOL ChooseViewFont( HWND hwnd, HWND hwndView )
{
HDC hdc;
INT iLogPixY, iSizePoint;
BOOL bResult = FALSE;
LOGFONT lf;
CHOOSEFONT cf;
CHARFORMAT2 cf2;

	if ( !hwnd )
		return bResult;
	if ( !hwndView )
		return bResult;

	ZeroMemory( &cf2, sizeof( cf2 ) );
	ZeroMemory( &lf, sizeof( lf ) );
	cf2.cbSize = sizeof( cf2 );
	SendMessage( hwndView, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf2 );

	_tcsncpy( lf.lfFaceName, cf2.szFaceName, LF_FACESIZE );
	lf.lfCharSet = cf2.bCharSet;
	if ( cf2.dwMask & CFM_BOLD ) {
		if ( cf2.dwEffects & CFE_BOLD )
			lf.lfWeight = FW_BOLD;
		else
			lf.lfWeight = FW_NORMAL;
	}
	if ( cf2.dwMask & CFM_ITALIC ) {
		if ( cf2.dwEffects & CFE_ITALIC )
			lf.lfItalic = TRUE;
		else
			lf.lfItalic = FALSE;
	}
	hdc = GetDC( hwnd );
	if ( cf2.yHeight > 0 ) {
		iLogPixY = GetDeviceCaps( hdc, LOGPIXELSY );
		iSizePoint = cf2.yHeight / 20; // 1 point as 20 twips
	} else {
		iLogPixY = GetDpiForWindow( hwnd );
		iSizePoint = 10;
	}
	lf.lfHeight = -MulDiv( iSizePoint, iLogPixY, 72 );

	ZeroMemory( &cf, sizeof( cf ) );
	cf.lStructSize = sizeof( cf );
	cf.hwndOwner = hwnd;
	cf.hDC = hdc;
	cf.lpLogFont = &lf;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT | CF_SCRIPTSONLY | CF_NOSCRIPTSEL;
//
	bResult = ChooseFont( &cf );
	ReleaseDC( hwnd, hdc );
	if ( bResult ) {
		ZeroMemory( &cf2, sizeof( cf2 ) );
		cf2.cbSize = sizeof( cf2 );
		cf2.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_CHARSET;

		_tcsncpy( cf2.szFaceName, lf.lfFaceName, LF_FACESIZE );
		cf2.bCharSet = lf.lfCharSet;

		if ( lf.lfWeight >= FW_BOLD )
			cf2.dwEffects |= CFE_BOLD;
		if ( lf.lfItalic )
			cf2.dwEffects |= CFE_ITALIC;

		hdc = GetDC( hwndView );
		iLogPixY = GetDeviceCaps( hdc, LOGPIXELSY );
		ReleaseDC( hwndView, hdc );

		iSizePoint = MulDiv( abs( lf.lfHeight ), 72, iLogPixY );
		cf2.yHeight = iSizePoint * 20;
		SendMessage( hwndView, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf2 );
	}
	return bResult;
}

// -----------------------------------------------------------------------------------------------
PCHAR _WideToAnsi( PWCHAR pwStr )
{
INT iLen;
PCHAR pcBuf;

	if ( !pwStr )
		return nullptr;

	iLen = WideCharToMultiByte( CP_ACP, 0, pwStr, -1, NULL, 0, NULL, NULL );
	pcBuf = (PCHAR)malloc( iLen );
	if ( pcBuf ) {
		WideCharToMultiByte( CP_ACP, 0, pwStr, -1, pcBuf, iLen, NULL, NULL );
	}
	return pcBuf;
}

PCHAR RemovePrefix( PCHAR pcStr )
{
PCHAR pcRetStr = nullptr;

	if ( !pcStr )
		return nullptr;

	if ( pcStr[ 0 ] == '-' ) {
		if ( pcStr[ 1 ] == '-' ) {
			pcRetStr = pcStr + 2;
		} else {
			pcRetStr = pcStr + 1;
		}
	}
	if ( pcStr[ 0 ] == '/' ) {
		pcRetStr = pcStr + 1;
	}
	if ( pcRetStr ) {
		if ( pcRetStr[ 0 ] == ' ' )
			pcRetStr++;
	} else {
		pcRetStr = pcStr;
	}
	return pcRetStr;
}

BOOL isOption( PCHAR pcArg, PCSTR* pcOption )
{
INT i;
BOOL bResult = FALSE;
PCHAR pcArg1;

	pcArg1 = RemovePrefix( pcArg );

	for ( i = 0; pcOption[ i ] != NULL; i++ ) {
		if ( !_tcsicmp( pcArg1, pcOption[ i ] ) ) {
			bResult = TRUE;
			break;
		}
	}
	return bResult;
}

VOID Usage( VOID )
{
TCHAR tsz[ MAX_PATH ];
TCHAR tsz0[ MAX_PATH ];

	GetModuleFileName( NULL, tsz0, sizeof( tsz0 ) );
	PathStripPath( tsz0 );
	wsprintf( tsz, _T( "Usage : %s - input bin6801.bin" ), tsz0 );
	AddMessage( tsz );
}

BOOL GetOption( VOID )
{
BOOL bResult = FALSE;
INT i, iArgcW = 0;
PWSTR* pArgvW;
PCHAR* pArgvA;
char* inputFile = NULL;
BOOL bUsage = FALSE;

	pArgvW = CommandLineToArgvW( GetCommandLineW(), &iArgcW );
	if ( !pArgvW )
		return 0;

	if ( iArgcW < 2 )
		bUsage = TRUE;

	pArgvA = (PCHAR*)malloc( sizeof( PCHAR ) * iArgcW );
	for ( i = 0; i < iArgcW; ++i ) {
		pArgvA[ i ] = _WideToAnsi( pArgvW[ i ] );
	}

	for ( i = 1; i < iArgcW; i++ ) {
		if ( isOption( pArgvA[ i ], OPT_HELP ) ) {
			bUsage = TRUE;
			continue;
		}

		if ( isOption( pArgvA[ i ], OPT_INPUT ) ) {
			if ( ( i + 1 ) < iArgcW ) {
				if ( g_pThis ) {
					bResult = g_pThis->SetBinFile( pArgvA[ ++i ] );
				}
			}
			continue;
		}

		if ( ( pArgvA[ i ][ 0 ] != '-' ) && ( pArgvA[ i ][ 0 ] != '/' ) ) {
			if ( g_pThis ) {
				bResult = g_pThis->SetBinFile( pArgvA[ i ] );
			}
			continue;
		}
	}

	if ( bUsage ) {
		Usage();
	}

	for ( i = 0; i < iArgcW; i++ ) {
		if ( pArgvA[ i ] )
			free( pArgvA[ i ] );
	}
	free( pArgvA );
	LocalFree( pArgvW );

	return bResult;
}

