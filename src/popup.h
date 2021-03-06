class CPopup
{
public:
    static HRESULT Create(
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CPopup** ppPopup);
protected:
    static bool EnsureWndClass();

    static LRESULT CALLBACK s_WindowProc(
        _In_ HWND hwnd,
        _In_ UINT uMsg, 
        _In_ WPARAM wParam, 
        _In_ LPARAM lParam);

    static constexpr PCWSTR c_szLayeredPopupClassName = L"LayeredPopup";
    static bool s_classRegistered;

    HRESULT Initialize(
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile);

public:
    CPopup(
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt)
        :
        m_size(size),
        m_pt(pt)
    {
    }

    ~CPopup()
    {
        if (m_hwnd != NULL)
        {
            DestroyWindow(m_hwnd);
        }
    }

protected:
    LRESULT WindowProc(
        _In_ HWND HWND,
        _In_ UINT uMsg, 
        _In_ WPARAM wParam, 
        _In_ LPARAM lParam);

public:
    RECT m_dragRect = {-1, -1, -1, -1};
    HWND m_hwnd = NULL;
    HBITMAP m_hBitmap = NULL;
    SIZE m_size;
    POINT m_pt;
    bool m_masterPopup = false;
};


class CWindowedPopup :
    public CPopup
{
public:
    static HRESULT Create(
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CPopup** ppPopup)
    {
        CWindowedPopup* pPopup = new CWindowedPopup(pImageFile, size, pt);

        HRESULT hr = pPopup->Initialize(pWindowName, pImageFile);

        if (SUCCEEDED(hr))
        {
            ShowWindow(pPopup->m_hwnd, SW_SHOW);

            *ppPopup = pPopup;
            pPopup = nullptr;
        }

        delete pPopup; 

        return hr;
    }

    CWindowedPopup(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CPopup(pImageFile, size, pt)
    {
    }

protected:
    HRESULT Initialize(
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile);
};

class CWindowedTopmostPopup :
    public CPopup
{
public:
    static HRESULT Create(
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CPopup** ppPopup)
    {
        CWindowedTopmostPopup* pPopup = new CWindowedTopmostPopup(pImageFile, size, pt);

        HRESULT hr = pPopup->Initialize(pWindowName, pImageFile);

        if (SUCCEEDED(hr))
        {
            ShowWindow(pPopup->m_hwnd, SW_SHOW);

            *ppPopup = pPopup;
            pPopup = nullptr;
        }

        delete pPopup; 

        return hr;
    }

    CWindowedTopmostPopup(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CPopup(pImageFile, size, pt)
    {
    }

protected:
    HRESULT Initialize(
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile);
};
