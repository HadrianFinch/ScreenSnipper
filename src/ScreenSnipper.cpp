#ifndef UNICODE
#define UNICODE
#endif 

#include "precomp.h"

int dbg = 4;
/* 
    0  -- No debug
    1  -- Enables asserts
    2  -- Enables known error alerts
    3  -- ???????
    4  -- Enables debug popups
 */
HINSTANCE g_hInstance = NULL;
IWICImagingFactory* g_pWICFactory = nullptr;
bool popupActive = false;
HWND g_highlightHwnd;
HWND g_zoneSnipHwnd;
RECT g_snipRect;
bool g_zoneMouseDown = false;
bool snipHwndCreated = false;
bool g_zoneActive = false;
bool g_settingsOpen = false;

struct WINFO
{
    PWSTR pImageFile;
    SIZE size;
    POINT pt;
    /* data */
};

RECT NormalizeRect(RECT inputRect)
{
    RECT outputRect;

    outputRect.left = min(inputRect.left, inputRect.right);
    outputRect.top = min(inputRect.top, inputRect.bottom);
    outputRect.right = max(inputRect.left, inputRect.right);
    outputRect.bottom = max(inputRect.top, inputRect.bottom);

    return outputRect;
}

CPopup* ShowOptionsPopup(HWND menuBarHwnd)
{
    CPopup* pOptionsMenu = nullptr;

    if (!popupActive)
    {
        SIZE size = {90, 109};
        POINT pt = {349, -112};
        ClientToScreen(menuBarHwnd, &pt);
        HRESULT hr = CPopup::Create(L"Screen Snipper Options Menu",
                    OP_IMG_POPUP,
                    size,
                    pt,
                    &pOptionsMenu);
        if (SUCCEEDED(hr))
        {
            size = {81, 25};
            pt = {-1, -1};
            CButton* pToption = nullptr;
            CFavorite1RadioButton::Create(pOptionsMenu->m_hwnd,
                            L"Option 1", 
                            OP_IMG_TOP, 
                            size, 
                            pt, 
                            &pToption);
            pToption->m_pHoverImageFileName = OP_IMG_HVR_TOP;
            pToption->m_HoverPt = pt;
            pToption->m_HoverSize = size;

            pt = {-1, 23};
            CButton* pOption2 = nullptr;
            CButton::Create(pOptionsMenu->m_hwnd,
                            L"Option 2", 
                            OP_IMG_NORMAL, 
                            size, 
                            pt, 
                            &pOption2);
            pOption2->m_pHoverImageFileName = OP_IMG_HVR_NORMAL;
            pOption2->m_HoverPt = pt;
            pOption2->m_HoverSize = size;

            pt = {-1, 47};
            CButton* pOption3 = nullptr;
            CButton::Create(pOptionsMenu->m_hwnd,
                            L"Option 3", 
                            OP_IMG_NORMAL, 
                            size, 
                            pt, 
                            &pOption3);
            pOption3->m_pHoverImageFileName = OP_IMG_HVR_NORMAL;
            pOption3->m_HoverPt = pt;
            pOption3->m_HoverSize = size;

            pt = {-1, 71};
            CButton* pMoreOptions = nullptr;
            CMoreOptionsButton::Create(pOptionsMenu->m_hwnd,
                            L"More Options", 
                            OP_IMG_MOREOPTIONS, 
                            size, 
                            pt, 
                            &pMoreOptions);
            pMoreOptions->m_pHoverImageFileName = OP_IMG_HVR_MOREOPTIONS;
            pMoreOptions->m_HoverPt = pt;
            pMoreOptions->m_HoverSize = size;

            popupActive = true;
        }
        else
        {
            DestroyWindow(pOptionsMenu->m_hwnd);
        }
    }
    return pOptionsMenu;
}

void HideOptionsPopup(CPopup* pOptionsPopup)
{  
    if (pOptionsPopup != nullptr)
    {
        DestroyWindow(pOptionsPopup->m_hwnd);
        popupActive = false;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HighlightWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FilmWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;

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

    LoadFavoriteFolders();

    if (g_currentSnipPath == L"")
    {
        ExpandEnvironmentStrings(
            L"%userprofile%\\Desktop",
            g_currentSnipPath,
            _countof(g_currentSnipPath));
    }
    

    WNDCLASSW highlightWndClass = {};
    highlightWndClass.lpfnWndProc = HighlightWindowProc;
    highlightWndClass.hInstance = g_hInstance;
    highlightWndClass.lpszClassName = L"Window Capture Highlight";

    if (RegisterClass(&highlightWndClass))
    {
        g_highlightHwnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            highlightWndClass.lpszClassName,
            L"Window Capture Highlight",
            WS_POPUPWINDOW,
            0,
            0,
            100,
            100,
            NULL,
            NULL,
            g_hInstance,
            nullptr);
        
        SetLayeredWindowAttributes(
            g_highlightHwnd,
            NULL,
            45,
            LWA_ALPHA);
    }


    WNDCLASSW zoneSnipWndClass = {};
    zoneSnipWndClass.lpfnWndProc = FilmWindowProc;
    zoneSnipWndClass.hInstance = g_hInstance;
    zoneSnipWndClass.lpszClassName = L"Zone Capture Selector Film";

    if (RegisterClass(&zoneSnipWndClass))
    {
        g_zoneSnipHwnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW,
            zoneSnipWndClass.lpszClassName,
            L"Zone Capture Selector Film",
            WS_POPUPWINDOW,
            0,
            0,
            100,
            100,
            NULL,
            NULL,
            g_hInstance,
            nullptr);
        
        COLORREF colorref = 0x000000FF;
        SetLayeredWindowAttributes(
            g_zoneSnipHwnd,
            colorref,
            125,
            LWA_ALPHA | LWA_COLORKEY);
    }

    RECT desktopClientRect;
    HWND desktopHwnd = GetDesktopWindow();
    GetClientRect(desktopHwnd, &desktopClientRect);


    SIZE size = {482, 75};
    POINT pt = {(((desktopClientRect.right - desktopClientRect.left) / 2) - (size.cx / 2)), (desktopClientRect.bottom - desktopClientRect.top) - 125};
    CPopup* pMenuBar = nullptr;
    CWindowedTopmostPopup::Create(L"ScreenSnipper",
                   MB_IMG_MENUBAR,
                   size,
                   pt,
                   &pMenuBar);
    
    pMenuBar->m_dragRect = {0, 0, 473, 62};
    pMenuBar->m_masterPopup = true;

    RegisterHotKey(
        pMenuBar->m_hwnd,
        1,
        MOD_ALT | MOD_CONTROL | MOD_NOREPEAT,
        0x31 /* 1 key */);

    RegisterHotKey(
        pMenuBar->m_hwnd,
        2,
        MOD_ALT | MOD_CONTROL | MOD_NOREPEAT,
        0x32 /* 2 key */);

    RegisterHotKey(
        pMenuBar->m_hwnd,
        3,
        MOD_ALT | MOD_CONTROL | MOD_NOREPEAT,
        0x33 /* 3 key */);

    size = {13, 13};
    pt = {8, 8};
    CButton* pCloseButton = nullptr;
    CCloseButton::Create(pMenuBar->m_hwnd,
                    L"Close Button", 
                    MB_IMG_CLOSEBUTTON, 
                    size, 
                    pt, 
                    &pCloseButton);
    pCloseButton->m_pHoverImageFileName = MB_IMG_HVR_CLOSEBUTTON;
    pCloseButton->m_HoverPt = pt;
    pCloseButton->m_HoverSize = size;

    size = {38, 31};
    pt = {32, 16};
    CButton* pScreenCapture = nullptr;
    CScreenCaptureButton::Create(pMenuBar->m_hwnd,
                    L"Screen Capture", 
                    MB_IMG_SCREENCAPTURE, 
                    size, 
                    pt, 
                    &pScreenCapture);
    pScreenCapture->m_pHoverImageFileName = MB_IMG_HVR_SCREENCAPTURE;
    pScreenCapture->m_HoverPt = {30, 14};
    pScreenCapture->m_HoverSize = {42, 35};

    pt = {82, 16};
    CButton* pWindowCapture = nullptr;
    CMouseCaptureButton::Create(pMenuBar->m_hwnd,
                    L"Window Capture", 
                    MB_IMG_WINDOWCAPTURE, 
                    size, 
                    pt, 
                    &pWindowCapture);
    pWindowCapture->m_pHoverImageFileName = MB_IMG_HVR_WINDOWCAPTURE;
    pWindowCapture->m_HoverPt = {80, 14};
    pWindowCapture->m_HoverSize = {42, 35};

    pt = {132, 16};
    CButton* pZoneCapture = nullptr;
    CZoneCaptureButton::Create(pMenuBar->m_hwnd,
                    L"Zone Capture", 
                    MB_IMG_ZONECAPTURE, 
                    size, 
                    pt, 
                    &pZoneCapture);
    pZoneCapture->m_pHoverImageFileName = MB_IMG_HVR_ZONECAPTURE;
    pZoneCapture->m_HoverPt = {130, 14};
    pZoneCapture->m_HoverSize = {41, 34};

    size = {41, 34};
    pt = {195, 16};
    CButton* pScreenRecord = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Screen Record", 
                    MB_IMG_SCREENRECORD, 
                    size, 
                    pt, 
                    &pScreenRecord);
    pScreenRecord->m_pHoverImageFileName = MB_IMG_HVR_SCREENRECORD;
    pScreenRecord->m_HoverPt = {193, 14};
    pScreenRecord->m_HoverSize = {45, 38};

    pt = {245, 16};
    CButton* pWindowRecord = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Window Record", 
                    MB_IMG_WINDOWRECORD, 
                    size, 
                    pt, 
                    &pWindowRecord);
    pWindowRecord->m_pHoverImageFileName = MB_IMG_HVR_WINDOWRECORD;
    pWindowRecord->m_HoverPt = {243, 14};
    pWindowRecord->m_HoverSize = {45, 38};

    pt = {295, 16};
    CButton* pZoneRecord = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Zone Record", 
                    MB_IMG_ZONERECORD, 
                    size, 
                    pt, 
                    &pZoneRecord);
    pZoneRecord->m_pHoverImageFileName = MB_IMG_HVR_ZONERECORD;
    pZoneRecord->m_HoverPt = {293, 14};
    pZoneRecord->m_HoverSize = {45, 38};

    size = {63, 63};
    pt = {359, -1};
    CButton* pOptions = nullptr;
    COptionsButton::Create(pMenuBar->m_hwnd,
                    L"Options", 
                    MB_IMG_OPTIONS, 
                    size, 
                    pt, 
                    &pOptions);
    pOptions->m_pHoverImageFileName = MB_IMG_HVR_OPTIONS;
    pOptions->m_HoverPt = pt;
    pOptions->m_HoverSize = size;

    size = {58, 63};
    pt = {415, -1};
    CButton* pCapture = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Options", 
                    MB_IMG_CAPTURE, 
                    size, 
                    pt, 
                    &pCapture);
    pCapture->m_pHoverImageFileName = MB_IMG_HVR_CAPTURE;
    pCapture->m_HoverPt = pt;
    pCapture->m_HoverSize = size;
    
    // ShowWindow(hwnd, nCmdShow);
    

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

    POINT ptNull = {0,0};

    // HWND hwndParent = GetParent(hwnd);
    // if (hwndParent != NULL)
    // {
    //     RECT rcParent;
    //     GetWindowRect(hwndParent, &rcParent);
    //     rcParent.left += ptWinPos.x;
    //     rcParent.top += ptWinPos.y;

    //     ScreenToClient(hwndParent, reinterpret_cast<LPPOINT>(&rcParent));
    //     ptWinPos.x = rcParent.left;
    //     ptWinPos.y = rcParent.top;
    // }
      
    BOOL bRet = UpdateLayeredWindow(
        hwnd,
        NULL,
        &ptWinPos,
        &windowSize,
        hdcSrc,
        &ptNull,
        NULL,
        &bf,
        ULW_ALPHA);

    SelectObject(hdcSrc, hBmpSave);
    DeleteDC(hdcSrc);

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

            HRESULT hr = CreateBitmapFromResource(
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

        case WM_NCHITTEST:
        {
            DWORD coordintes = static_cast<DWORD>(lParam);
            POINT hitPos;
            hitPos.y = HIWORD(coordintes);
            hitPos.x = LOWORD(coordintes);

            RECT wndRect;
            GetWindowRect(hwnd, &wndRect);

            if (hitPos.y >= wndRect.top &&
                hitPos.y <= (wndRect.top + 39))
            {
                return HTCAPTION;
            }
        }
        return HTCLIENT;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK HighlightWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        const COLORREF rgbRed = 0x000000FF;
        HBRUSH blueBrush = CreateSolidBrush(rgbRed);

        // All painting occurs here, between BeginPaint and EndPaint.
        FillRect(hdc, &ps.rcPaint, blueBrush);
        EndPaint(hwnd, &ps);

        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK FilmWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT crDesktop;
            GetClientRect(GetDesktopWindow(), &crDesktop);
            
            COLORREF colorref = 0x00818181;
            HBRUSH brush = CreateSolidBrush(colorref);
            FillRect(hdc, &ps.rcPaint, brush);

            colorref = 0x000000FF;
            brush = CreateSolidBrush(colorref);
            FillRect(hdc, &g_snipRect, brush);

            EndPaint(hwnd, &ps);
        }
        return 0; 

        case WM_LBUTTONDOWN:
        {
            SetCapture(hwnd);
            g_zoneMouseDown = true;
            POINT clickPos;
            clickPos.x = GET_X_LPARAM(lParam);
            clickPos.y = GET_Y_LPARAM(lParam);

            g_snipRect.left = clickPos.x;
            g_snipRect.top = clickPos.y;
        }
        return 0;
        
        case WM_LBUTTONUP:
        {
            g_zoneMouseDown = false;
            g_zoneActive = false;

            ShowWindow(hwnd, SW_HIDE);

            CaptureZone(g_currentSnipPath, NormalizeRect(g_snipRect));
        }
        return 0;

        case WM_MOUSEMOVE:
        {
            if (g_zoneMouseDown)
            {
                POINT mousePos;
                mousePos.x = GET_X_LPARAM(lParam);
                mousePos.y = GET_Y_LPARAM(lParam);

                RECT oldSnipRect = g_snipRect;

                g_snipRect.right = mousePos.x;
                g_snipRect.bottom = mousePos.y;

                InvalidateRect(hwnd, &g_snipRect, 0);
                InvalidateRect(hwnd, &oldSnipRect, 0);
            }
        }
        return 0;
        
        case WM_ERASEBKGND:
        {
            
        }
        return 0;
    }
  
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}