#include <fstream>
#include <iostream>
#include "gldebugger.h"
#include "gld_engine.h"
#include "gld_types.h"
#include "gld_drawing.h"

#include <gl\glf.h>

CRITICAL_SECTION gCS;

class gldController {
private:
	void createWindow();
	void shareContext(HGLRC shareGLRC, HDC shareDC);
public:
	int mainFont;
	HWND mainWindowHandle;
	HDC mainDC, origDC;
	HGLRC mainGLRC, origGLRC;
	void setContextOriginal() {
		if (origDC != NULL && origGLRC != NULL)
			wglMakeCurrent(origDC, origGLRC);
		LeaveCriticalSection(&gCS);
	};
	void setContextDebugger() {
		EnterCriticalSection(&gCS);
		wglMakeCurrent(mainDC, mainGLRC);
	};

	gldEngine engine;
	gldController() {};
	~gldController();
	void init(HGLRC shareGLRC, HDC shareDC);
	void step();
};

gldController controller;

/////////// SYSTEM PROCEDURES ///////////
DWORD WINAPI threadLoop(LPVOID lpParam) {
	bool quit = FALSE;
	MSG msg;
	while (!quit) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_CLOSE)
				quit = TRUE;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			controller.step();
		}
	}
	return (DWORD)0;
};
void enableOpenGL()
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	controller.mainDC = GetDC(controller.mainWindowHandle);
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(controller.mainDC, &pfd);
	SetPixelFormat(controller.mainDC, format, &pfd);

	controller.mainGLRC = wglCreateContext(controller.mainDC);
};
void disableOpenGL()
{
	wglMakeCurrent(NULL, NULL);
	//wglDeleteContext(hRC);
	ReleaseDC(controller.mainWindowHandle, controller.mainDC);
};
LRESULT CALLBACK gldWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		RECT rect;
		if (GetClientRect(hWnd, &rect)) {
			Box border;
			border.set((int)rect.left, (int)rect.top, (int)rect.right, (int)rect.bottom);
			canvas.setSize(border.width, border.height);
		}
		return 0;
	}

	case WM_CLOSE:
	{
		ShowWindow(controller.mainWindowHandle, SW_HIDE);
		controller.engine.saveConfig();
		//PostQuitMessage(0);
		return 0;
	}

	case WM_DESTROY:
	{
		//controller.engine.saveConfig();
		return 0;
	}

	case  WM_QUIT:
	{
		//controller.engine.saveConfig();
		return 0;
	}

	case WM_MOVE: 
	{
		RECT rect;
		if (GetWindowRect(hWnd, &rect)) { // get window rect
			int x = rect.left, y = rect.top;
			form.moveTo(x,y);
		}
		return 0;
	}
	case WM_SIZE:
	{
		RECT rect;
		Box border;
		if (GetWindowRect(hWnd, &rect)) { // get window rect
			border.set((int)rect.left, (int)rect.top, (int)rect.right, (int)rect.bottom);
			form.set(border);
		}
		if (GetClientRect(hWnd, &rect)) { // get canvas rect
			border.set((int)rect.left, (int)rect.top, (int)rect.right, (int)rect.bottom);
			canvas.setSize(border.width, border.height);
			controller.engine.onCanvasSizeChange(border.width, border.height);
		}
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		POINT mousePosition;
		if (GetCursorPos(&mousePosition)) {
			if (ScreenToClient(hWnd, &mousePosition)) {
				input.setMousePosition((int)mousePosition.x, (int)mousePosition.y);
				controller.engine.onMouseMove((int)mousePosition.x, (int)mousePosition.y);
			}
		}
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta > 0)
			controller.engine.onMouseWheel(1);
		else
			controller.engine.onMouseWheel(-1);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		controller.engine.onMouseDown(mbLeft);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		controller.engine.onMouseDown(mbMiddle);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		controller.engine.onMouseDown(mbRight);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		controller.engine.onMouseUp(mbLeft);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		controller.engine.onMouseUp(mbMiddle);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		controller.engine.onMouseUp(mbRight);
		return 0;
	}

	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
};


void gldController::init(HGLRC shareGLRC, HDC shareDC) {
	InitializeCriticalSection(&gCS);
	createWindow();
	shareContext(shareGLRC, shareDC);

	setContextDebugger();
	initFonts();
	engine.init();
	setContextOriginal();
};

void gldController::shareContext(HGLRC shareGLRC, HDC shareDC) {
	origDC = shareDC;
	if (origDC == NULL)
		origDC = wglGetCurrentDC();
	origGLRC = shareGLRC;
	if (origGLRC == NULL)
		origGLRC = wglGetCurrentContext();
	wglMakeCurrent(NULL, NULL);
	if (!wglShareLists(origGLRC, mainGLRC))
		std::cout << "Sharing Lists Failed! Error Code: " << GetLastError() << std::endl;
	setContextOriginal();
};

void gldController::createWindow() {

	WNDCLASS wc;

	wc.style = CS_OWNDC;
	wc.lpfnWndProc = gldWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"GLDebugger";
	RegisterClass(&wc);

	std::ifstream stream;
	stream.open("user.conf");
	if (stream.fail()) {
		form.moveTo(0, 0);
		form.setSize(500, 500);
	}
	else {
		form.load(stream);
		stream.close();
	}
	
	controller.mainWindowHandle = CreateWindow(
		L"GLDebugger", L"OpenGL Debugger Visualizer",
		WS_OVERLAPPEDWINDOW,
		form.left, form.top, form.width, form.height,
		NULL, NULL, NULL, NULL);
	enableOpenGL();
};

void gldController::step() {
	
	setContextDebugger();
	engine.render();
	SwapBuffers(controller.mainDC); // pozor pri spravovanom renderovani !!!
	setContextOriginal();
	Sleep(25);
	//MessageBox(0, L"Chcipni", L"MessageBox caption", MB_OK);
};

gldController::~gldController() {
	disableOpenGL();
	DestroyWindow(controller.mainWindowHandle);
};
  
//////////   INTERFACE   //////////
int gldInit(HGLRC glrcToShare, HDC dcToShare) {
	controller.init(glrcToShare, dcToShare);
	return 0;
};

void gldStart(){
	ShowWindow(controller.mainWindowHandle, SW_SHOW);
	LPDWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, threadLoop, NULL, 0, dwThreadID);
	CloseHandle(hThread);
};

void gldStop() {
	controller.engine.saveConfig();
	ShowWindow(controller.mainWindowHandle, SW_HIDE);
};

bool gldAddTexture(std::string caption, const GLuint texture) {
	controller.engine.addTexture(caption.c_str(), texture);
	return true;
};

bool gldAddLine(std::string format, void * data) {
	return gldAddLine(format, format, data);
};

bool gldAddLine(std::string format, void * data[]) {
	return gldAddLine(format, format, data);
};

bool gldAddLine(std::string caption, std::string format, void * data) {
	void * a_data[] = { data };
	controller.engine.addValues(caption.c_str(), format.c_str(), a_data);
	return true;
};

bool gldAddLine(std::string caption, std::string format, void * data[]) {
	controller.engine.addValues(caption.c_str(), format.c_str(), data);
	return true;
};

bool gldAddModel(std::string caption, unsigned count, GLuint vertices, GLuint indices) {
	controller.engine.addModel(caption.c_str(), count, vertices, indices);
	return true;
};

bool gldAddModelData(std::string caption, float * data, float min, float max) {
	controller.engine.addModelData(caption.c_str(), data, min, max);
	return true;
};

bool gldAddModelData(GLuint vboid, float * data, float min, float max) {
	controller.engine.addModelData(vboid, data, min, max);
	return true;
};


