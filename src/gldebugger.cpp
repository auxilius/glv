#include <fstream>
#include <iostream>
#include "gldebugger.h"
#include "gld_engine.h"
#include "gld_types.h"
#include "gld_drawing.h"

#include <gl\glf.h>
#include <typeinfo> 

CRITICAL_SECTION gCS;

class gldController {
private:
	void createWindow();
	void shareContext(HGLRC shareGLRC, HDC shareDC);
	bool isInit;
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
	gldController() { isInit = false; };
	~gldController();
	void init(HGLRC shareGLRC, HDC shareDC);
	void step();
	bool initialized();
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
		return 0;
	}

	case WM_DESTROY:
	{
		return 0;
	}

	case  WM_QUIT:
	{
		return 0;
	}

	case WM_MOVE: 
	{
		RECT rect;
		if (GetWindowRect(hWnd, &rect)) { // get window rect
			int x = rect.left, y = rect.top;
			form.moveTo(x,y);
		}
		configLoader.save();
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
		configLoader.save();
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
	isInit = true;
};

bool gldController::initialized() {
	return isInit;
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

	if (!configLoader.load()) {
		form.moveTo(0, 0);
		form.setSize(500, 500);
	}
	
	controller.mainWindowHandle = CreateWindow(
		L"GLDebugger", L"GLD Visualizer",
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
	ShowWindow(controller.mainWindowHandle, SW_HIDE);
};

bool gldAddTexture(std::string caption, const GLuint texture) {
	controller.engine.visualizer.addTexture(caption.c_str(), texture);
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
	controller.engine.visualizer.addValues(caption.c_str(), format.c_str(), a_data);
	return true;
};

bool gldAddLine(std::string caption, std::string format, void * data[]) {
	controller.engine.visualizer.addValues(caption.c_str(), format.c_str(), data);
	return true;
};

template<class T>
void gldAddArray(std::string caption, T * data, std::string format, int length, int lineLength) {
	//cout << data << " " << &data << " " << ((float*)data)[0] << endl;
	//controller.engine.visualizer.addValArray(caption.c_str(), data, format.c_str(), length, lineLength);
	if (T == int)
		cout << "int";
	if (T == float)
		cout << "float";
};

bool gldAddModel(std::string caption, unsigned count, GLuint vertices, GLenum type) {
	controller.engine.visualizer.addModel(caption.c_str(), count, vertices, type);
	return true;
};

bool gldAddModelData(std::string caption, float * data, float min, float max) {
	controller.engine.visualizer.addModelData(caption.c_str(), data, min, max);
	return true;
};

bool gldAddModelEdges(std::string caption, GLenum mode, unsigned count, GLuint indices, GLenum type) {
	controller.engine.visualizer.addModelEdges(caption.c_str(), mode, count, indices, type);
	return true;
};

bool gldAddModelTexture(std::string caption, GLuint texture, GLuint coordinates, GLenum type) {
	controller.engine.visualizer.addModelTexture(caption.c_str(), texture, coordinates, type);
	return true;
};

bool gldSetDirectory(char * path) {
	std::string str_path = path;
	if (str_path[str_path.size() - 1] != '/')
		str_path += '/';
	if (controller.initialized()) {
		MessageBox(0, L"Calling gldSetDirectory after gldInit, need to be called befor, no changes will appear now.", L"Error changing path", MB_OK);
		return false;
	}
	if (!dirExists(str_path)) {
		MessageBox(0, L"Directory send by gldSetDirectory does not exist. Please select existing directory.", L"Error changing path", MB_OK);
	}
	SAVE_PATH = str_path;
	return true;
};