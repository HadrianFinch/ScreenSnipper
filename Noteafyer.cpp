#ifndef UNICODE
#define UNICODE
#endif 

#include "precomp.h"

IWICImagingFactory* g_pWICFactory = nullptr;
HBITMAP g_hBitmap = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    DWORD dwCoInit = COINIT_APARTMENTTHREADED;
    HRESULT hr = CoInitializeEx(nullptr, dwCoInit);
    if (FAILED(hr))
    {
        return HRESULT_CODE(hr);
    }
    
    hr = CreateWICFactory(&g_pWICFactory);
    if (FAILED(hr))
    {
        return HRESULT_CODE(hr);
    }

    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
      return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return 0;
}

BOOL LayerWindow(
    _In_ HWND hwnd)
{
    // Add the WS_EX_LAYERED bit to the window ex style
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    SIZE windowSize = {657, 482};

    HDC hdcSrc = CreateCompatibleDC(NULL);
    HGDIOBJ hBmpSave = SelectObject(hdcSrc, g_hBitmap);

    POINT ptSrc = {0, 0};

    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    BOOL bRet = UpdateLayeredWindow(
        hwnd,
        NULL,
        NULL,
        &windowSize,
        hdcSrc,
        &ptSrc,
        NULL,
        &bf,
        ULW_ALPHA);
    SelectObject(hdcSrc, hBmpSave);
    ReleaseDC(NULL, hdcSrc);

    return bRet;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_CREATE:
        {
            // HBITMAP hBitmap = NULL;
            HRESULT hr = CreateBitmapFromFile(
                g_pWICFactory,
                L"images\\NoatifyerControlPanelWindow.png",
                &g_hBitmap);

            LayerWindow(hwnd);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    // case WM_PAINT:
    //     {
    //         PAINTSTRUCT ps;
    //         HDC hdc = BeginPaint(hwnd, &ps);

    //         HDC hdcSrc = CreateCompatibleDC(hdc);
    //         HGDIOBJ hBmpSave = SelectObject(hdcSrc, g_hBitmap);

    //         FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
    //         BitBlt(
    //             hdc,
    //             0,
    //             0,
    //             657,
    //             482,
    //             hdcSrc,
    //             // ps.rcPaint.left,
    //             // ps.rcPaint.top,
    //             0,
    //             0,
    //             SRCCOPY);

    //         SelectObject(hdcSrc, hBmpSave);
    //         ReleaseDC(NULL, hdcSrc);
    //         EndPaint(hwnd, &ps);
    //     }
    //     return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}