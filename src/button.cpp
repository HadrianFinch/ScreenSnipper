#include "precomp.h"

bool CButton::s_classRegistered = false;

/*static*/
HRESULT CButton::Create(
    _In_ HWND hwndParent,
    _In_ PCWSTR pWindowName,
    _In_ PCWSTR pImageFile,
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
    _In_ PCWSTR pImageFile)
{
    if (!EnsureWndClass())
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = CreateBitmapFromFile(g_pWICFactory, pImageFile, &m_hBitmap);
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

            LayerWindow(hwnd, m_hBitmap, m_size, m_pt);
        }
        return 0;

        case WM_DESTROY:
        {
            hwnd = NULL;
            PostQuitMessage(0);
        }
        return 0;
        
        case WM_MOUSEMOVE:
        {
            if (m_pHoverImageFileName != nullptr)
            {
                if (!m_mouseDown)
                {
                    HBITMAP hoverHBitmap = NULL;
                    HRESULT hr = CreateBitmapFromFile(
                                                    g_pWICFactory, 
                                                    m_pHoverImageFileName, 
                                                    &hoverHBitmap);
                    LayerWindow(hwnd, hoverHBitmap, m_HoverSize, m_HoverPt);
                    TRACKMOUSEEVENT mouseTracking;
                    mouseTracking.hwndTrack = m_hwnd;
                    mouseTracking.dwFlags = TME_LEAVE;
                    mouseTracking.dwHoverTime = 700;
                    mouseTracking.cbSize = sizeof(mouseTracking);
                    BOOL mouseTreckSuccess = TrackMouseEvent(&mouseTracking);
                }                 
            }
        }
        return 0;

        case WM_MOUSELEAVE:
        {
            m_mouseDown = false;
            if (m_pHoverImageFileName != nullptr)
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
                HRESULT hr = CreateBitmapFromFile(
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
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}








/* Overide */
void COptionsButton::OnClicked()
{
    HWND hwndParent = GetParent(m_hwnd);
    if (hwndParent != NULL)
    {
        if (!popupActive)
        {
            ShowOptionsPopup(hwndParent);
        }
        else
        {
            HideOptionsPopup(hwndParent);
        }           
    }
}