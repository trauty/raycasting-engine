#include "platform.h"

#if PLATFORM_WIN

#include <windows.h>
#include <winuser.h>
#include <libloaderapi.h>

#include "framebuffer.h"

static HWND hwnd = NULL;

static Framebuffer frame_buffer = {0};

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = NULL;
static HDC frame_device_ctx = NULL;

static OnWindowDestroyCB destroy_callback;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
        case WM_QUIT:
        case WM_DESTROY:
            destroy_callback();
            break;

        case WM_PAINT:
        {
            static PAINTSTRUCT paint;
            static HDC device_ctx;
            device_ctx = BeginPaint(hwnd, &paint);
            BitBlt(
                device_ctx,
                paint.rcPaint.left, paint.rcPaint.top,
                paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                frame_device_ctx,
                paint.rcPaint.left, paint.rcPaint.top,
                SRCCOPY
            );
            EndPaint(hwnd, &paint);
        }
        break;

        case WM_SIZE:
        {
            frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
            frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

            if (frame_bitmap)
            {
                DeleteObject(frame_bitmap);
            }
            frame_bitmap = CreateDIBSection(
                NULL, &frame_bitmap_info,
                DIB_RGB_COLORS, (void **)&frame_buffer.pixels, 0, 0
            );

            SelectObject(frame_device_ctx, frame_bitmap);

            frame_buffer.width = LOWORD(lParam);
            frame_buffer.height = HIWORD(lParam);
        }
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

Framebuffer* create_window()
{
    char class_name[] = "window_win32";
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = class_name;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(
            NULL,
            "Window registration failed!",
            "ERROR!",
            MB_ICONEXCLAMATION | MB_OK);

        return NULL;
    }

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_ctx = CreateCompatibleDC(0);

    hwnd = CreateWindowEx(
        0,
        class_name,
        "raycasting-engine",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1280,
        720,
        NULL,
        NULL,
        GetModuleHandle(0),
        NULL);

    if (hwnd == NULL)
    {
        MessageBox(
            NULL,
            "Create window failed!",
            "ERROR!",
            MB_ICONEXCLAMATION | MB_OK);

        return NULL;
    }

    ShowWindow(hwnd, SW_SHOW);

    return &frame_buffer;
}

void update_window()
{
    MSG msg = {0};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        DispatchMessage(&msg);
    }

    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);
}

void register_window_destroy_cb(OnWindowDestroyCB on_destroy_cb)
{
    destroy_callback = on_destroy_cb;
}

#endif