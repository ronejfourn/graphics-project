#include "window/window.hpp"
#include "window/events.hpp"

#define GLX_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB     0x2092
#define GLX_CONTEXT_PROFILE_MASK_ARB      0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

#include <time.h>
namespace X11 {
    #include <X11/Xlib.h>
    #include <GL/glx.h>
    #include <X11/Xutil.h>

    static Display *dpy = nullptr;
    static GLXContext ctx = nullptr;
    static Window win = 0;
}

static void dummySwapInterval(X11::Display *dpy, X11::GLXDrawable drawable, int interval) { (void)dpy, (void)drawable, (void)interval; }
static int translateKeyEvent(X11::XKeyEvent *ke);
typedef X11::GLXContext (*glXCreateContextAttribsARBProc)(X11::Display*, X11::GLXFBConfig, X11::GLXContext, Bool, const int*);
static glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
typedef void (*glXSwapIntervalEXTProc)(X11::Display *dpy, X11::GLXDrawable drawable, int interval);
static glXSwapIntervalEXTProc glXSwapIntervalEXT;

void Window::_initialize(const Window::Config &cfg)
{
    using namespace X11;
    X11::dpy = XOpenDisplay(NULL);
    if (!X11::dpy)
        die("X11: failed to open display");

    i32 scr = DefaultScreen(X11::dpy);
    bool ok;
    int maj = 0, min = 0;
    ok = glXQueryVersion(X11::dpy, &maj, &min);
    ok &= (maj > 1) || (maj == 1 && min >= 4);
    if (!ok)
        die("need GLX version >= 1.4");

    int fba[] = {
        GLX_X_RENDERABLE , 1,
        GLX_RENDER_TYPE  , GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_DOUBLEBUFFER , cfg.doublebuffer,
        GLX_RED_SIZE     , cfg.redBits,
        GLX_GREEN_SIZE   , cfg.greenBits,
        GLX_BLUE_SIZE    , cfg.blueBits,
        GLX_ALPHA_SIZE   , cfg.alphaBits,
        GLX_DEPTH_SIZE   , cfg.depthBits,
        GLX_STENCIL_SIZE , cfg.stencilBits,
        None
    };

    int n;
    GLXFBConfig *fbcp = glXChooseFBConfig(X11::dpy, scr, fba, &n);
    if (!fbcp)
        die("GLX: no matching FB config found");
    GLXFBConfig fbc = fbcp[0];
    XFree(fbcp);

    XVisualInfo *vip = nullptr, vi;
    vip = glXGetVisualFromFBConfig(X11::dpy, fbc);
    vi  = *vip;
    XFree(vip);

    X11::Window root = RootWindow(X11::dpy, vi.screen);
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(X11::dpy, root, vi.visual, AllocNone);
    swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask | StructureNotifyMask;
    u32 cwm = CWColormap | CWEventMask;

    X11::win = XCreateWindow(X11::dpy, root, 0, 0, cfg.width, cfg.height,
            0, vi.depth, InputOutput, vi.visual, cwm, &swa);
    if (!X11::win)
        die("X11: could not create window");

    XTextProperty tp;
    char *t = (char*)cfg.title;
    XStringListToTextProperty(&t, 1, &tp);
    XSetWMName(X11::dpy, X11::win, &tp);

    Atom WM_DELETE_WINDOW = XInternAtom(X11::dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(X11::dpy, X11::win, &WM_DELETE_WINDOW, 1);

    glXSwapIntervalEXT = (glXSwapIntervalEXTProc)
        glXGetProcAddress((const GLubyte *)"glXSwapIntervalEXT");
    if (!glXSwapIntervalEXT)
        glXSwapIntervalEXT = dummySwapInterval;

    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
        glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
    if (!glXCreateContextAttribsARB)
        die("GLX: glXCreateContextAttribsARB not found");

    int ca[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, cfg.openglMajor,
        GLX_CONTEXT_MINOR_VERSION_ARB, cfg.openglMinor,
        GLX_CONTEXT_PROFILE_MASK_ARB , GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None,
    };

    X11::ctx = glXCreateContextAttribsARB(X11::dpy, fbc, 0, True, ca);
    XSync(X11::dpy, False);
    if (!X11::ctx)
        die("GLX: could not create context");

    glXMakeCurrent(X11::dpy, X11::win, X11::ctx);

    XMapWindow(X11::dpy, X11::win);
    XSync(X11::dpy, False);
}

void Window::_terminate()
{
    if (X11::dpy) {
        if (X11::ctx) {
            glXMakeCurrent(X11::dpy, 0, 0);
            glXDestroyContext(X11::dpy, X11::ctx);
        }
        if (X11::win) XDestroyWindow(X11::dpy, X11::win);
        XCloseDisplay(X11::dpy);
    }
    X11::dpy = nullptr;
    X11::ctx = nullptr;
    X11::win = 0;
}

void Window::_pollEvents()
{
    using namespace X11;
    XEvent xe;
    XPending(X11::dpy);

    while(QLength(X11::dpy)) {
        XNextEvent(X11::dpy, &xe);
        int key = 0;
        int btn = 0;

        switch(xe.type) {
            case KeyPress:
                key = translateKeyEvent(&xe.xkey);
                events.keyStates[key] = BITSET(events.keyStates[key], 0);
                break;
            case KeyRelease:
                key = translateKeyEvent(&xe.xkey);
                if (XEventsQueued(X11::dpy, QueuedAfterReading)) {
                    XEvent nxt;
                    XPeekEvent(X11::dpy, &nxt);
                    if (nxt.type == KeyPress && nxt.xkey.keycode == xe.xkey.keycode && nxt.xkey.window == xe.xkey.window)
                        if (nxt.xkey.time - xe.xkey.time < 20)
                            break;
                }
                events.keyStates[key] = BITRESET(events.keyStates[key], 0);
                break;
            case ButtonPress:
                if (xe.xbutton.button <= 3) {
                    btn = xe.xbutton.button - 1;
                    events.btnStates[btn] = BITSET(events.btnStates[btn], 0);
                } else if (xe.xbutton.button == 4) {
                        events.wheel =  1;
                } else if (xe.xbutton.button == 5) {
                        events.wheel = -1;
                }
                break;
            case ButtonRelease:
                if (xe.xbutton.button <= 3) {
                    btn = xe.xbutton.button - 1;
                    events.btnStates[btn] = BITRESET(events.btnStates[btn], 0);
                }
                break;
            case MotionNotify:
                events.cursor.x = xe.xmotion.x;
                events.cursor.y = events.window.h - xe.xmotion.y;
                break;
            case ConfigureNotify:
                if (events.window.w != (u32)xe.xconfigure.width || events.window.h != (u32)xe.xconfigure.height) {
                    events.window.resized = true;
                    events.window.w = xe.xconfigure.width;
                    events.window.h = xe.xconfigure.height;
                }
                break;
            case ClientMessage:
                events.quit = true;
                break;
        }
    }
}

void Window::_swapBuffers()
{
    glXSwapBuffers(X11::dpy, X11::win);
}

void Window::_swapInterval(i32 i)
{
    glXSwapIntervalEXT(X11::dpy, X11::win, i);
}

void Window::_sleep(u32 ms)
{
    i64 s = ms / 1000;
    i64 n = (ms - s * 1000) * 10e5;
    timespec ts = {s, n};
    nanosleep(&ts, nullptr);
}

static int translateKeyEvent(X11::XKeyEvent *ke)
{
    X11::KeySym ks = XLookupKeysym(ke, 1);

    switch (ks)
    {
        case XK_A: return KEY_A; case XK_B: return KEY_B;
        case XK_C: return KEY_C; case XK_D: return KEY_D;
        case XK_E: return KEY_E; case XK_F: return KEY_F;
        case XK_G: return KEY_G; case XK_H: return KEY_H;
        case XK_I: return KEY_I; case XK_J: return KEY_J;
        case XK_K: return KEY_K; case XK_L: return KEY_L;
        case XK_M: return KEY_M; case XK_N: return KEY_N;
        case XK_O: return KEY_O; case XK_P: return KEY_P;
        case XK_Q: return KEY_Q; case XK_R: return KEY_R;
        case XK_S: return KEY_S; case XK_T: return KEY_T;
        case XK_U: return KEY_U; case XK_V: return KEY_V;
        case XK_W: return KEY_W; case XK_X: return KEY_X;
        case XK_Y: return KEY_Y; case XK_Z: return KEY_Z;
        case XK_0: return KEY_0; case XK_1: return KEY_1;
        case XK_2: return KEY_2; case XK_3: return KEY_3;
        case XK_4: return KEY_4; case XK_5: return KEY_5;
        case XK_6: return KEY_6; case XK_7: return KEY_7;
        case XK_8: return KEY_8; case XK_9: return KEY_9;
        case XK_Up  : return KEY_UP  ; case XK_Down : return KEY_DOWN ;
        case XK_Left: return KEY_LEFT; case XK_Right: return KEY_RIGHT;
        case XK_Return: return KEY_RETURN;
        case XK_Escape: return KEY_ESCAPE;
        default: return KEY_UNKNOWN;
    }
}
