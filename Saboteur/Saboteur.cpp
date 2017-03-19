#include "stdafx.h"
#include "saboteur.h"

#include <string>
#include <iostream>
#include <sstream>
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND mainGameWindowInitialize(HINSTANCE hInstance);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	HWND                hWnd;
	MSG                 msg;
	//WNDCLASS            mainWnd;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	hWnd = mainGameWindowInitialize(hInstance);
	Saboteur::game.attachWindow(hWnd);

	/*mainWnd.style = CS_HREDRAW | CS_VREDRAW;
	mainWnd.lpfnWndProc = WndProc;
	mainWnd.cbClsExtra = 0;
	mainWnd.cbWndExtra = 0;
	mainWnd.hInstance = hInstance;
	mainWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	mainWnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	mainWnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	mainWnd.lpszMenuName = NULL;
	mainWnd.lpszClassName = TEXT("Main window class");

	RegisterClass(&mainWnd);

	hWnd = CreateWindow(
		TEXT("Main window class"),   // window class name
		TEXT("Sabouter"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style

								  //CW_USEDEFAULT,       
		500, 300,                 // initial position
		800, 500,				  // initial size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters
	Saboteur::game.attachWindow(hWnd);

	UnregisterClass(TEXT("Main window class"), hInstance);*/

	
	//Saboteur::game.start();
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_CREATE:
		
		//if (hWnd == Saboteur::game.gameWindow)
			Saboteur::game.start();

		break;


	case WM_PAINT:

		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		Saboteur::game.draw(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		wchar_t msg[32];
		swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", wParam);
		Saboteur::game.processKey(wParam);
		OutputDebugString(msg);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}
}

HWND mainGameWindowInitialize(HINSTANCE hInstance) {

	WNDCLASS            mainWnd;

	mainWnd.style = CS_HREDRAW | CS_VREDRAW;
	mainWnd.lpfnWndProc = WndProc;
	mainWnd.cbClsExtra = 0;
	mainWnd.cbWndExtra = 0;
	mainWnd.hInstance = hInstance;
	mainWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	mainWnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	mainWnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	mainWnd.lpszMenuName = NULL;
	mainWnd.lpszClassName = TEXT("Main window class");

	RegisterClass(&mainWnd);

	HWND hWnd = CreateWindow(
		TEXT("Main window class"),   // window class name
		TEXT("Sabouter"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style

								  //CW_USEDEFAULT,       
		500, 300,                 // initial position
		800, 500,				  // initial size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters
	Saboteur::game.attachWindow(hWnd);

	UnregisterClass(TEXT("Main window class"), hInstance);

	return hWnd;
}