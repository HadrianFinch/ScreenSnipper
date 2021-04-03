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

    LRESULT WindowProc(
        _In_ HWND HWND,
        _In_ UINT uMsg, 
        _In_ WPARAM wParam, 
        _In_ LPARAM lParam);
private:
    bool m_mouseDown = false;

public:
    PCWSTR m_pHoverImageFileName = nullptr;
    SIZE m_HoverSize;
    POINT m_HoverPt;

    PCWSTR m_pMouseDownImageFileName = nullptr;
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
        _In_ PCWSTR pImageFile,
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
        _In_ PCWSTR pImageFile, 
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
            DestroyWindow(hwndParent);
        }
    }
};




class CScreenCaptureButton :
    public CButton
{
public:
    static HRESULT Create(
        _In_ HWND hwndParent,
        _In_ PCWSTR pWindowName,
        _In_ PCWSTR pImageFile,
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
        _In_ PCWSTR pImageFile, 
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
            CaptureScreen(hwndParent);
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
        _In_ PCWSTR pImageFile,
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
        _In_ PCWSTR pImageFile, 
        _In_ SIZE size,
        _In_ POINT pt)
        :
        CButton(pImageFile, size, pt)
    {
    }

protected:
    CPopup* pOptionsMenu = nullptr;
    bool popupActive = false;
    bool popupCreated = false;

    void OnClicked() override
    {
        HWND hwndParent = GetParent(m_hwnd);
        if (hwndParent != NULL)
        {
            if (!popupActive)
            {
                if (!popupCreated)
                {
                    SIZE size = {90, 109};
                    POINT pt = {349, -112};
                    ClientToScreen(hwndParent, &pt);
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
                        CScreenCaptureButton::Create(pOptionsMenu->m_hwnd,
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
                        CScreenCaptureButton::Create(pOptionsMenu->m_hwnd,
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
                        CScreenCaptureButton::Create(pOptionsMenu->m_hwnd,
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
                        CScreenCaptureButton::Create(pOptionsMenu->m_hwnd,
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
                    ClientToScreen(hwndParent, &pt);
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
            else
            {
                ShowWindow(pOptionsMenu->m_hwnd, SW_HIDE);
                popupActive = false;
            }           
        }
    }
};
