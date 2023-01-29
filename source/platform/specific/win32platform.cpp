#include "platform/platform.hpp"
#include "platform/events.hpp"

#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

static struct
{
    bool init = false;
    HWND hwnd = nullptr;
    HGLRC ctx = nullptr;
    HDC hdc   = nullptr;
} WIN;

#define _WC(M) L ## M
#define WC(M) _WC(M)

static LRESULT wndproc(HWND, UINT, WPARAM, LPARAM);
static int translateKey(WPARAM ks);
static BOOL dummySwapinterval(int i) { return FALSE; }

typedef HGLRC (WINAPI * wglCreateContextAttribsARBProc) (HDC, HGLRC, const int *);
typedef BOOL (WINAPI * wglSwapIntervalEXTProc) (int interval);
static wglCreateContextAttribsARBProc wglCreateContextAttribsARB;
static wglSwapIntervalEXTProc wglSwapIntervalEXT = dummySwapinterval;

void Platform::_init()
{
    if (WIN.init)
        return;

    SetProcessDPIAware();

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    LPCWSTR name = WC(PROJECT_TITLE);

    WNDCLASSW wc = {};
    wc.hInstance = hInstance;
    wc.lpfnWndProc = wndproc;
    wc.lpszClassName = name;
    wc.hCursor = LoadCursorW(hInstance, IDC_ARROW);
    wc.style = CS_OWNDC;
    if (!RegisterClassW(&wc))
        die("WIN32: failed to register class");

    RECT rect = {0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    events.window.w = rect.right;
    events.window.h = rect.bottom;

    WIN.hwnd = CreateWindowExW(0, name, name,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right, rect.bottom, nullptr, FALSE, hInstance, nullptr);
    if (!WIN.hwnd)
        die("WIN32: failed to create window");

    WIN.hdc = GetDC(WIN.hwnd);
    if (!WIN.hdc)
        die("WIN32: failed to get DC");

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pxf = ChoosePixelFormat(WIN.hdc, &pfd);
    if (!pxf)
        die("WIN32: failed to choose pixel format");
    if (!SetPixelFormat(WIN.hdc, pxf, &pfd))
        die("WIN32: failed to set pixel format");

    auto tmp = wglCreateContext(WIN.hdc);
    if (!tmp)
        die("WGL: failed to create context");
    wglMakeCurrent(WIN.hdc, tmp);
    wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc)
        wglGetProcAddress("wglCreateContextAttribsARB");
    wglMakeCurrent(WIN.hdc, nullptr);
    wglDeleteContext(tmp);

    if (!wglCreateContextAttribsARB)
        die("WGL: wglCreateContextAttribsARB not found");

    int ca[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, OPENGL_VERSION_MAJOR,
        WGL_CONTEXT_MINOR_VERSION_ARB, OPENGL_VERSION_MINOR,
        WGL_CONTEXT_PROFILE_MASK_ARB , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    WIN.ctx = wglCreateContextAttribsARB(WIN.hdc, nullptr, ca);
    if (!WIN.ctx)
        die("WGL: failed to create context");
    wglMakeCurrent(WIN.hdc, WIN.ctx);

    wglSwapIntervalEXT = (wglSwapIntervalEXTProc)
        wglGetProcAddress("wglSwapIntervalEXT");
    if (!wglSwapIntervalEXT)
        wglSwapIntervalEXT = dummySwapinterval;

    SetPropW(WIN.hwnd, L"BG", &events);

    UpdateWindow(WIN.hwnd);
    ShowWindow(WIN.hwnd, SW_SHOW);

    WIN.init = true;
}

void Platform::_destroy()
{
    if (WIN.ctx) {
        wglMakeCurrent(WIN.hdc, nullptr);
        wglDeleteContext(WIN.ctx);
    }
    if (WIN.hdc) ReleaseDC(WIN.hwnd, WIN.hdc);
    if (WIN.hwnd) DestroyWindow(WIN.hwnd);
    WIN.hwnd  = nullptr;
    WIN.hdc   = nullptr;
    WIN.ctx   = nullptr;
    WIN.init  = false;
}

void Platform::_pollEvents()
{
    ASSERT(WIN.init, "WIN32 not initialized");
    MSG msg = {0};
	while (PeekMessageW(&msg, WIN.hwnd, 0, 0, PM_REMOVE))
        DispatchMessageW(&msg);
}

void Platform::_swapBuffers()
{
    ASSERT(WIN.init, "WIN32 not initialized");
    SwapBuffers(WIN.hdc);
}

void Platform::_swapInterval(i32 i)
{
    wglSwapIntervalEXT(i);
}

LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    i32 key;
	Events *ev = (Events *)GetPropW(hwnd, L"BG");
    switch (msg) {
		case WM_CLOSE:
		case WM_QUIT:
			ev->quit = true;
			return FALSE;
		case WM_SIZE:
			ev->window.resized = true;
			ev->window.w = LOWORD(lParam);
			ev->window.h = HIWORD(lParam);
			return FALSE;
		case WM_LBUTTONDOWN:
			ev->btnStates[BUTTON_LEFT] = BITSET(ev->btnStates[BUTTON_LEFT], 0);
			return FALSE;
		case WM_MBUTTONDOWN:
			ev->btnStates[BUTTON_MIDDLE] = BITSET(ev->btnStates[BUTTON_MIDDLE], 0);
			return FALSE;
		case WM_RBUTTONDOWN:
			ev->btnStates[BUTTON_RIGHT] = BITSET(ev->btnStates[BUTTON_RIGHT], 0);
			return FALSE;
		case WM_LBUTTONUP:
			ev->btnStates[BUTTON_LEFT] = BITRESET(ev->btnStates[BUTTON_LEFT], 0);
			return FALSE;
		case WM_MBUTTONUP:
			ev->btnStates[BUTTON_MIDDLE] = BITRESET(ev->btnStates[BUTTON_MIDDLE], 0);
			return FALSE;
		case WM_RBUTTONUP:
			ev->btnStates[BUTTON_RIGHT] = BITRESET(ev->btnStates[BUTTON_RIGHT], 0);
			return FALSE;
		case WM_KEYDOWN:
			key = translateKey(wParam);
			ev->keyStates[key] = BITSET(ev->keyStates[key], 0);
			return FALSE;
		case WM_KEYUP:
			key = translateKey(wParam);
			ev->keyStates[key] = BITRESET(ev->keyStates[key], 0);
			return FALSE;
		case WM_MOUSEMOVE:
			ev->cursor.x = (int)(short)LOWORD(lParam);
			ev->cursor.y = (int)(short)HIWORD(lParam);
			ev->cursor.y = ev->window.h - ev->cursor.y;
			return FALSE;
		case WM_MOUSEWHEEL:
			ev->wheel = HIWORD(wParam);
			return FALSE;
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(NULL, IDC_ARROW));
            return TRUE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int translateKey(WPARAM ks)
{
    switch (ks)
    {
        case 'A': return KEY_A; case 'B': return KEY_B;
        case 'C': return KEY_C; case 'D': return KEY_D;
        case 'E': return KEY_E; case 'F': return KEY_F;
        case 'G': return KEY_G; case 'H': return KEY_H;
        case 'I': return KEY_I; case 'J': return KEY_J;
        case 'K': return KEY_K; case 'L': return KEY_L;
        case 'M': return KEY_M; case 'N': return KEY_N;
        case 'O': return KEY_O; case 'P': return KEY_P;
        case 'Q': return KEY_Q; case 'R': return KEY_R;
        case 'S': return KEY_S; case 'T': return KEY_T;
        case 'U': return KEY_U; case 'V': return KEY_V;
        case 'W': return KEY_W; case 'X': return KEY_X;
        case 'Y': return KEY_Y; case 'Z': return KEY_Z;
        case '0': return KEY_0; case '1': return KEY_1;
        case '2': return KEY_2; case '3': return KEY_3;
        case '4': return KEY_4; case '5': return KEY_5;
        case '6': return KEY_6; case '7': return KEY_7;
        case '8': return KEY_8; case '9': return KEY_9;
        case VK_UP  : return KEY_UP  ; case VK_DOWN : return KEY_DOWN ;
        case VK_LEFT: return KEY_LEFT; case VK_RIGHT: return KEY_RIGHT;
        case VK_RETURN: return KEY_RETURN;
        case VK_ESCAPE: return KEY_ESCAPE;
        default: return KEY_UNKNOWN;
    }
}
