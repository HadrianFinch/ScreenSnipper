extern WCHAR g_szFavoriteFolder1[MAX_PATH];
extern WCHAR g_szFavoriteFolder2[MAX_PATH];
extern WCHAR g_szFavoriteFolder3[MAX_PATH];
extern WCHAR g_currentSnipPath[MAX_PATH];

class CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton);
protected:
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
        _In_ int pImageFile);

public:
    CButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        m_size(size),
        m_pt(pt)
    {
    }

    virtual ~CButton()
    {
        if (m_hwnd != NULL)
        {
            DestroyWindow(m_hwnd);
        }
    }

protected:
    virtual void OnClicked()
    {
    }
    virtual void TimerTrigger(WPARAM timerId)
    {
        KillTimer(m_hwnd, timerId);
    }
    virtual void lButtonDown()
    {
    }
    virtual void MouseMove(HWND hwnd, LPARAM lParam)
    {
    }
    virtual void WMCREATE(HWND hwnd);
    

    LRESULT WindowProc(
        _In_ HWND HWND,
        _In_ UINT uMsg, 
        _In_ WPARAM wParam, 
        _In_ LPARAM lParam);
private:
    bool m_mouseDown = false;

public:
    int m_pHoverImageFileName = 0;
    SIZE m_HoverSize;
    POINT m_HoverPt;

    int m_pMouseDownImageFileName = 0;
    SIZE m_MouseDownSize;
    POINT m_MouseDownPt;

protected:
    HWND m_hwnd = NULL;
    HBITMAP m_hBitmap = NULL;
    SIZE m_size;
    POINT m_pt;
};




























class CCloseButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CCloseButton* pButton = new CCloseButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CCloseButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }

protected:
    void OnClicked() override;
};

class CMinimizeButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CMinimizeButton* pButton = new CMinimizeButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CMinimizeButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }

protected:
    void OnClicked() override;
};




class CScreenCaptureButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CScreenCaptureButton* pButton = new CScreenCaptureButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CScreenCaptureButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }

protected:
    void OnClicked() override
    {
        HWND hwndParent = GetParent(m_hwnd);
        if (hwndParent != NULL)
        {
            CaptureScreen(g_currentSnipPath, hwndParent);
        }
    }
};




class COptionsButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        COptionsButton* pButton = new COptionsButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    COptionsButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }

protected:
    CPopup* pPopup = nullptr;
    void OnClicked() override;
};





class CMouseCaptureButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CMouseCaptureButton* pButton = new CMouseCaptureButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CMouseCaptureButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }

protected:
    bool active = false;
    HWND hwndToSnip = NULL;

    void OnClicked() override;
    void lButtonDown() override;
    void MouseMove(HWND hwnd, LPARAM lParam) override;
    
};


class CAlertButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CAlertButton* pButton = new CAlertButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);

        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);
            SetTimer(pButton->m_hwnd, 1, 4500, NULL);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CAlertButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }
public:
    WCHAR m_filePath[MAX_PATH] = {};

protected:
    void OnClicked() override;
    void TimerTrigger(WPARAM timerId) override;
};

class CZoneCaptureButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CZoneCaptureButton* pButton = new CZoneCaptureButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CZoneCaptureButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }

protected:
    void OnClicked() override;
};

class CMoreOptionsButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CMoreOptionsButton* pButton = new CMoreOptionsButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CMoreOptionsButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }
protected:
    HWND settingsHwnd = NULL;
    void OnClicked() override;
};

class CFavoriteFolder1Button :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CFavoriteFolder1Button* pButton = new CFavoriteFolder1Button(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CFavoriteFolder1Button(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }
protected:
    void OnClicked() override;
};

class CFavoriteFolder2Button :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CFavoriteFolder2Button* pButton = new CFavoriteFolder2Button(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CFavoriteFolder2Button(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }
protected:
    void OnClicked() override;
};

class CFavoriteFolder3Button :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CFavoriteFolder3Button* pButton = new CFavoriteFolder3Button(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CFavoriteFolder3Button(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }
protected:
    void OnClicked() override;
};


class CFolderTemplateButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CFolderTemplateButton* pButton = new CFolderTemplateButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CFolderTemplateButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }
public:
    void WMCREATE(HWND hwnd) override;
};

class CFavorite1RadioButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ int pImageFile,
        _In_ SIZE size,
        _In_ POINT pt,
        _Outptr_ CButton** ppButton)
    {
        CFavorite1RadioButton* pButton = new CFavorite1RadioButton(pImageFile, size, pt);

        HRESULT hr = pButton->Initialize(hwndParent, pWindowName, pImageFile);
        if (SUCCEEDED(hr))
        {
            ShowWindow(pButton->m_hwnd, SW_SHOW);

            *ppButton = pButton;
            pButton = nullptr;
        }

        delete pButton;

        return hr;
    }

    CFavorite1RadioButton(
        _In_ int pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }
protected:
    void OnClicked() override;
};