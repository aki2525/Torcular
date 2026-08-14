// Torcular.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "Torcular.h"

// Globals...
HINSTANCE g_hInstance;
TCHAR g_tszTitle[ MAX_PATH ];
TCHAR g_tszWindowClass[ MAX_PATH ];

// Locals...
ATOM MyRegisterClass( HINSTANCE hInstance );
BOOL InitInstance( HINSTANCE hInstance, INT nCmdShow );
LRESULT CALLBACK WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK About( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

INT APIENTRY wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ INT nCmdShow )
{
MSG msg;
HACCEL hAccelTable;
UNREFERENCED_PARAMETER( hPrevInstance );
UNREFERENCED_PARAMETER( pCmdLine );

	LoadString( hInstance, IDS_APP_TITLE, g_tszTitle, _countof( g_tszWindowClass ) );
	LoadString( hInstance, IDC_TORCULAR, g_tszWindowClass, _countof( g_tszWindowClass ) );
	MyRegisterClass( hInstance );

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
	case WM_COMMAND: {
	INT wmId;
		wmId = LOWORD( wParam );
		switch( wmId ) {
		case IDM_ABOUT:
			DialogBox( g_hInstance, MAKEINTRESOURCE( IDD_ABOUTBOX ), hWnd, About );
			break;
		case IDM_EXIT:
			DestroyWindow( hWnd );
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

