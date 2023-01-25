#include "platform.hpp"
#include "events.hpp"

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/Xutil.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB     0x2092
#define GLX_CONTEXT_PROFILE_MASK_ARB      0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

static struct
{
    bool init = false;
    Display *dpy = nullptr;
    GLXContext ctx = nullptr;
    Window win = 0;
} X11;

// TODO
// - cleanup
// - check for extensions

static void dummySwapInterval(Display *dpy, GLXDrawable drawable, int interval) {}
static int translateKeyEvent(XKeyEvent *ke);
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
static glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
typedef void (*glXSwapIntervalEXTProc)(Display *dpy, GLXDrawable drawable, int interval);
static glXSwapIntervalEXTProc glXSwapIntervalEXT;

void Platform::_init()
{
    if (X11.init)
        return;

    X11.dpy = XOpenDisplay(NULL);
    if (!X11.dpy)
        die("X11: failed to open display");

    i32 scr = DefaultScreen(X11.dpy);
    bool ok;
    int maj = 0, min = 0;
    ok = glXQueryVersion(X11.dpy, &maj, &min);
    ok &= (maj > 1) || (maj == 1 && min >= 4);
    if (!ok)
        die("need GLX version >= 1.4");

    int fba[] = {
        GLX_X_RENDERABLE , 1,
        GLX_RENDER_TYPE  , GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_DOUBLEBUFFER , 1,
        GLX_RED_SIZE     , 8,
        GLX_GREEN_SIZE   , 8,
        GLX_BLUE_SIZE    , 8,
        GLX_ALPHA_SIZE   , 8,
        GLX_DEPTH_SIZE   , 24,
        GLX_STENCIL_SIZE , 8,
        None
    };

    int n;
    GLXFBConfig *fbcp = glXChooseFBConfig(X11.dpy, scr, fba, &n);
    if (!fbcp)
        die("GLX: no matching FB config found");
    GLXFBConfig fbc = fbcp[0];
    XFree(fbcp);

    XVisualInfo *vip = nullptr, vi;
    vip = glXGetVisualFromFBConfig(X11.dpy, fbc);
    vi  = *vip;
    XFree(vip);

    Window root = RootWindow(X11.dpy, vi.screen);
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap(X11.dpy, root, vi.visual, AllocNone);
    swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask | StructureNotifyMask;
    u32 cwm = CWColormap | CWEventMask;

    X11.win = XCreateWindow(X11.dpy, root, 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
            0, vi.depth, InputOutput, vi.visual, cwm, &swa);
    if (!X11.win)
        die("X11: could not create window");

    XTextProperty tp;
    char *t = (char*)PROJECT_TITLE;
    XStringListToTextProperty(&t, 1, &tp);
    XSetWMName(X11.dpy, X11.win, &tp);

    Atom WM_DELETE_WINDOW = XInternAtom(X11.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(X11.dpy, X11.win, &WM_DELETE_WINDOW, 1);

    glXSwapIntervalEXT = (glXSwapIntervalEXTProc)
        glXGetProcAddress((const GLubyte *)"glXSwapIntervalEXT");
    if (!glXSwapIntervalEXT)
        glXSwapIntervalEXT = dummySwapInterval;

    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
        glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
    if (!glXCreateContextAttribsARB)
        die("GLX: glXCreateContextAttribsARB not found");

    int ca[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, OPENGL_VERSION_MAJOR,
        GLX_CONTEXT_MINOR_VERSION_ARB, OPENGL_VERSION_MINOR,
        GLX_CONTEXT_PROFILE_MASK_ARB , GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None,
    };

    X11.ctx = glXCreateContextAttribsARB(X11.dpy, fbc, 0, True, ca);
    XSync(X11.dpy, False);
    if (!X11.ctx)
        die("GLX: could not create context");

    glXMakeCurrent(X11.dpy, X11.win, X11.ctx);

    XMapWindow(X11.dpy, X11.win);
    XSync(X11.dpy, False);
    X11.init = true;
}

void Platform::_destroy()
{
    if (X11.dpy) {
        if (X11.ctx) {
            glXMakeCurrent(X11.dpy, 0, 0);
            glXDestroyContext(X11.dpy, X11.ctx);
        }
        if (X11.win) XDestroyWindow(X11.dpy, X11.win);
        XCloseDisplay(X11.dpy);
    }
    X11.dpy = nullptr;
    X11.ctx = nullptr;
    X11.win = 0;
    X11.init = false;
}

void Platform::_pollEvents()
{
    ASSERT(X11.init, "X11 not initialized");
    XEvent xe;
    XPending(X11.dpy);

    while(QLength(X11.dpy)) {
        XNextEvent(X11.dpy, &xe);
        int key = 0;
        int btn = 0;

        switch(xe.type) {
            case KeyPress:
                key = translateKeyEvent(&xe.xkey);
                events.keyStates[key] = BITSET(events.keyStates[key], 0);
                break;
            case KeyRelease:
                key = translateKeyEvent(&xe.xkey);
                if (XEventsQueued(X11.dpy, QueuedAfterReading)) {
                    XEvent nxt;
                    XPeekEvent(X11.dpy, &nxt);
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
                if (events.window.w != xe.xconfigure.width || events.window.h != xe.xconfigure.height) {
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

void Platform::_swapBuffers()
{
    ASSERT(X11.init, "X11 not initialized");
    glXSwapBuffers(X11.dpy, X11.win);
}

void Platform::_swapInterval(i32 i)
{
    ASSERT(X11.init, "X11 not initialized");
    glXSwapIntervalEXT(X11.dpy, X11.win, i);
}

static int translateKeyEvent(XKeyEvent *ke)
{
    KeySym ks = XLookupKeysym(ke, 1);

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
