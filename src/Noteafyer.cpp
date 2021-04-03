#ifndef UNICODE
#define UNICODE
#endif 

#include "precomp.h"

HINSTANCE g_hInstance = NULL;
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
        WS_POPUPWINDOW | WS_CLIPCHILDREN,            // Window style
        0, 
        0, 
        657, 
        482,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        &wi1);
    if (hwnd == NULL)
    {
      return 0;
    }


    // SIZE size = {25, 25};
    // POINT point = {13, 13};
    // CButton* pCloseButton = nullptr;
    // CButton::Create(hwnd, 
    //                 L"Close Button", 
    //                 L"images\\NoteafyerControlPanelCloseButton.png", 
    //                 size, 
    //                 point, 
    //                 &pCloseButton);

    // size = {25, 25};
    // point = {35, 13};
    // CButton* pMinimizeButton = nullptr;
    // CButton::Create(hwnd,  
    //                 L"Close Button", 
    //                 L"images\\NoteafyerControlPanelMinimazeButton.png", 
    //                 size, 
    //                 point, 
    //                 &pMinimizeButton);

    // size = {284, 92};
    // point = {1627, 29};
    // CPopup* pPopupTest = nullptr;
    // CPopup::Create(L"Notification Test",
    //                L"images\\CalanderPopup.png",
    //                size,
    //                point,
    //                &pPopupTest);

    // size = {102, 24};
    // point = {160, 46};
    // CButton* pTestOpenZoomButton = nullptr;
    // CButton::Create(pPopupTest->m_hwnd,
    //                 L"Open Zoom", 
    //                 L"images\\OpenZoomButton.png", 
    //                 size, 
    //                 point, 
    //                 &pTestOpenZoomButton);

    // size = {284, 92};
    // point = {1627, 128};
    // CPopup* pSettingsPopup = nullptr;
    // CPopup::Create(L"Settings Notification",
    //                L"images\\SettingsPopup.png",
    //                size,
    //                point,
    //                &pSettingsPopup);

    // size = {83, 28};
    // point = {181, 44};
    // CButton* pOpenSettingsButton = nullptr;
    // CButton::Create(pSettingsPopup->m_hwnd,
    //                 L"Open Settings", 
    //                 L"images\\openButton.png", 
    //                 size, 
    //                 point, 
    //                 &pTestOpenZoomButton);
    
    // size = {83, 28};
    // point = {181, 8};
    // CButton* pDismissButton = nullptr;
    // CButton::Create(pSettingsPopup->m_hwnd,
    //                 L"Dismiss Button", 
    //                 L"images\\didmissButton.png", 
    //                 size, 
    //                 point, 
    //                 &pDismissButton);

    SIZE size = {482, 75};
    POINT pt = {719, 1050};
    CPopup* pMenuBar = nullptr;
    CPopup::Create(L"ScreenSnipper",
                   L"menuBarImages\\menuBar.png",
                   size,
                   pt,
                   &pMenuBar);
    
    pMenuBar->m_dragRect = {0, 0, 473, 62};

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
    pScreenCapture->m_HoverSize = {41, 34};

    pt = {82, 16};
    CButton* pWindowCapture = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Screen Capture", 
                    L"menuBarImages\\windowCapture.png", 
                    size, 
                    pt, 
                    &pWindowCapture);
    pWindowCapture->m_pHoverImageFileName = L"menuBarImages\\highlighted\\windowCaptureHighlight.png";
    pWindowCapture->m_HoverPt = {80, 14};
    pWindowCapture->m_HoverSize = {41, 34};

    pt = {132, 16};
    CButton* pZoneCapture = nullptr;
    CButton::Create(pMenuBar->m_hwnd,
                    L"Screen Capture", 
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
    CButton::Create(pMenuBar->m_hwnd,
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