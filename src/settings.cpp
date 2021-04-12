#include "precomp.h"

WCHAR g_szFavoriteFolder1[MAX_PATH] = {};
WCHAR g_szFavoriteFolder2[MAX_PATH] = {};
WCHAR g_szFavoriteFolder3[MAX_PATH] = {};
WCHAR g_currentSnipPath[MAX_PATH] = {};

HWND CreateSettingsWindow()
{
    RECT desktopClientRect;
    HWND desktopHwnd = GetDesktopWindow();
    GetClientRect(desktopHwnd, &desktopClientRect);

    SIZE size = {484, 321};
    POINT pt = {(((desktopClientRect.right - desktopClientRect.left) / 2) - (size.cx / 2)), ((desktopClientRect.bottom - desktopClientRect.top) / 2) - (size.cy / 2)};
    
    CPopup* pSettingsWindow = nullptr;
    CWindowedPopup::Create(
        L"ScreenSnipper Settings",
        L"optionsWindowImages\\optionsWindow.png",
        size,
        pt,
        &pSettingsWindow);
    pSettingsWindow->m_dragRect = {0, 0, 473, 23};

    SetWindowLongPtr(pSettingsWindow->m_hwnd, GWL_EXSTYLE, WS_EX_LAYERED);

    size = {13, 13};
    pt = {6, 6};
    CButton* pCloseButton = nullptr;
    CCloseButton::Create(
        pSettingsWindow->m_hwnd,
        L"Close Button",
        L"optionsWindowImages\\closeButton.png",
        size,
        pt,
        &pCloseButton);
    pCloseButton->m_pHoverImageFileName = L"optionsWindowImages\\closeButtonHighlight.png";
    pCloseButton->m_HoverSize = size;
    pCloseButton->m_HoverPt = pt;

    size = {99, 36};
    pt = {6, 33};
    CButton* pFavoriteFoldersButton = nullptr;
    CButton::Create(
        pSettingsWindow->m_hwnd,
        L"favorite folders button",
        L"optionsWindowImages\\favoriteFoldersButton.png",
        size,
        pt,
        &pFavoriteFoldersButton);
    pFavoriteFoldersButton->m_pHoverImageFileName = L"optionsWindowImages\\fovoriteFoldersButtonHighlight.png";
    pFavoriteFoldersButton->m_HoverPt = pt;
    pFavoriteFoldersButton->m_HoverSize = size;

    
    size = {88, 25};
    pt = {139, 78};
    CButton* pSelectFolder1 = nullptr;
    CFavoriteFolder1Button::Create(
        pSettingsWindow->m_hwnd,
        L"Select Folder 1",
        L"optionsWindowImages\\selectFolderButton.png",
        size,
        pt,
        &pSelectFolder1);
    pSelectFolder1->m_pHoverImageFileName = L"optionsWindowImages\\selectFolderButtonHighlght.png";
    pSelectFolder1->m_HoverPt = pt;
    pSelectFolder1->m_HoverSize = size;

    size = {88, 25};
    pt = {139, 159};
    CButton* pSelectFolder2 = nullptr;
    CFavoriteFolder2Button::Create(
        pSettingsWindow->m_hwnd,
        L"Select Folder 2",
        L"optionsWindowImages\\selectFolderButton.png",
        size,
        pt,
        &pSelectFolder2);
    pSelectFolder2->m_pHoverImageFileName = L"optionsWindowImages\\selectFolderButtonHighlght.png";
    pSelectFolder2->m_HoverPt = pt;
    pSelectFolder2->m_HoverSize = size;

    size = {88, 25};
    pt = {139, 240};
    CButton* pSelectFolder3 = nullptr;
    CFavoriteFolder3Button::Create(
        pSettingsWindow->m_hwnd,
        L"Select Folder 3",
        L"optionsWindowImages\\selectFolderButton.png",
        size,
        pt,
        &pSelectFolder3);
    pSelectFolder3->m_pHoverImageFileName = L"optionsWindowImages\\selectFolderButtonHighlght.png";
    pSelectFolder3->m_HoverPt = pt;
    pSelectFolder3->m_HoverSize = size;

    size = {331, 241};
    pt = {127, 48};
    CButton* pTemplate = nullptr;
    CFolderTemplateButton::Create(
        pSettingsWindow->m_hwnd,
        L"template",
        L"optionsWindowImages\\foldersTemplate.png",
        size,
        pt,
        &pTemplate);

    return pSettingsWindow->m_hwnd;
}

PCWSTR GetInputFolder()
{
    PWSTR pszFilePath = NULL;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, 
            NULL, 
            CLSCTX_ALL, 
            IID_IFileOpenDialog, 
            reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            pFileOpen->SetOptions(FOS_PICKFOLDERS);

            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        if (dbg >= 3)
                        {
                            MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
                            
                        }
                        CoTaskMemFree(pszFilePath);
                    }

                    pItem->Release();
                }
            }                 

            pFileOpen->Release();
        }

        CoUninitialize();
    }

    return pszFilePath;
}

void LoadFavoriteFolders()
{
    HKEY hkey = NULL;
    bool loadedFav1 = false;
    bool loadedFav2 = false;
    bool loadedFav3 = false;
    bool loadedSnipPath = false;

    auto result = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\BlueDragon\\ScreenSnipper",
        0,
        KEY_QUERY_VALUE,
        &hkey);
    if (result == ERROR_SUCCESS)
    {
        DWORD dwType;
        DWORD cbData = sizeof(g_szFavoriteFolder1);

        result = RegQueryValueEx(
            hkey,
            L"FavoriteFolder1",
            NULL,
            &dwType,
            reinterpret_cast<LPBYTE>(g_szFavoriteFolder1),
            &cbData);

        if ((result == ERROR_SUCCESS) &&
            (dwType == REG_SZ))
        {
            loadedFav1 = true;
        }

        cbData = sizeof(g_szFavoriteFolder2);
        result = RegQueryValueEx(
            hkey,
            L"FavoriteFolder2",
            NULL,
            &dwType,
            reinterpret_cast<LPBYTE>(g_szFavoriteFolder2),
            &cbData);

        if ((result == ERROR_SUCCESS) &&
            (dwType == REG_SZ))
        {
            loadedFav2 = true;
        }
        
        cbData = sizeof(g_szFavoriteFolder3);
        result = RegQueryValueEx(
            hkey,
            L"FavoriteFolder3",
            NULL,
            &dwType,
            reinterpret_cast<LPBYTE>(g_szFavoriteFolder3),
            &cbData);

        if ((result == ERROR_SUCCESS) &&
            (dwType == REG_SZ))
        {
            loadedFav3 = true;
        }

        cbData = sizeof(g_currentSnipPath);
        result = RegQueryValueEx(
            hkey,
            L"CurrentSnipPath",
            NULL,
            &dwType,
            reinterpret_cast<LPBYTE>(g_currentSnipPath),
            &cbData);

        if ((result == ERROR_SUCCESS) &&
            (dwType == REG_SZ))
        {
            loadedSnipPath = true;
        }
    }

    if (!loadedFav1)
    {
        ExpandEnvironmentStrings(
            L"%userprofile%\\Desktop",
            g_szFavoriteFolder1,
            _countof(g_szFavoriteFolder1));
    }

    if (!loadedFav2)
    {
        ExpandEnvironmentStrings(
            L"%userprofile%\\Desktop",
            g_szFavoriteFolder2,
            _countof(g_szFavoriteFolder2));
    }

    if (!loadedFav3)
    {
        ExpandEnvironmentStrings(
            L"%userprofile%\\Desktop",
            g_szFavoriteFolder3,
            _countof(g_szFavoriteFolder3));
    }

    if (!loadedSnipPath)
    {
        ExpandEnvironmentStrings(
            L"%userprofile%\\Desktop",
            g_currentSnipPath,
            _countof(g_currentSnipPath));
    }
    
    if (hkey != NULL)
    {
        RegCloseKey(hkey);
    }
    
    return;
}

void SaveFavoriteFolders()
{
    HKEY hkey = NULL;
    auto result = RegCreateKeyEx(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\BlueDragon\\ScreenSnipper",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        &hkey,
        NULL);
    if (result == ERROR_SUCCESS)
    {
        size_t cbSize;

        StringCbLength(
            g_szFavoriteFolder1,
            sizeof(g_szFavoriteFolder1),
            &cbSize);

        RegSetValueEx(
            hkey,
            L"FavoriteFolder1",
            0,
            REG_SZ,
            reinterpret_cast<LPBYTE>(g_szFavoriteFolder1),
            (cbSize + sizeof(WCHAR)));


        StringCbLength(
            g_szFavoriteFolder2,
            sizeof(g_szFavoriteFolder2),
            &cbSize);

        RegSetValueEx(
            hkey,
            L"FavoriteFolder2",
            0,
            REG_SZ,
            reinterpret_cast<LPBYTE>(g_szFavoriteFolder2),
            (cbSize + sizeof(WCHAR)));


        StringCbLength(
            g_szFavoriteFolder3,
            sizeof(g_szFavoriteFolder3),
            &cbSize);

        RegSetValueEx(
            hkey,
            L"FavoriteFolder3",
            0,
            REG_SZ,
            reinterpret_cast<LPBYTE>(g_szFavoriteFolder3),
            (cbSize + sizeof(WCHAR)));

        
        StringCbLength(
            g_currentSnipPath,
            sizeof(g_currentSnipPath),
            &cbSize);

        RegSetValueEx(
            hkey,
            L"CurrentSnipPath",
            0,
            REG_SZ,
            reinterpret_cast<LPBYTE>(g_currentSnipPath),
            (cbSize + sizeof(WCHAR)));
    }

    if (hkey != NULL)
    {
        RegCloseKey(hkey);
    }
    
    return;
}