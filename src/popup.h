class CPopup
{
public:
    static HRESULT Create(
        _In_ PCWSTR pWindowName,
        _In_ PCWSTR pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CPopup** ppPopup);
private:
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
        _In_ PCWSTR pImageFile);

public:
    CPopup(
        _In_ PCWSTR pImageFile,
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
