#include "framework.h"

HWND window = 0;
HDC device = 0;
HGLRC context = 0;
int active = 1;

#define get_mouse_buttons(wparam, buttons)          \
    if(wparam & MK_LBUTTON) buttons |= MBUTTON0;    \
    if(wparam & MK_RBUTTON) buttons |= MBUTTON1;    \
    if(wparam & MK_MBUTTON) buttons |= MBUTTON2;    \
    if(wparam & MK_XBUTTON1) buttons |= MBUTTON3;   \
    if(wparam & MK_XBUTTON2) buttons |= MBUTTON4;

static LRESULT CALLBACK
WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    unsigned char buttons = 0;

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_ACTIVATE:
            break;

    case WM_PAINT:
        ValidateRect(hWnd, NULL);
        break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        onKeyPress(wParam & 0xff);
        break;

    case WM_SYSKEYUP:
    case WM_KEYUP:
        onKeyRelease(wParam & 0xff);
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:

        get_mouse_buttons(wParam, buttons);

        onMousePress(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), buttons);
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:

        get_mouse_buttons(wParam, buttons);

        onMouseRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), buttons);
        break;

    case WM_MOUSEMOVE:
        get_mouse_buttons(wParam, buttons);

        onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), buttons);
        break;

    case WM_CLOSE:
        onQuit();
        break;

    case WM_SYSCOMMAND:
        switch (wParam & 0xfff0)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
        case SC_KEYMENU:
            return 0;
        }
        break;    
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

extern int
openDisplay(const char *title, int width, int height, int fullscreen, int cursor)
{
    HINSTANCE instance = GetModuleHandle(0);

    WNDCLASSEX windowClass;
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = &WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = instance;
    windowClass.hIcon = 0;
    windowClass.hCursor = LoadCursor(NULL,IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = title;
    windowClass.hIconSm = NULL;

    UnregisterClass(title, instance);

    if (!RegisterClassEx(&windowClass))
        return 0;

    // determine window style

    int style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

    if (fullscreen)
        style = WS_POPUP;

    // create window

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    AdjustWindowRect(&rect, style, 0);
    rect.right -= rect.left;
    rect.bottom -= rect.top;

    int x = 0;
    int y = 0;

    if (!fullscreen)
    {
        x = (GetSystemMetrics(SM_CXSCREEN) - rect.right) >> 1;
        y = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) >> 1;
    }

    window = CreateWindow( title, title, style, x, y, rect.right, rect.bottom, NULL, NULL, instance, NULL );

    if (!window)
        return 0;

    // change display mode

    if (fullscreen)
    {
        DEVMODE mode;
        memset(&mode, 0, sizeof(mode));
        mode.dmSize = sizeof(mode);
        mode.dmPelsWidth = width;
        mode.dmPelsHeight = height;
        mode.dmBitsPerPel = 32;
        mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (ChangeDisplaySettings(&mode, CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
            return 0;

        ShowCursor(0);
    }

    // initialize wgl

    PIXELFORMATDESCRIPTOR descriptor;
    memset(&descriptor, 0, sizeof(descriptor));
    descriptor.nVersion = 1;
    descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    descriptor.iPixelType = PFD_TYPE_RGBA;
    descriptor.cColorBits = 32;
    descriptor.cDepthBits = 16;
    descriptor.cStencilBits = 8;
    descriptor.iLayerType = PFD_MAIN_PLANE;

    device = GetDC(window);
    if (!device)
        return 0;

    GLuint format = ChoosePixelFormat(device, &descriptor);
    if (!format)
        return 0;

    if (!SetPixelFormat(device, format, &descriptor))
        return 0;

    context = wglCreateContext(device);
    if (!context)
        return 0;

    if (!wglMakeCurrent(device, context))
        return 0;

    // show window

    ShowWindow(window, SW_NORMAL);
    ShowCursor(cursor);

    return 1;
}

extern void
updateDisplay()
{
    // process window messages

    MSG message;

    while (1)
    {
        if (active)
        {
            if (!PeekMessage(&message, window, 0, 0, PM_REMOVE)) 
                break;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            if (!GetMessage(&message, window, 0, 0)) 
                break;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    // show rendering

    SwapBuffers(device);
}

extern void
closeDisplay()
{	
    ReleaseDC(window, device);
    device = 0;

    DestroyWindow(window);
    window = 0;
}

extern void
setTitle(const char *title)
{
    SetWindowText(window, title);
}
