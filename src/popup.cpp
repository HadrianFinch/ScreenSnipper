#include "precomp.h"

bool CPopup::s_classRegistered = false;

/*static*/
HRESULT CPopup::Create(
    _In_ PCWSTR pWindowName,
    _In_ PCWSTR pImageFile,
    _In_ SIZE size,
    _In_ POINT pt,
    _Outptr_ CPopup** ppPopup)
{
    HRESULT hr = S_OK;

    if (EnsureWndClass())
    {
        CPopup* pPopup = new CPopup(pImageFile, size, pt);

        hr = pPopup->Initialize(pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pPopup->m_hwnd, SW_SHOW);

            *ppPopup = pPopup;
            pPopup = nullptr;
        }

        delete pPopup;
    }
    else
    {
        hr = E_FAIL;
    }

    return hr;
}

/*static*/
bool CPopup::EnsureWndClass()
{
    if (!s_classRegistered)
    {
        WNDCLASSW wndClass = {};

        wndClass.lpfnWndProc = CPopup::s_WindowProc;
        wndClass.hInstance = g_hInstance;
        wndClass.lpszClassName = c_szLayeredPopupClassName;
        wndClass.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(128));

        if (RegisterClass(&wndClass))
        {
            s_classRegistered = true;
        }
    }

    return s_classRegistered;
}

/*static*/
LRESULT CALLBACK CPopup::s_WindowProc(
    _In_ HWND hwnd,
    _In_ UINT uMsg, 
    _In_ WPARAM wParam, 
    _In_ LPARAM lParam)
{
    CPopup* pThis = nullptr;

    if (uMsg == WM_CREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<CPopup*>(pCS->lpCreateParams);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CPopup*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis != nullptr)
    {
        // Assert(pThis->m_hwnd == hwnd);

        return pThis->WindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HRESULT CPopup::Initialize(
    _In_ PCWSTR pWindowName,
    _In_ PCWSTR pImageFile)
{
    HRESULT hr = CreateBitmapFromFile(g_pWICFactory, pImageFile, &m_hBitmap);

    if (SUCCEEDED(hr))
    {
        m_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,              // Optional window styles.
                                c_szLayeredPopupClassName,
                                pWindowName,
                                WS_POPUPWINDOW | WS_CLIPCHILDREN, // Window style
                                m_pt.x, 
                                m_pt.y, 
                                m_size.cx, 
                                m_size.cy,
                                NULL,     // Parent window    
                                NULL,           // Menu
                                g_hInstance,    // Instance handle
                                this);
        if (m_hwnd == NULL)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }   

    return hr;
}

LRESULT CPopup::WindowProc(
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
            if (m_masterPopup)
            {
                SaveFavoriteFolders();
                PostQuitMessage(0);
            }
        }
        return 0;

        case WM_NCHITTEST:
        {
            if (m_dragRect.bottom != -1 &&
                m_dragRect.top != -1 &&
                m_dragRect.right != -1 &&
                m_dragRect.left != -1)
            {
                DWORD coordintes = static_cast<DWORD>(lParam);
                POINT hitPos;
                hitPos.x = GET_X_LPARAM(lParam); 
                hitPos.y = GET_Y_LPARAM(lParam); 

                // RECT wndRect;
                POINT tl = {m_dragRect.left, m_dragRect.top};
                POINT br = {m_dragRect.right, m_dragRect.bottom};

                ClientToScreen(hwnd, &tl);
                ClientToScreen(hwnd, &br);

                if (hitPos.y >= tl.y &&
                    hitPos.y <= br.y &&
                    hitPos.x >= tl.x &&
                    hitPos.x <= br.x)
                {
                    return HTCAPTION;
                }
            }
        }
        return HTCLIENT;

        case WM_HOTKEY:
        {
            if (wParam == 1)
            {
                if (m_masterPopup)
                {
                    CaptureScreen(g_currentSnipPath, hwnd);
                }
            }
            else if (wParam == 2)
            {
                CaptureWindow(g_currentSnipPath, GetForegroundWindow());
            }
            else if (wParam == 3)
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
                    
                    
        }
        return 0;

        case WM_LBUTTONDOWN:
        {
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



HRESULT CWindowedPopup::Initialize(
    _In_ PCWSTR pWindowName,
    _In_ PCWSTR pImageFile)
{
    EnsureWndClass();
    HRESULT hr = CreateBitmapFromFile(g_pWICFactory, pImageFile, &m_hBitmap);

    if (SUCCEEDED(hr))
    {
        m_hwnd = CreateWindowEx(0,              // Optional window styles.
                                c_szLayeredPopupClassName,
                                pWindowName,
                                WS_POPUPWINDOW | WS_CLIPCHILDREN, // Window style
                                m_pt.x, 
                                m_pt.y, 
                                m_size.cx, 
                                m_size.cy,
                                NULL,     // Parent window    
                                NULL,           // Menu
                                g_hInstance,    // Instance handle
                                this);
        if (m_hwnd == NULL)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }   

    return hr;
}
