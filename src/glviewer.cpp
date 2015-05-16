#include <fstream>
#include <iostream>
#include <typeinfo> 
#include <direct.h>
#include "glviewer.h"
#include "glv_Engine.h"
#include "gld_types.h"
#include "gld_drawing.h"





CRITICAL_SECTION gCS;

class gldController {
private:
	void createWindow();
	void shareContext(HGLRC shareGLRC, HDC shareDC);
	bool isInit;
public:
	bool windowConfigChanged;
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

	Engine * engine;
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
	case WM_DESTROY: return 0;
	case WM_QUIT: return 0;

	case WM_MOVE: 
	{
		RECT rect;
		if (GetWindowRect(hWnd, &rect)) { // get window rect
			int x = rect.left, y = rect.top;
			form.moveTo(x,y);
		}
		if (controller.engine != NULL)
			controller.engine->fieldManager->save();
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
			if (controller.engine != NULL) 
				controller.engine->onCanvasSizeChange(border.width, border.height);
		}
		controller.windowConfigChanged = true;
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		POINT mousePosition;
		if (GetCursorPos(&mousePosition)) {
			if (ScreenToClient(hWnd, &mousePosition)) {
				input.setMousePosition((int)mousePosition.x, (int)mousePosition.y);
				if (controller.engine != NULL) 
					controller.engine->onMouseMove((int)mousePosition.x, (int)mousePosition.y);
			}
		}
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		if (controller.engine == NULL) 
			return 0;
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta > 0)
			controller.engine->onMouseWheel(1);
		else
			controller.engine->onMouseWheel(-1);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		if (controller.engine != NULL) 
			controller.engine->onMouseDown(mbLeft);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		if (controller.engine != NULL) 
			controller.engine->onMouseDown(mbMiddle);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		if (controller.engine != NULL) 
			controller.engine->onMouseDown(mbRight);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		if (controller.engine != NULL) 
			controller.engine->onMouseUp(mbLeft);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		if (controller.engine != NULL) 
			controller.engine->onMouseUp(mbMiddle);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		if (controller.engine != NULL) 
			controller.engine->onMouseUp(mbRight);
		return 0;
	}

	case WM_KEYDOWN:
	{
		char key = wParam;
		if (!input.shift && key >= 'A' && key <= 'Z')
			key = key - 'A' + 'a';
		if (controller.engine != NULL)
			controller.engine->onKeyDown(key);

		if (key == KEY_SHIFT)
			input.shift = true;
		return 0;
	}

	case WM_KEYUP:
	{
		char key = wParam;
		if (key == KEY_SHIFT)
			input.shift = false;
		return 0;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
};


void gldController::init(HGLRC shareGLRC, HDC shareDC) {
	InitializeCriticalSection(&gCS);
	
	createWindow();
	enableOpenGL();
	mainGLRC = wglCreateContext(mainDC);
	shareContext(shareGLRC, shareDC);
	
	setContextDebugger();
	initFonts();
	engine = new Engine();
	engine->init();
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
	wc.lpszClassName = "WINGLVIEW";
	RegisterClass(&wc);

	form.set(0, 0, 500);
	
	controller.mainWindowHandle = CreateWindow(
		"WINGLVIEW", "OpenGL View",
		WS_OVERLAPPEDWINDOW,
		form.left, form.top, form.width, form.height,
		NULL, NULL, NULL, NULL);
	windowHandle = mainWindowHandle;
	deviceContext = GetDC(mainWindowHandle);
};

void gldController::step() {
	POINT mousePosition;
	if (GetCursorPos(&mousePosition)) {
		if (ScreenToClient(mainWindowHandle, &mousePosition)) {
			input.setMousePosition((int)mousePosition.x, (int)mousePosition.y);
		}
	}
	setContextDebugger();
	engine->render();
	SwapBuffers(controller.mainDC); // pozor pri spravovanom renderovani !!!
	setContextOriginal();
	static int configSaveTimer = 0;
	if (windowConfigChanged && configSaveTimer >= 10000) {
		if (controller.engine != NULL)
			controller.engine->fieldManager->save();
		windowConfigChanged = false;
		configSaveTimer = 0;
	}
	Sleep(25);
};

gldController::~gldController() {
	disableOpenGL();
	DestroyWindow(controller.mainWindowHandle);
};
  
//////////   INTERFACE   //////////

bool setDirectory(std::string path) {
	if (!dirExists(path)) {
		MessageBox(0, ERRORTEXT_WORKDIR_DONT_EXIST, ERRORTEXT_HEADER, MB_OK | MB_ICONERROR);
		std::cout << "GLV ERROR: setting working path to '" << path << "' failed. Path does not exist. Please create the folder structure manualy." << std::endl;
	}
	else
		SAVE_PATH = path.c_str();
	return true;
};

int glv::init(std::string workingDir, HGLRC glrcToShare, HDC dcToShare) {
	if (controller.initialized())
		return 1;
	setDirectory(workingDir);
	glewInit();
	controller.init(glrcToShare, dcToShare);
	return 0;
};

void glv::show(){
	ShowWindow(controller.mainWindowHandle, SW_SHOW);
	LPDWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, threadLoop, NULL, 0, dwThreadID);
	CloseHandle(hThread);
};

void glv::hide() {
	ShowWindow(controller.mainWindowHandle, SW_HIDE);
};

bool glv::setTexture(std::string caption, const GLuint texture) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addTexture(caption.c_str(), texture);
		return true;
	}
	return false;
};

bool glv::setLine(std::string format, void * data) {
	return glv::setLine(format, format, data);
};

bool glv::setLine(std::string format, void * data[]) {
	return glv::setLine(format, format, data);
};

bool glv::setLine(std::string caption, std::string format, void * data) {
	void * a_data[] = { data };
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addValues(caption.c_str(), format.c_str(), a_data);
		return true;
	}
	return false;
};

bool glv::setLine(std::string caption, std::string format, void * data[]) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addValues(caption.c_str(), format.c_str(), data);
		return true;
	}
	return false;
};

template<class T>
void setArray(std::string caption, T * data, std::string format, int length, int lineLength) {
	if (T == int)
		cout << "int";
	if (T == float)
		cout << "float";
};

bool glv::setModel(std::string caption, unsigned count, GLuint vertices, bool rewrite, GLenum type) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModel(caption.c_str(), count, vertices, rewrite, type);
		return true;
	}
	return false;
};

bool glv::modelData(std::string caption, float * data, float min, float max, int colorMap ) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelData(caption.c_str(), data, min, max, colorMap);
		return true;
	}
	return false;
};

bool glv::modelColor(std::string caption, float * data, float min, float max, int colorMap ) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelColor(caption.c_str(), data, min, max, colorMap);
		return true;
	}
	return false;
};

bool glv::modelColorBuffer(std::string caption, GLuint colorBuffer ) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelColorBuffer(caption.c_str(), colorBuffer);
		return true;
	}
	return false;
};

bool glv::modelEdges(std::string caption, GLenum mode, unsigned count, GLuint indices, GLenum type) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelEdges(caption.c_str(), mode, count, indices, type);
		return true;
	}
	return false;
};

bool glv::modelTexture(std::string caption, GLuint texture, GLuint coordinates, GLenum type) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelTexture(caption.c_str(), texture, coordinates, type);
		return true;
	}
	return false;
};

bool glv::modelShaderProgram(std::string caption, GLuint shaderProgramId) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelShader(caption.c_str(), shaderProgramId);
		return true;
	}
	return false;
};

bool glv::modelVertexAttrib(std::string caption, GLuint atributeID, GLint size, GLenum type, GLuint buffer) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelVertexAttrib(caption.c_str(), atributeID, size, type, buffer);
		return true;
	}
	return false;
};

bool glv::modelNomals(std::string caption, GLuint normalBuffer) {
	if (controller.engine != NULL) {
		controller.engine->viewModeCtrl->addModelNormals(caption.c_str(), normalBuffer);
		return true;
	}
	return false;
};