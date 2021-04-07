#ifndef UNICODE
#define UNICODE
#endif 

#include "precomp.h"

HINSTANCE g_hInstance = NULL;
IWICImagingFactory* g_pWICFactory = nullptr;
CPopup* pOptionsMenu = nullptr;
bool popupCreated = false;
bool popupActive = false;
HWND g_highlightHwnd;
HWND g_zoneSnipHwnd;

struct WINFO
{
    PWSTR pImageFile;
    SIZE size;
    POINT pt;
    /* data */
};

HWND GetTopLevelWindowFromPoint(POINT pt)
{
    HWND hwnd = WindowFromPoint(pt);

    HWND hwndTopLevel = GetAncestor(hwnd, GA_ROOT);
    HWND hwndShell = GetShellWindow();

    if ((hwndTopLevel != GetDesktopWindow()) &&
        (hwndTopLevel != hwndShell))
    {
        return hwndTopLevel;
    }

    return NULL;

/* 
    if (GetParent(inputHwnd) == NULL)
    {
        return inputHwnd;
    }
    else
    {
        HWND outputHwnd = inputHwnd;
        while (GetParent(outputHwnd) != NULL)
        {
            outputHwnd = GetParent(outputHwnd);
        }
        Assert(outputHwnd != desktopWindow);
        return outputHwnd;
    }     */
}

void ShowOptionsPopup(HWND menuBarHwnd)
{
    if (!popupActive)
    {
        if (!popupCreated)
        {
            SIZE size = {90, 109};
            POINT pt = {349, -112};
            ClientToScreen(menuBarHwnd, &pt);
            HRESULT hr = CPopup::Create(L"Screen Snipper Options Menu",
                        L"menuBarImages\\optionsMenu.png",
                        size,
                        pt,
                        &pOptionsMenu);
            if (SUCCEEDED(hr))
            {
                size = {81, 25};
                pt = {-1, 0};
                CButton* pTopOption = nullptr;
                CButton::Create(pOptionsMenu->m_hwnd,
                                L"Option 1", 
                                L"menuBarImages\\optionsMenu\\optionsButtonTop.png", 
                                size, 
                                pt, 
                                &pTopOption);
                pTopOption->m_pHoverImageFileName = L"menuBarImages\\optionsMenu\\highlighted\\optionsButtonTopHighlight.png";
                pTopOption->m_HoverPt = pt;
                pTopOption->m_HoverSize = size;

                pt = {-1, 24};
                CButton* pOption2 = nullptr;
                CButton::Create(pOptionsMenu->m_hwnd,
                                L"Option 2", 
                                L"menuBarImages\\optionsMenu\\optionsButton.png", 
                                size, 
                                pt, 
                                &pOption2);
                pOption2->m_pHoverImageFileName = L"menuBarImages\\optionsMenu\\highlighted\\optionsButtonHighlight.png";
                pOption2->m_HoverPt = pt;
                pOption2->m_HoverSize = size;

                pt = {-1, 48};
                CButton* pOption3 = nullptr;
                CButton::Create(pOptionsMenu->m_hwnd,
                                L"Option 3", 
                                L"menuBarImages\\optionsMenu\\optionsButton.png", 
                                size, 
                                pt, 
                                &pOption3);
                pOption3->m_pHoverImageFileName = L"menuBarImages\\optionsMenu\\highlighted\\optionsButtonHighlight.png";
                pOption3->m_HoverPt = pt;
                pOption3->m_HoverSize = size;

                pt = {-1, 72};
                CButton* pMoreOptions = nullptr;
                CButton::Create(pOptionsMenu->m_hwnd,
                                L"More Options", 
                                L"menuBarImages\\optionsMenu\\moreOptions.png", 
                                size, 
                                pt, 
                                &pMoreOptions);
                pMoreOptions->m_pHoverImageFileName = L"menuBarImages\\optionsMenu\\highlighted\\moreOptionsHighlight.png";
                pMoreOptions->m_HoverPt = pt;
                pMoreOptions->m_HoverSize = size;

                popupActive = true;
                popupCreated = true;
            }
        }
        else
        {
            SIZE size = {90, 109};
            POINT pt = {349, -112};
            ClientToScreen(menuBarHwnd, &pt);
            SetWindowPos(pOptionsMenu->m_hwnd,
                            NULL,
                            pt.x,
                            pt.y,
                            size.cx,
                            size.cy,
                            NULL);

            ShowWindow(pOptionsMenu->m_hwnd, SW_SHOW);
            popupActive = true;
        }
    }
}

void HideOptionsPopup(HWND menuBarHwnd)
{  
    if (pOptionsMenu != nullptr)
    {
        ShowWindow(pOptionsMenu->m_hwnd, SW_HIDE);
        popupActive = false;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HighlightWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
    zoneSnipWndClass.lpfnWndProc = DefWindowProc;
    zoneSnipWndClass.hInstance = g_hInstance;
    zoneSnipWndClass.lpszClassName = L"Zone Capture Selector Film";

    if (RegisterClass(&highlightWndClass))
    {
        g_highlightHwnd = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
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
        
        SetLayeredWindowAttributes(
            g_highlightHwnd,
            NULL,
            45,
            LWA_ALPHA);
    }

    RECT desktopClientRect;
    HWND desktopHwnd = GetDesktopWindow();
    GetClientRect(desktopHwnd, &desktopClientRect);


    SIZE size = {482, 75};
    POINT pt = {(((desktopClientRect.right - desktopClientRect.left) / 2) - (size.cx / 2)), (desktopClientRect.bottom - desktopClientRect.top) - 125};
    CPopup* pMenuBar = nullptr;
    CPopup::Create(L"ScreenSnipper",
                   L"menuBarImages\\menuBar.png",
                   size,
                   pt,
                   &pMenuBar);
    
    pMenuBar->m_dragRect = {0, 0, 473, 62};
    pMenuBar->m_masterPopup = true;
    

    size = {13, 13};
    pt = {8, 8};
    CButton* pCloseButton = nullptr;
    CCloseButton::Create(pMenuBar->m_hwnd,
                    L"Close Button", 
                    L"menuBarImages\\closeButton.png", 
                    size, 
                    pt, 
                    &pCloseButton);
    pCloseButton->m_pHoverImageFileName = L"menuBarImages\\highlighted\\closeButtonHighlight.png";
    pCloseButton->m_HoverPt = pt;
    pCloseButton->m_HoverSize = size;

    size = {38, 31};
    pt = {32, 16};
    CButton* pScreenCapture = nullptr;
    CScreenCaptureButton::Create(pMenuBar->m_hwnd,
                    L"Screen Capture", 
                    L"menuBarImages\\screenCapture.png", 
                    size, 
                    pt, 
                    &pScreenCapture);
    pScreenCapture->m_pHoverImageFileName = L"menuBarImages\\highlighted\\screenCaptureHighlight.png";
    pScreenCapture->m_HoverPt = {30, 14};
    pScreenCapture->m_HoverSize = {42, 35};
    pScreenCapture->m_pMouseDownImageFileName = L"menuBarImages\\clicked\\screenCaptureClick.png";
    pScreenCapture->m_MouseDownPt = {30, 14};
    pScreenCapture->m_MouseDownSize = {42, 34};

    pt = {82, 16};
    CButton* pWindowCapture = nullptr;
    CMouseCaptureButton::Create(pMenuBar->m_hwnd,
                    L"Window Capture", 
                    L"menuBarImages\\windowCapture.png", 
                    size, 
                    pt, 
                    &pWindowCapture);
    pWindowCapture->m_pHoverImageFileName = L"menuBarImages\\highlighted\\windowCaptureHighlight.png";
    pWindowCapture->m_HoverPt = {80, 14};
    pWindowCapture->m_HoverSize = {42, 35};

    pt = {132, 16};
    CButton* pZoneCapture = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Zone Capture", 
                    L"menuBarImages\\zoneCapture.png", 
                    size, 
                    pt, 
                    &pZoneCapture);
    pZoneCapture->m_pHoverImageFileName = L"menuBarImages\\highlighted\\zoneCaptureHighlight.png";
    pZoneCapture->m_HoverPt = {130, 14};
    pZoneCapture->m_HoverSize = {41, 34};

    size = {41, 34};
    pt = {195, 16};
    CButton* pScreenRecord = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Screen Record", 
                    L"menuBarImages\\screenRecord.png", 
                    size, 
                    pt, 
                    &pScreenRecord);
    pScreenRecord->m_pHoverImageFileName = L"menuBarImages\\highlighted\\screenRecordHighlight.png";
    pScreenRecord->m_HoverPt = {193, 14};
    pScreenRecord->m_HoverSize = {45, 38};

    pt = {245, 16};
    CButton* pWindowRecord = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Window Record", 
                    L"menuBarImages\\windowRecord.png", 
                    size, 
                    pt, 
                    &pWindowRecord);
    pWindowRecord->m_pHoverImageFileName = L"menuBarImages\\highlighted\\windowRecordHighlighted.png";
    pWindowRecord->m_HoverPt = {243, 14};
    pWindowRecord->m_HoverSize = {45, 38};

    pt = {295, 16};
    CButton* pZoneRecord = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Zone Record", 
                    L"menuBarImages\\zoneRecord.png", 
                    size, 
                    pt, 
                    &pZoneRecord);
    pZoneRecord->m_pHoverImageFileName = L"menuBarImages\\highlighted\\zoneRecordHighlight.png";
    pZoneRecord->m_HoverPt = {293, 14};
    pZoneRecord->m_HoverSize = {45, 38};

    size = {63, 63};
    pt = {359, -1};
    CButton* pOptions = nullptr;
    COptionsButton::Create(pMenuBar->m_hwnd,
                    L"Options", 
                    L"menuBarImages\\options.png", 
                    size, 
                    pt, 
                    &pOptions);
    pOptions->m_pHoverImageFileName = L"menuBarImages\\highlighted\\optionsHighlight.png";
    pOptions->m_HoverPt = pt;
    pOptions->m_HoverSize = size;

    size = {58, 63};
    pt = {415, -1};
    CButton* pCapture = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Options", 
                    L"menuBarImages\\capture.png", 
                    size, 
                    pt, 
                    &pCapture);
    pCapture->m_pHoverImageFileName = L"menuBarImages\\highlighted\\captureHighlight.png";
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