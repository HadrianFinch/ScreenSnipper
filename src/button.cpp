#include "precomp.h"

bool CButton::s_classRegistered = false;

/*static*/
HRESULT CButton::Create(
    _In_ HWND hwndParent,
    _In_ PCWSTR pWindowName,
    _In_ int pImageFile,
    _In_ SIZE size,
    _In_ POINT pt,
    _Outptr_ CButton** ppButton)
{
    HRESULT hr = S_OK;
    CButton* pButton = new CButton(pImageFile, size, pt);
    if (pButton != nullptr)
    {
        hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

/*static*/
bool CButton::EnsureWndClass()
{
    if (!s_classRegistered)
    {
        WNDCLASSW wndClass = {};

        wndClass.lpfnWndProc = CButton::s_WindowProc;
        wndClass.hInstance = g_hInstance;
        wndClass.lpszClassName = c_szLayeredButtonClassName;

        if (RegisterClass(&wndClass))
        {
            s_classRegistered = true;
        }
    }

    return s_classRegistered;
}

/*static*/
LRESULT CALLBACK CButton::s_WindowProc(
    _In_ HWND hwnd,
    _In_ UINT uMsg, 
    _In_ WPARAM wParam, 
    _In_ LPARAM lParam)
{
    CButton* pThis = nullptr;

    if (uMsg == WM_CREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<CButton*>(pCS->lpCreateParams);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CButton*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis != nullptr)
    {
        // Assert(pThis->m_hwnd == hwnd);

        return pThis->WindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HRESULT CButton::Initialize(
    _In_ HWND hwndParent,
    _In_ PCWSTR pWindowName,
    _In_ int pImageFile)
{
    if (!EnsureWndClass())
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = CreateBitmapFromResource(g_pWICFactory, pImageFile, &m_hBitmap);
    if (SUCCEEDED(hr))
    {
        m_hwnd = CreateWindowEx(0,              // Optional window styles.
                                c_szLayeredButtonClassName,
                                pWindowName,
                                WS_CHILD,       // Window style
                                m_pt.x, 
                                m_pt.y, 
                                m_size.cx + 10, 
                                m_size.cy + 10,
                                hwndParent,     // Parent window    
                                NULL,           // Menu
                                g_hInstance,    // Instance handle
                                this);
        // TRACKMOUSEEVENT mouseTracking;
        // mouseTracking.hwndTrack = m_hwnd;
        // mouseTracking.dwFlags = TME_LEAVE;
        // mouseTracking.dwHoverTime = 700;
        // mouseTracking.cbSize = sizeof(mouseTracking);
        // BOOL mouseTreckSuccess = TrackMouseEvent(&mouseTracking);
    }   

    return hr;
}

LRESULT CButton::WindowProc(
    _In_ HWND hwnd,
    _In_ UINT uMsg, 
    _In_ WPARAM wParam, 
    _In_ LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            // Assert(m_hBitmap != NULL);
            WMCREATE(hwnd);
        }
        return 0;

        case WM_DESTROY:
        {
            hwnd = NULL;
            // PostQuitMessage(0);
        }
        return 0;
        
        case WM_MOUSEMOVE:
        {
            if (m_pHoverImageFileName != 0)
            {
                MouseMove(hwnd, lParam);
                if (!m_mouseDown)
                {
                    HBITMAP hoverHBitmap = NULL;
                    HRESULT hr = CreateBitmapFromResource(
                        g_pWICFactory, 
                        m_pHoverImageFileName, 
                        &hoverHBitmap);

                    LayerWindow(hwnd, hoverHBitmap, m_HoverSize, m_HoverPt);

                    TRACKMOUSEEVENT mouseTracking;
                    mouseTracking.hwndTrack = m_hwnd;
                    mouseTracking.dwFlags = TME_LEAVE;
                    mouseTracking.dwHoverTime = 700;
                    mouseTracking.cbSize = sizeof(mouseTracking);
                    BOOL mouseTrackSuccess = TrackMouseEvent(&mouseTracking);
                }                 
            }
        }
        return 0;

        case WM_MOUSELEAVE:
        {
            m_mouseDown = false;
            if (m_pHoverImageFileName != 0)
            {
                LayerWindow(hwnd, m_hBitmap, m_size, m_pt);      
            }
        }
        return 0;

        case WM_LBUTTONDOWN:
        {
            m_mouseDown = true;
            if (m_pMouseDownImageFileName != NULL)
            {
                HBITMAP mouseClickHBitmap = NULL;
                HRESULT hr = CreateBitmapFromResource(
                                                  g_pWICFactory, 
                                                  m_pMouseDownImageFileName, 
                                                  &mouseClickHBitmap);
                BOOL succeded = LayerWindow(
                    hwnd, 
                    mouseClickHBitmap, 
                    m_MouseDownSize, 
                    m_MouseDownPt);
                Assert(succeded != 0);
            }
            lButtonDown();
        }
        return 0;

        case WM_LBUTTONUP:
        {
            m_mouseDown = false;
            if (m_pMouseDownImageFileName != NULL)
            {
                LayerWindow(hwnd, m_hBitmap, m_size, m_pt);
            }
            
            OnClicked();
        }
        return 0;

        case WM_TIMER:
        {
            TimerTrigger(wParam);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CButton::WMCREATE(HWND hwnd)
{
    LayerWindow(hwnd, m_hBitmap, m_size, m_pt);
}







/* Override */
void CCloseButton::OnClicked() 
{
    HWND hwndParent = GetParent(m_hwnd);
    if (hwndParent != NULL)
    {
        DestroyWindow(hwndParent);
        g_settingsOpen = false;
    }
}

/* Override */
void CMinimizeButton::OnClicked() 
{
    HWND hwndParent = GetParent(m_hwnd);
    if (hwndParent != NULL)
    {
        ShowWindow(hwndParent, SW_MINIMIZE);
    }
}

/* Override */
void COptionsButton::OnClicked()
{
    HWND hwndParent = GetParent(m_hwnd);
    if (hwndParent != NULL)
    {
        if (!popupActive)
        {
            pPopup = ShowOptionsPopup(hwndParent);
        }
        else
        {
            HideOptionsPopup(pPopup);
        }           
    }
}

/* Override */
void CMouseCaptureButton::OnClicked()
{
    HWND hwndParent = GetParent(m_hwnd);
    if (hwndParent != NULL)
    {
        if (active)
        {
            BOOL success = ReleaseCapture();
            active = false;

            ShowWindow(g_highlightHwnd, SW_HIDE);
            if (hwndToSnip != NULL)
            {
                CaptureWindow(g_currentSnipPath, hwndToSnip);
            }
        } 
    }
}

/* Override */
void CMouseCaptureButton::MouseMove(HWND hwnd, LPARAM lParam)
{
    if (active)
    {
        // SetCapture(m_hwnd);

        POINT hitPos;
        hitPos.x = GET_X_LPARAM(lParam); 
        hitPos.y = GET_Y_LPARAM(lParam); 

        ClientToScreen(hwnd, &hitPos);

        HWND selectedHwnd = WindowFromPoint(hitPos);

        if ((selectedHwnd != GetDesktopWindow()) &&
            (selectedHwnd != GetParent(m_hwnd)) &&
            (GetParent(selectedHwnd) == NULL) &&
            (selectedHwnd != g_highlightHwnd))
        {
            RECT selectedRect;
            GetWindowRect(selectedHwnd, &selectedRect);

            SetWindowPos(
                g_highlightHwnd, 
                NULL,
                selectedRect.left,
                selectedRect.top,
                selectedRect.right - selectedRect.left,
                selectedRect.bottom - selectedRect.top,
                NULL);
            ShowWindow(g_highlightHwnd, SW_SHOW);
            hwndToSnip = selectedHwnd;
        }
        else if (selectedHwnd != g_highlightHwnd)
        {
            ShowWindow(g_highlightHwnd, SW_HIDE);
            selectedHwnd = NULL;
            hwndToSnip = selectedHwnd;
        }
    }
}

/* Override */
void CMouseCaptureButton::lButtonDown()
{
    SetCapture(m_hwnd);
    active = true;
}

/* Override */
void CAlertButton::OnClicked()
{
    DestroyWindow(GetParent(m_hwnd));
    ShellExecute(NULL, L"explore", m_filePath, NULL, NULL, SW_SHOW);
}

/* Override */
void CAlertButton::TimerTrigger(WPARAM timerId)
{
    if (timerId == 1)
    {
        DestroyWindow(GetParent(m_hwnd));
    }
}

/* Override */
void CZoneCaptureButton::OnClicked()
{
    if (!g_zoneActive)
    {
        HWND hwndDesktop = GetDesktopWindow();
        RECT crDesktop;
        GetClientRect(hwndDesktop, &crDesktop);

        g_snipRect = {};

        SetWindowPos(
            g_zoneSnipHwnd,
            NULL,
            crDesktop.left,
            crDesktop.top,
            (crDesktop.right - crDesktop.left),
            (crDesktop.bottom - crDesktop.top),
            NULL);
        ShowWindow(g_zoneSnipHwnd, SW_SHOW);

        g_zoneActive = true;
    }
    else
    {
        ShowWindow(g_zoneSnipHwnd, SW_HIDE);
        g_zoneActive = false;
    }    
}

/* Override */
void CMoreOptionsButton::OnClicked()
{
    if (!g_settingsOpen)
    {
        settingsHwnd = CreateSettingsWindow();
        g_settingsOpen = true;
    }
    DestroyWindow(GetParent(m_hwnd));
    popupActive = false;
}

/* Override */
void CFavoriteFolder1Button::OnClicked()
{
    PCWSTR selectedFolder = GetInputFolder();

    if (selectedFolder != nullptr)
    {
        StringCchCopy(
            g_szFavoriteFolder1,
            _countof(g_szFavoriteFolder1),
            selectedFolder);

        DestroyWindow(GetParent(m_hwnd));
        SetWindowPos(
            CreateSettingsWindow(),
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE);
    }
}

void CFavoriteFolder2Button::OnClicked()
{
    PCWSTR selectedFolder = GetInputFolder();

    if (selectedFolder != nullptr)
    {
        StringCchCopy(
            g_szFavoriteFolder2,
            _countof(g_szFavoriteFolder2),
            selectedFolder);

        DestroyWindow(GetParent(m_hwnd));
        SetWindowPos(
            CreateSettingsWindow(),
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE);
    }
}

void CFavoriteFolder3Button::OnClicked()
{
    PCWSTR selectedFolder = GetInputFolder();

    if (selectedFolder != nullptr)
    {
        StringCchCopy(
            g_szFavoriteFolder3,
            _countof(g_szFavoriteFolder3),
            selectedFolder);

        DestroyWindow(GetParent(m_hwnd));
        SetWindowPos(
            CreateSettingsWindow(),
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE);
    }
}


void CFolderTemplateButton::WMCREATE(HWND hwnd)
{
    // Add the WS_EX_LAYERED bit to the window ex style
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    HDC hdcSrc = CreateCompatibleDC(NULL);
    HGDIOBJ hBmpSave = SelectObject(hdcSrc, m_hBitmap);

    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    POINT ptNull = {0,0};

    COLORREF colorref = 0x00000000;
    SetTextColor(hdcSrc, colorref);

    size_t cbLength = 0;
    StringCbLength(g_szFavoriteFolder1, 
        _countof(g_szFavoriteFolder1),
        &cbLength);

    TextOut(
        hdcSrc,
        117,
        32,
        g_szFavoriteFolder1,
        cbLength);

    StringCbLength(g_szFavoriteFolder2, 
        _countof(g_szFavoriteFolder2),
        &cbLength);

    TextOut(
        hdcSrc,
        117,
        114,
        g_szFavoriteFolder2,
        cbLength);

    StringCbLength(g_szFavoriteFolder3, 
        _countof(g_szFavoriteFolder3),
        &cbLength);

    TextOut(
        hdcSrc,
        117,
        195,
        g_szFavoriteFolder3,
        cbLength);

    BOOL bRet = UpdateLayeredWindow(
        hwnd,
        NULL,
        &m_pt,
        &m_size,
        hdcSrc,
        &ptNull,
        NULL,
        &bf,
        ULW_ALPHA);

    SelectObject(hdcSrc, hBmpSave);
    DeleteDC(hdcSrc);

}

void CFavorite1RadioButton::OnClicked()
{
    if (g_currentSnipPath != g_szFavoriteFolder1)
    {
        StringCchCopyW(
            g_currentSnipPath,
            _countof(g_szFavoriteFolder1),
            g_szFavoriteFolder1);
    }
}

void CFavorite2RadioButton::OnClicked()
{
    if (g_currentSnipPath != g_szFavoriteFolder2)
    {
        StringCchCopyW(
            g_currentSnipPath,
            _countof(g_szFavoriteFolder2),
            g_szFavoriteFolder2);
    }
}

void CFavorite3RadioButton::OnClicked()
{
    if (g_currentSnipPath != g_szFavoriteFolder3)
    {
        StringCchCopyW(
            g_currentSnipPath,
            _countof(g_szFavoriteFolder3),
            g_szFavoriteFolder3);
    }
}