class CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ PCWSTR pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton);

private:
    static bool EnsureWndClass();

    static LRESULT CALLBACK s_WindowProc(
        _In_ HWND hwnd,
        _In_ UINT uMsg, 
        _In_ WPARAM wParam, 
        _In_ LPARAM lParam);

    static constexpr PCWSTR c_szLayeredButtonClassName = L"LayeredButton";
    static bool s_classRegistered;

    HRESULT Initialize(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ PCWSTR pImageFile);

public:
    CButton(
        _In_ PCWSTR pImageFile,
        _In_ SIZE size,
        _In_ POINT pt)
        :
        m_size(size),
        m_pt(pt)
    {
    }

    ~CButton()
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

private:
    HWND m_hwnd = NULL;
    HBITMAP m_hBitmap = NULL;
    SIZE m_size;
    POINT m_pt;
};
