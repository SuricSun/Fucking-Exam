#include<Windows.h>

#define SayAndExit(arg) MessageBoxW(nullptr,arg,L"¾¯¸æ",MB_OK);return -1;

HWND mainWnd = 0;
HWND editBox = 0;
INT hideFlag = SW_SHOW;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyBoardHook(int nCode, WPARAM wParam, LPARAM lParam);

INT WINAPI wWinMain1(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, INT nCmdShow) {

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Fucking Exam";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.
	HWND mainWnd = CreateWindowEx(
		WS_EX_LAYERED,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Fucking Exam",                // Window text
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,// Window style
		// Size and position
		0, 0, 512, 512,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (mainWnd == NULL) {
		SayAndExit(L"Err CreateWindowEx mainWnd");
	}

	editBox = CreateWindowEx(
		0,
		L"EDIT",   // predefined class 
		L"ÊäÈëÄÚÈÝ",         // no window title 
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		0, 0, 0, 0,   // set size in WM_SIZE message 
		mainWnd,         // parent window 
		NULL,
		hInstance,
		NULL          // pointer not needed 
	);        

	if (editBox == NULL) {
		SayAndExit(L"Err CreateWindowEx editBox");
	}

	if (SetWindowPos(mainWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE) == false) {
		SayAndExit(L"Err SetWindowPos");
	}

	if (SetLayeredWindowAttributes(mainWnd, RGB(0, 0, 0), 32, LWA_ALPHA) == false) {
		SayAndExit(L"Err SetLayeredWindowAttributes");
	}

	if (SetWindowsHookExW(WH_KEYBOARD_LL, KeyBoardHook, NULL, NULL) == NULL) {
		SayAndExit(L"Err SetWindowsHookExW");
	}

	MSG msg = { };

	while (true) {
		if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) == 0) {

		} else {
			if (msg.message == WM_QUIT) {

				return 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (SetWindowPos(mainWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE) == false) {
			SayAndExit(L"Err SetWindowPos");
		}
		ShowWindow(mainWnd, hideFlag);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_SIZE:
	{
		RECT rect = {};
		GetClientRect(hwnd, &rect);
		SetWindowPos(editBox, 0, 0, 0, rect.right, rect.bottom, 0);
		break;
	}
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	}
	
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyBoardHook(int nCode, WPARAM wParam, LPARAM lParam) {
	
	/*
	* If code is less than zero, 
	* the hook procedure must pass the message to the CallNextHookEx function without further processing 
	* and should return the value returned by CallNextHookEx.
	*/
	if (nCode >= 0) {
		if (GetAsyncKeyState(0x32) & 0x8000) {
			if (hideFlag == SW_SHOW) {
				hideFlag = SW_HIDE;
			} else {
				hideFlag = SW_SHOW;
			}
		}
	}

	return CallNextHookEx(HC_ACTION, nCode, wParam, lParam);
}