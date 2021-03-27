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

    if (EnsureWndClass())
    {
        CButton* pButton = new CButton(pImageFile, size, pt);

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
        hr = E_FAIL;
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
        pThis = reinterpret_cast<CButton*>(lParam);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<CButton*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis != nullptr)
    {
        Assert(pThis->m_hwnd == hwnd);

        return pThis->WindowProc(uMsg, wParam, lParam);
    }

    return 0;
}

HRESULT CButton::Initialize(
    _In_ HWND hwndParent,
    _In_ PCWSTR pWindowName,
    _In_ PCWSTR pImageFile)
{
    HRESULT hr = CreateBitmapFromFile(g_pWICFactory, pImageFile, &m_hBitmap);

    if (SUCCEEDED(hr))
    {
        m_hwnd = CreateWindowEx(0,              // Optional window styles.
                                 c_szLayeredButtonClassName,
                                 pWindowName,
                                 WS_CHILD,       // Window style
                                 m_pt.x, 
                                 m_pt.y, 
                                 m_size.cx, 
                                 m_size.cy,
                                 hwndParent,     // Parent window    
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

LRESULT CButton::WindowProc(
    _In_ UINT uMsg, 
    _In_ WPARAM wParam, 
    _In_ LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            Assert(m_hBitmap != NULL);

            LayerWindow(m_hwnd, m_hBitmap, m_size, m_pt);
        }
        return 0;

        case WM_DESTROY:
        {
            m_hwnd = NULL;
            PostQuitMessage(0);
        }
        return 0;

        // case WM_NCCALCSIZE:
        // {
        //     if (wParam)
        //     {
        //         NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
        //     }
            
        // }
        // return 0;
    }

    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

