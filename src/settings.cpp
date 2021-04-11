#include "precomp.h"

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
    CButton::Create(
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
    CButton::Create(
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
    CButton::Create(
        pSettingsWindow->m_hwnd,
        L"Select Folder 2",
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
    CButton::Create(
        pSettingsWindow->m_hwnd,
        L"template",
        L"optionsWindowImages\\foldersTemplate.png",
        size,
        pt,
        &pTemplate);

    return pSettingsWindow->m_hwnd;
}

void GetInputFolder(HWND dialogOwner)
{
    

}