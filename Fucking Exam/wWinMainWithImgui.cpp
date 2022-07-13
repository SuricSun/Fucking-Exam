// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "../ThirdParty/imgui-1.88/imgui.h"
#include "../ThirdParty/imgui-1.88/imgui_internal.h"
#include "../ThirdParty/imgui-1.88/imgui_impl_win32.h"
#include "../ThirdParty/imgui-1.88/imgui_impl_dx11.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11")

#include"FileParser.h"
#include"UTFStringAffair.h"
#include<vector>

using namespace std;
using namespace FuckingExam;

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static INT hideFlag = SW_SHOW;
static INT wnd_alpha = 255;
static INT textInputBufferSize = 1 * 1024 * 1024;
static char* textInputBuffer = new char[textInputBufferSize];
static HGLOBAL clip_board_data_handle = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyBoardHook(int nCode, WPARAM wParam, LPARAM lParam);
void CopyToClipBoard(char* p_utf8_data);

/*
* Wrapper functions of ImGui
*/
//size��xy�������0, is_filled�٣�������rectֻ��Ⱦ�߿򣬴�ʱthickness��������
const ImWchar* GetMyGlyphRange();
void DummyRectWidget(ImGuiID id, ImVec2 in_size, bool is_filled = true, float thickness = 1.0f, ImU32 color = IM_COL32(128, 128, 255, 255), float rounding = ImGui::GetStyle().FrameRounding);

// MACROS
#define U8(arg) (char*)u8 ## arg
#define SayAndExit(arg) MessageBoxW(nullptr,arg,L"����",MB_OK);return -1;

// Main code
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, INT nCmdShow) {

	// Init Global Vars
	ZeroMemory(textInputBuffer, textInputBufferSize);

	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Fucking Exam";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.
	HWND hwnd = CreateWindowEx(
		WS_EX_LAYERED,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Fucking Exam (Canary Version)",                // Window text
		WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME,// Window style
		// Size and position
		32, 32, 512, 512,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL) {
		SayAndExit(L"Err CreateWindowEx mainWnd");
	}

	if (SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE) == false) {
		SayAndExit(L"Err SetWindowPos");
	}

	if (SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA) == false) {
		SayAndExit(L"Err SetLayeredWindowAttributes");
	}

	if (SetWindowsHookExW(WH_KEYBOARD_LL, KeyBoardHook, NULL, NULL) == NULL) {
		SayAndExit(L"Err SetWindowsHookExW");
	}

	// * ��ʼ��ImGui

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		SayAndExit(L"Err CreateDeviceD3D");
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF(U8("C:\\Windows\\Fonts\\SIMYOU.TTF"), 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	ImFont* p_font = io.Fonts->AddFontFromFileTTF(U8("C:\\Windows\\Fonts\\msyh.ttc"), 18, NULL, GetMyGlyphRange());
	//IM_ASSERT(font != NULL);

	/*
	* App State
	*/
	RECT win32_wnd_rect = {};
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	boolean exam_file_selected = false;

	int file_copy_str_length = 1024 * 1024;
	char* p_file_copy_str = new char[file_copy_str_length];
	ZeroMemory(p_file_copy_str, file_copy_str_length);

	u8string file_parse_err_str;

	int file_path_length = 1024;
	char* p_file_path = new char[file_path_length];
	ZeroMemory(p_file_path, file_path_length);

	vector<Block> block_vec;
	/*
	* End App State
	*/

	// * ����ImGuiȫ����ʽ
	ImGuiStyle style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.14);

	// Main loop
	bool shouldContinue = true;
	while (shouldContinue) {
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				shouldContinue = false;
		}
		if (shouldContinue == false)
			break;

		if (SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE) == false) {
			SayAndExit(L"Err SetWindowPos");
		}
		if (SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), wnd_alpha, LWA_ALPHA) == false) {
			SayAndExit(L"Err SetLayeredWindowAttributes");
		}
		ShowWindow(hwnd, hideFlag);

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (exam_file_selected) {
			// 1. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{
				ImGui::SetNextWindowPos(ImVec2(0, 0));
				GetClientRect(hwnd, &win32_wnd_rect);
				ImGui::SetNextWindowSize(ImVec2(win32_wnd_rect.right, win32_wnd_rect.bottom));

				// Create a window
				ImGui::Begin(U8("MainWnd"), nullptr,
							 ImGuiWindowFlags_NoMove |
							 ImGuiWindowFlags_NoResize |
							 ImGuiWindowFlags_NoTitleBar);

				if (ImGui::Button(U8("������һҳ"))) {
					exam_file_selected = false;
				}
				ImGui::TextColored(ImVec4(1, 0, 0, 1), U8("֡�� (%.3f FPS) / ֡ʱ�� (%.3f ms)"), ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
				ImGui::SliderInt(U8("����͸����"), &wnd_alpha, 32, 255);

				ImGui::Spacing();

				//ѭ����

				auto it = block_vec.begin();
				ImGuiID id = 1; //id���ܴ�0��ʼ
				ImGui::PushID(u8"Markdown����");
				while (it != block_vec.end()) {
					Block& b = deref(it);
					//��Ⱦ
					if (b.blockType == Block::Part) {
						//��Ⱦ����
						ImGui::GetWindowDrawList()->AddRectFilled(
							ImVec2(b.x0, b.y0),
							ImVec2(ImGui::GetWindowContentRegionMax().x, b.y1),
							IM_COL32(255, 69, 0, 255),
							ImGui::GetStyle().FrameRounding
						);
						//��Ⱦ����
						ImGui::TextWrapped((char*)b.blockName.c_str());
						//��ȡ�����Χ��
						ImVec2 min = ImGui::GetItemRectMin();
						ImVec2 max = ImGui::GetItemRectMax();
						b.x0 = min.x;
						b.y0 = min.y;
						b.x1 = max.x;
						b.y1 = max.y;
						//��Ⱦ����
						if (b.content.length() > 0) {
							ImGui::TextWrapped((char*)b.content.c_str());
						}
					} else if (b.blockType == Block::Question) {
						//��ȾBullet��
						DummyRectWidget(id, ImVec2(b.x_len, b.y_len));
						//���������Ͱ����ݸ��Ƶ����а�
						if (ImGui::IsItemClicked()) {
							CopyToClipBoard((char*)b.content.c_str());
						}
						//ͬ��
						ImGui::SameLine();
						//��Ⱦ����
						ImGui::TextWrapped((char*)b.blockName.c_str());
						//��ȡ����߶�
						ImVec2 min = ImGui::GetItemRectMin();
						ImVec2 max = ImGui::GetItemRectMax();
						b.y_len = max.y - min.y;
						//��Ⱦ����
						if (b.content.length() > 0) {
							ImGui::TextWrapped((char*)b.content.c_str());
						}
					}
					it++;
					id++;
				}
				ImGui::PopID();

				//ImDrawList* p_dl = ImGui::GetWindowDrawList();
				////p_dl->AddRectFilled(ImVec2(ImGui::GetCurrentWindow()->WindowPadding.x, min.y), ImVec2(20, max.y), IM_COL32(128, 128, 255, 255), 3.14);

				//DummyRectWidget(ImVec2(20, 20));
				//DummyRectWidget(ImVec2(20, 20), false);

				//ImGui::SameLine();
				//ImGui::TextWrapped(U8("˭��GUI������˭��GUI������˭��GUI������˭��GUI������˭��GUI������"));
				//p_dl->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(128, 128, 255, 255), 3.14);
				//ImGui::TextWrapped(U8("����ô���᣿����ô���᣿����ô���᣿����ô���᣿����ô���᣿"));
				//p_dl->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(128, 128, 255, 255), 3.14);
				//ImGui::TextWrapped(U8("����ô���᣿����ô���᣿����ô���᣿����ô���᣿����ô���᣿"));
				//p_dl->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(128, 128, 255, 255), 3.14);
				////INT wrap_width = ImGui::GetWindowWidth() - 20;
				////ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
				////ImGui::Text(u8"����ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ������ɵ��");
				////ImGui::PopTextWrapPos();

				ImGui::End();
			}
		} else {
			{
				ImGui::SetNextWindowPos(ImVec2(0, 0));
				GetClientRect(hwnd, &win32_wnd_rect);
				ImGui::SetNextWindowSize(ImVec2(win32_wnd_rect.right, win32_wnd_rect.bottom));

				// Create a window called "Hello, world!" and append into it.
				ImGui::Begin(U8("##FileSelectWnd"), nullptr,
							 ImGuiWindowFlags_NoMove |
							 ImGuiWindowFlags_NoResize |
							 ImGuiWindowFlags_NoTitleBar);

				if (ImGui::BeginTabBar("SelectTabBar")) {
					if (ImGui::BeginTabItem(U8("ֱ�Ӹ����ı�"))) {
						ImGui::Text(U8("�����ı����·��ı���"));
						ImGui::SameLine();
						if (ImGui::Button(U8("����"))) {
							//����ַ��������Ƿ�ϸ�
							i32 bufferSize = Util::UTFStringAffair::UTF8StrLen((char8_t*)p_file_copy_str, 0);
							//��ʼ����
							FileParser fp = FileParser();
							if (fp.parseFile(p_file_copy_str, bufferSize, &block_vec) < 0) {
								file_parse_err_str = fp.errInfo;
								ImGui::OpenPopup(U8("����"));
								ImGui::SetNextWindowPos(ImVec2(0, 0));
								ImGui::SetNextWindowSize(ImVec2(300, 150));
							} else {
								exam_file_selected = true;
							}
						}
						//Modal
						if (ImGui::BeginPopupModal(U8("����"), NULL, ImGuiWindowFlags_NoResize)) {
							ImGui::TextWrapped((char*)file_parse_err_str.c_str());
							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();
							if (ImGui::Button(U8("ȷ��"), ImVec2(-1, 0))) {
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}
						// *  �ı����ƿ�
						ImGui::InputTextMultiline(U8("##"), p_file_copy_str, file_copy_str_length, ImVec2(-1, -1));
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem(U8("���ļ��д�"))) {
						ImGui::Text(U8("�������ı�·��"));
						// *  �ļ�ѡ�����
						ImGui::InputTextEx(U8("##"), U8("�����ı�·��"), p_file_path, file_path_length, ImVec2(-1, 0), 0);
						if (ImGui::Button(U8("��"))) {
							//���Խ����ı������ʧ�ܾͱ���(modal)

							//��ת������
							u8string from = u8string((char8_t*)p_file_path);
							u16string to = u16string();
							Util::UTFStringAffair::UTF8To16(from, to);

							//��ʼ����
							FileParser fp = FileParser();
							if (fp.parseFile((WCHAR*)to.c_str(), &block_vec) < 0) {
								file_parse_err_str = fp.errInfo;
								ImGui::OpenPopup(U8("����"));
								ImGui::SetNextWindowPos(ImVec2(0, 0));
								ImGui::SetNextWindowSize(ImVec2(300, 150));
							} else {
								exam_file_selected = true;
							}
						}
						//Modal
						if (ImGui::BeginPopupModal(U8("����"), NULL, ImGuiWindowFlags_NoResize)) {
							ImGui::TextWrapped((char*)file_parse_err_str.c_str());
							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();
							if (ImGui::Button(U8("ȷ��"), ImVec2(-1, 0))) {
								ImGui::CloseCurrentPopup();
							}
							ImGui::EndPopup();
						}
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}

				ImGui::End();
			}
		}


		//ImGui::ShowDemoWindow();

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0,};
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D() {
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget() {
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK KeyBoardHook(int nCode, WPARAM wParam, LPARAM lParam) {

	/*
	* If code is less than zero,
	* the hook procedure must pass the message to the CallNextHookEx function without further processing
	* and should return the value returned by CallNextHookEx.
	*/
	if (nCode >= 0) {
		if (GetAsyncKeyState(0x32) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000) {
			if (hideFlag == SW_SHOW) {
				hideFlag = SW_HIDE;
			} else {
				hideFlag = SW_SHOW;
			}
		}
	}

	return CallNextHookEx(HC_ACTION, nCode, wParam, lParam);
}

void CopyToClipBoard(char* p_utf8_data) {

	// * �������а�����
	//ת��
	u8string from = (char8_t*)p_utf8_data;
	u16string to;
	Util::UTFStringAffair::UTF8To16(from, to);

	clip_board_data_handle = GlobalAlloc(GPTR, to.length() * sizeof(char16_t) + sizeof(char16_t));
	if (clip_board_data_handle != NULL) {
		char* p_data = (char*)GlobalLock(clip_board_data_handle);
		if (p_data != nullptr) {
			CopyMemory(p_data, to.c_str(), to.length() * sizeof(char16_t) + sizeof(char16_t));
			if (GlobalUnlock(clip_board_data_handle) == 0) {
				if (GetLastError() != NO_ERROR) {
					//����
					return;
				}
			}
			//�򿪼����壨ӵ���ߴ���ΪNULL��
			if (OpenClipboard(NULL)) {
				EmptyClipboard(); //���
				SetClipboardData(CF_UNICODETEXT, clip_board_data_handle);
				CloseClipboard(); //�رռ�����
			}
		}
	}

	//GlobalFree(clip_board_data_handle);
}

const ImWchar* GetMyGlyphRange() {

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x2000, 0x206F, // General Punctuation
		0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
		0xFFFD, 0xFFFD, // Invalid
		0x4e00, 0x9FAF, // CJK Ideograms
		0x2460, 0x2473, // Add by SCY, support circled arab number 1 to 20
		0,
	};
	return &ranges[0];
}

void DummyRectWidget(ImGuiID id, ImVec2 in_size, bool is_filled, float thickness, ImU32 color, float rounding) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (window->SkipItems) {
		return;
	}

	ImVec2 new_size = ImGui::CalcItemSize(in_size, 0.0f, 0.0f);
	const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + new_size.x, window->DC.CursorPos.y + new_size.y));
	ImGui::ItemSize(new_size);
	ImGui::ItemAdd(bb, id);

	ImGui::ButtonBehavior(bb, id, nullptr, nullptr);

	u8 r, g, b, a;
	r = color >> IM_COL32_R_SHIFT;
	g = color >> IM_COL32_G_SHIFT;
	b = color >> IM_COL32_B_SHIFT;
	a = color >> IM_COL32_A_SHIFT;
	//��ȡrgb���ֵ
	ImU32 maxx = max(r, g);
	maxx = max(maxx, b);
	//����״̬������ɫ
	//IsItemActive���ȼ���
	if (ImGui::IsItemActive()) {
		//��������Ϊԭ����0.75
		r = 0.6f * (float)r;
		g = 0.6f * (float)g;
		b = 0.6f * (float)b;
	} else if (ImGui::IsItemHovered()) {
		//��������Ϊԭ����0.9
		r = 0.8f * (float)r;
		g = 0.8f * (float)g;
		b = 0.8f * (float)b;
	}

	//����Color
	color = IM_COL32((ImU32)r, (ImU32)g, (ImU32)b, (ImU32)a);

	//��Ⱦ
	if (is_filled) {
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), color, rounding, 0);
	} else {
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), color, rounding, 0, thickness);
	}
}