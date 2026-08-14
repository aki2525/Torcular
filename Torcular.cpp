// Torcular.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "Torcular.h"

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

// Locals...
ATOM MyRegisterClass( HINSTANCE hInstance );
BOOL InitInstance( HINSTANCE hInstance, INT nCmdShow );
LRESULT CALLBACK WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK About( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL GetOption( VOID );

INT APIENTRY wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ INT nCmdShow )
{
MSG msg;
HACCEL hAccelTable;
HMODULE hRichEdit;
UNREFERENCED_PARAMETER( hPrevInstance );
UNREFERENCED_PARAMETER( pCmdLine );

	LoadString( hInstance, IDS_APP_TITLE, g_tszTitle, _countof( g_tszWindowClass ) );
	LoadString( hInstance, IDC_TORCULAR, g_tszWindowClass, _countof( g_tszWindowClass ) );
	MyRegisterClass( hInstance );

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
	case WM_CREATE:
		g_hwndView = CreateWindowExW( 0, MSFTEDIT_CLASS/*RICHEDIT_CLASS*/, L"", ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWnd, ( HMENU )IDC_VIEW, g_hInstance, nullptr);
		g_pThis = new CDisasm6801;
		if ( g_pThis )
			PostMessage( hWnd, WM_COMMAND, IDM_HERE_WE_GO, 0 );
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
			if ( bReady )
				g_pThis->DoDisasm();
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

VOID AddMessage( PTSTR ptStr )
{
CHARRANGE cr;

	if ( g_hwndView ) {
		//SendMessage( g_hwndView, WM_SETREDRAW, FALSE, 0 );
		cr.cpMin = -1;
		cr.cpMax = -1;
		SendMessage( g_hwndView, EM_EXSETSEL, 0, (LPARAM)&cr );
		SendMessage( g_hwndView, EM_REPLACESEL, FALSE, (LPARAM)ptStr );
		//SendMessage( g_hwndView, WM_SETREDRAW, TRUE, 0 );
		SendMessage( g_hwndView, EM_SCROLLCARET, 0, 0 );
		//InvalidateRect( g_hwndView, NULL, TRUE );
	}
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

