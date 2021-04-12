
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
        L"images\\container.png", 
        size,
        pt,
        &pContainer);
    Assert(SUCCEEDED(hr));
    CButton* pAlert = nullptr;
    hr = CAlertButton::Create(
        pContainer->m_hwnd,
        L"File Saved Popup",
        L"images\\snipSavedAlert.png",
        size,
        {-1, -1},
        &pAlert);
    
    auto pAlertBtn = static_cast<CAlertButton*>(pAlert);
    StringCchCopy(
        pAlertBtn->m_filePath, 
        _countof(pAlertBtn->m_filePath),
        filePath);
    
    pAlert->m_pHoverImageFileName = L"images\\snipSavedAlertHover.png";
    pAlert->m_HoverPt = {-1, -1};
    pAlert->m_HoverSize = size;
    
}

void PathNotSetupError()
{
    RECT desktopClientRect;
    HWND desktopHwnd = GetDesktopWindow();
    GetClientRect(desktopHwnd, &desktopClientRect);

    SIZE size = {200, 45};
    POINT pt = {(((desktopClientRect.right - desktopClientRect.left) / 2) - 100), (25)};

    CPopup* pContainer = nullptr;
    CPopup::Create(
        L"File Save Container",
        L"images\\container.png", 
        size,
        pt,
        &pContainer);

    CButton* pAlert = nullptr;
    HRESULT hr = CAlertButton::Create(
        pContainer->m_hwnd,
        L"File Saved Popup",
        L"images\\snipSavedAlert.png",
        size,
        {-1, -1},
        &pAlert);
        
    pAlert->m_pHoverImageFileName = L"images\\snipSavedAlertHover.png";
    pAlert->m_HoverPt = {-1, -1};
    pAlert->m_HoverSize = size;
    Assert(SUCCEEDED(hr));
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height.  
    Assert(GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)); 

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

     if (cClrBits < 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
 } 

void CreateBMPFile(LPTSTR pszFile, HBITMAP hBMP) 
{ 
    HANDLE hf;                  // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE *hp;                   // byte pointer  
    DWORD dwTmp;     
    PBITMAPINFO pbi;
    HDC hDC;

    hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
    SelectObject(hDC, hBMP);

    pbi = CreateBitmapInfoStruct(hBMP);

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    Assert(lpBits) ;

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    Assert(GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
        DIB_RGB_COLORS));

    // Create the .BMP file.  
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    Assert(hf != INVALID_HANDLE_VALUE) ;

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    Assert(WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)); 

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    Assert(WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, ( NULL)));

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    Assert(WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)); 

    // Close the .BMP file.  
     Assert(CloseHandle(hf)); 

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
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

HRESULT CreateBitmapFromFile(
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
        L"%s\\ScreenSnip Screen %02d-%02d-%02d.jpg",
        directory,
        lt.wMonth, lt.wDay, lt.wYear);

    CreateBMPFile(filePath, hbitmap);

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
        L"%s\\ScreenSnip Window %02d-%02d-%02d.jpg",
        directory,
        lt.wMonth, lt.wDay, lt.wYear);

    CreateBMPFile(filePath, hbitmapResize);

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
        L"%s\\ScreenSnip Zone %02d-%02d-%02d.jpg",
        directory,
        lt.wMonth, lt.wDay, lt.wYear);

    CreateBMPFile(filePath, hbitmap);

    SnipSavedAlert(directory);

    SelectObject(hdcDest, hbmpSave);
    DeleteDC(hdcDest);
    
    DeleteDC(hdcScreen);

    DeleteDC(hdcDesktop);

    g_count = 0;

    return S_OK;
}
