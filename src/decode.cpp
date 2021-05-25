
#include "precomp.h"

UINT snipCount = NULL;

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

void SnipSavedAlert(PCWSTR filePath)
{
    RECT desktopClientRect;
    HWND desktopHwnd = GetDesktopWindow();
    GetClientRect(desktopHwnd, &desktopClientRect);

    SIZE size = {200, 45};
    POINT pt = {(((desktopClientRect.right - desktopClientRect.left) / 2) - 100), (25)};

    CPopup* pContainer = nullptr;
    HRESULT hr = CPopup::Create(
        L"File Save Container",
        PU_IMG_CONTAINER, 
        size,
        pt,
        &pContainer);
    Assert(SUCCEEDED(hr));
    CButton* pAlert = nullptr;
    hr = CAlertButton::Create(
        pContainer->m_hwnd,
        L"File Saved Popup",
        PU_IMG_SAVED,
        size,
        {-1, -1},
        &pAlert);
    
    auto pAlertBtn = static_cast<CAlertButton*>(pAlert);
    StringCchCopy(
        pAlertBtn->m_filePath, 
        _countof(pAlertBtn->m_filePath),
        filePath);
    
    pAlert->m_pHoverImageFileName = PU_IMG_HVR_SAVED;
    pAlert->m_HoverPt = {-1, -1};
    pAlert->m_HoverSize = size;
    
}

HRESULT CreateWICFactory(
    _Outptr_ IWICImagingFactory** ppFactory)
{
    // Create WIC factory
    return CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(ppFactory)
        );
}

HRESULT CreateBitmapFromResource(
    _In_ IWICImagingFactory* pIWICFactory,
    _In_ PCWSTR uri,
    _Out_ HBITMAP* phBitmap)
{
    
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;
    IWICBitmapScaler *pScaler = NULL;

    IWICBitmapDecoder* pDecoder = nullptr;
    RETURN_IF_FAILED(pIWICFactory->CreateDecoderFromFilename(
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder));


    IWICBitmapFrameDecode* pFirstFrame = nullptr;
    RETURN_IF_FAILED(pDecoder->GetFrame(0, &pFirstFrame));

    // Convert the image format to 32bppPBGRA
    RETURN_IF_FAILED(pIWICFactory->CreateFormatConverter(&pConverter));

    RETURN_IF_FAILED(
        pConverter->Initialize(
        pFirstFrame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut));

    UINT width = 0;
    UINT height = 0;
    RETURN_IF_FAILED(pConverter->GetSize(&width, &height));

    UINT stride = ALIGN_SIZE(width * 4, sizeof(DWORD));
    UINT cbSize = stride * height;
    BYTE* pMemory = new BYTE[cbSize];

    WICRect bitmapRect = {0, 0, static_cast<INT>(width), static_cast<INT>(height)};
    RETURN_IF_FAILED(pConverter->CopyPixels(
        &bitmapRect,
        stride,
        cbSize,
        pMemory));

    *phBitmap = CreateBitmap(
        width,
        height,
        1,
        32,
        pMemory);

    if (*phBitmap == NULL)
    {
        return E_OUTOFMEMORY;
    } 

    delete(pMemory);

    // release varibles from memory
    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);
    SafeRelease(&pScaler);
    SafeRelease(&pFirstFrame);
    return S_OK;
}

HRESULT CaptureScreen(PCWSTR directory, HWND parrentHwnd)
{
    HWND hwnd;
    HDC hdc[2];
    HBITMAP hbitmap;
    RECT rect;

    ShowWindow(parrentHwnd, SW_HIDE);

    hwnd = GetDesktopWindow();
    GetClientRect(hwnd, &rect);    
    hdc[0] = GetWindowDC(hwnd);
    hbitmap = CreateCompatibleBitmap(hdc[0], rect.right, rect.bottom); 
    hdc[1] = CreateCompatibleDC(hdc[0]);
    HGDIOBJ hbmpSave = SelectObject(hdc[1], hbitmap);    

    BitBlt (
        hdc[1],
        0,
        0,
        rect.right,
        rect.bottom,
        hdc[0],
        0,
        0,
        SRCCOPY);

    ShowWindow(parrentHwnd, SW_SHOW);

    SYSTEMTIME lt;
    GetLocalTime(&lt);

    WCHAR filePath[MAX_PATH] = {};
    StringCchPrintfW(filePath,
        _countof(filePath),
        L"%s\\ScreenSnip Screen %02d-%02d-%02d",
        directory,
        lt.wMonth, lt.wDay, lt.wYear);

    CreateSaveFile(filePath, hbitmap);

    SnipSavedAlert(directory);

    SelectObject(hdc[1], hbmpSave);
    DeleteDC(hdc[1]);
    
    DeleteDC(hdc[0]);
    
    return S_OK;
}

HRESULT CaptureWindow(PCWSTR directory, HWND windowToSnip)
{
    HWND hwnd;
    
    HBITMAP hbitmap;
    RECT rect;
    RECT shortenedFrame;
    
    DwmGetWindowAttribute(
        windowToSnip,
        DWMWA_EXTENDED_FRAME_BOUNDS,
        &shortenedFrame,
        sizeof(shortenedFrame));

    GetWindowRect(windowToSnip, &rect);   

    POINT ptOffset = {(shortenedFrame.left - rect.left),
                      (shortenedFrame.top - rect.top)};
    
    HDC hdcWindow = GetWindowDC(windowToSnip);
    HDC hdcSrc = CreateCompatibleDC(hdcWindow);

    hbitmap = CreateCompatibleBitmap(
        hdcWindow,
        (rect.right - rect.left),
        (rect.bottom - rect.top));

    HGDIOBJ hbmpSave =  SelectObject(hdcSrc, hbitmap);

    PrintWindow(windowToSnip, hdcSrc, PW_RENDERFULLCONTENT);

    HDC hdcResize = CreateCompatibleDC(hdcWindow);
    HBITMAP hbitmapResize = CreateCompatibleBitmap(
        hdcWindow,
        (shortenedFrame.right - shortenedFrame.left),
        (shortenedFrame.bottom - shortenedFrame.top));
    
    HGDIOBJ hbmpSave2 = SelectObject(hdcResize, hbitmapResize);

    BitBlt(
        hdcResize,
        0,
        0,
        (shortenedFrame.right - shortenedFrame.left),
        (shortenedFrame.bottom - shortenedFrame.top),
        hdcSrc,
        ptOffset.x,
        ptOffset.y,
        SRCCOPY);

    SYSTEMTIME lt;
    GetLocalTime(&lt);

    WCHAR filePath[MAX_PATH] = {};
    StringCchPrintfW(filePath,
        _countof(filePath),
        L"%s\\ScreenSnip Window %02d-%02d-%02d",
        directory,
        lt.wMonth, lt.wDay, lt.wYear);

    CreateSaveFile(filePath, hbitmapResize);

    SnipSavedAlert(directory);

    SelectObject(hdcSrc, hbmpSave);
    DeleteDC(hdcSrc);
    
    SelectObject(hdcResize, hbmpSave2);
    DeleteDC(hdcResize);

    DeleteDC(hdcWindow);

    return S_OK;
}

int g_count = 0;

HRESULT CaptureZone(PCWSTR directory, RECT captureRect)
{   
    g_count++;

    if (dbg >= 4)
    {
        WCHAR string[250];
        StringCchPrintfW(
            string,
             _countof(string), 
            L"The current count is %d", g_count);
        MessageBox(NULL, string, L"Zone Capture Count", MB_OK);
    }
    
    if (g_count >= 2)
    {
        if (dbg >= 2)
        {
            MessageBox(NULL, L"count eccceded", L"Zone Capture Count Alert", MB_OK);
        }

        g_count = 0;
        return S_FALSE;
    }

    HBITMAP hbitmap;
    
    HDC hdcDesktop = GetWindowDC(GetDesktopWindow());
    HDC hdcScreen = GetDC(NULL);
    HDC hdcDest = CreateCompatibleDC(hdcDesktop);

    hbitmap = CreateCompatibleBitmap(
        hdcDesktop,
        (captureRect.right - captureRect.left),
        (captureRect.bottom - captureRect.top));
    HGDIOBJ hbmpSave = SelectObject(hdcDest, hbitmap);

    BitBlt(
        hdcDest,
        0,
        0,
        (captureRect.right - captureRect.left),
        (captureRect.bottom - captureRect.top),
        hdcScreen,
        captureRect.left,
        captureRect.top,
        SRCCOPY);

    SYSTEMTIME lt;
    GetLocalTime(&lt);

    WCHAR filePath[MAX_PATH] = {};
    StringCchPrintfW(filePath,
        _countof(filePath),
        L"%s\\ScreenSnip Zone %02d-%02d-%02d",
        directory,
        lt.wMonth, lt.wDay, lt.wYear);

    CreateSaveFile(filePath, hbitmap);

    SnipSavedAlert(filePath);

    SelectObject(hdcDest, hbmpSave);
    DeleteDC(hdcDest);
    
    DeleteDC(hdcScreen);

    DeleteDC(hdcDesktop);

    g_count = 0;

    return S_OK;
}

HRESULT CreateBitmapFromResource(
    _In_ IWICImagingFactory* pIWICFactory,
    _In_ int resourceId,
    _Out_ HBITMAP* phBitmap)
{
    HRESULT hr = S_OK;

    // WIC interface pointers.
    IWICStream* pIWICStream = nullptr;
    IWICBitmapDecoder* pDecoder = nullptr;
    IWICBitmapFrameDecode* pFirstFrame = nullptr;

    // Resource management.
    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource in the application's executable.
    imageResHandle = FindResource(
        NULL,             
        MAKEINTRESOURCE(resourceId),
        L"Image");

    hr = (imageResHandle ? S_OK : E_FAIL);

    // Load the resource to the HGLOBAL.
    if (SUCCEEDED(hr))
    {
        imageResDataHandle = LoadResource(NULL, imageResHandle);
        hr = (imageResDataHandle ? S_OK : E_FAIL);
    }

    // Lock the resource to retrieve memory pointer.
    if (SUCCEEDED(hr))
    {
        pImageFile = LockResource(imageResDataHandle);
        hr = (pImageFile ? S_OK : E_FAIL);
    }

    // Calculate the size.
    if (SUCCEEDED(hr))
    {
        imageFileSize = SizeofResource(NULL, imageResHandle);
        hr = (imageFileSize ? S_OK : E_FAIL);
    }

    if (SUCCEEDED(hr))
    {
        hr = pIWICFactory->CreateStream(&pIWICStream);
    }

    // Initialize the stream with the memory pointer and size.
    if (SUCCEEDED(hr))
    {
        hr = pIWICStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize);
    }

    // Create a decoder for the stream.
    if (SUCCEEDED(hr))
    {
        hr = pIWICFactory->CreateDecoderFromStream(
            pIWICStream,
            NULL, 
            WICDecodeMetadataCacheOnLoad,
            &pDecoder);
    }

    // Retrieve the initial frame.
    if (SUCCEEDED(hr))
    {
        hr = pDecoder->GetFrame(0, &pFirstFrame);
    }

    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    RETURN_IF_FAILED(pDecoder->GetFrame(0, &pFirstFrame));

    // Convert the image format to 32bppPBGRA
    RETURN_IF_FAILED(pIWICFactory->CreateFormatConverter(&pConverter));

    RETURN_IF_FAILED(pConverter->Initialize(
            pFirstFrame,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut));

    UINT width = 0;
    UINT height = 0;
    RETURN_IF_FAILED(pConverter->GetSize(&width, &height));

    UINT stride = ALIGN_SIZE(width * 4, sizeof(DWORD));
    UINT cbSize = stride * height;
    BYTE* pMemory = new BYTE[cbSize];

    WICRect bitmapRect = {0, 0, static_cast<INT>(width), static_cast<INT>(height)};
    RETURN_IF_FAILED(pConverter->CopyPixels(
        &bitmapRect,
        stride,
        cbSize,
        pMemory));

    *phBitmap = CreateBitmap(
        width,
        height,
        1,
        32,
        pMemory);

    if (*phBitmap == NULL)
    {
        return E_OUTOFMEMORY;
    } 

    delete(pMemory);


    return hr;
}
