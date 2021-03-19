#ifndef UNICODE
#define UNICODE
#endif 

#include "precomp.h"

IWICImagingFactory* g_pWICFactory = nullptr;

struct WINFO
{
    PWSTR pImageFile;
    SIZE size;
    POINT pt;
    /* data */
};


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
    WINFO wi1;
    wi1.pImageFile = L"images\\NoatifyerControlPanelWindow.png";
    wi1.pt = {0, 0};
    wi1.size = {657, 482};

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPED | WS_CLIPCHILDREN,            // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        &wi1);
    if (hwnd == NULL)
    {
      return 0;
    }


    // close button window
    const wchar_t CLASS_NAME_BTN[]  = L"button";
    
    WNDCLASS btn = { };

    btn.lpfnWndProc   = WindowProc;
    btn.hInstance     = hInstance;
    btn.lpszClassName = CLASS_NAME_BTN;

    RegisterClass(&btn);

    // Create the window.
    WINFO wi2;
    wi2.pImageFile = L"images\\NoteafyerControlPanelCloseButton.png";
    wi2.pt = {13, 13};
    wi2.size = {31, 31};

    HWND closeBtnHwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME_BTN,                     // Window class
        L"close button",                // Window text
        WS_CHILD,            // Window style
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT,
        hwnd,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        &wi2);
    if (closeBtnHwnd == NULL)
    {
      return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    ShowWindow(closeBtnHwnd, SW_SHOW);

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
    _In_ HWND hwnd,
    _In_ HBITMAP hBitmap,
    _In_ SIZE windowSize,
    _In_ POINT ptWinPos)
{
    // Add the WS_EX_LAYERED bit to the window ex style
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    HDC hdcSrc = CreateCompatibleDC(NULL);
    HGDIOBJ hBmpSave = SelectObject(hdcSrc, hBitmap);

    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    POINT pt = {0,0};

    BOOL bRet = UpdateLayeredWindow(
        hwnd,
        NULL,
        &ptWinPos,
        &windowSize,
        hdcSrc,
        &pt,
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
            // get the right image from CREATSTRUCT
            CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
            WINFO* pwi = reinterpret_cast<WINFO*>(pCS->lpCreateParams);
            
            HBITMAP hBitmap = NULL;

            HRESULT hr = CreateBitmapFromFile(
                g_pWICFactory,
                pwi->pImageFile,
                &hBitmap);

            LayerWindow(hwnd, hBitmap, pwi->size, pwi->pt);
        }
        return 0;

        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        return 0;

        // case WM_WINDOWPOSCHANGING:
        // {
        //     WINDOWPOS* pPos = reinterpret_cast<WINDOWPOS*>(lParam);

        //     pPos->flags |= SWP_NOSIZE;
        // }
        // return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}