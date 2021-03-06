#include "precomp.h"

enum SaveFormat
{
    SaveFormatDefault = 1,
    SaveFormatBmp = 0,
    SaveFormatJpg = 1,
    SaveFormatPng = 2,
    SaveFormatTiff = 3,
    SaveFormatGif = 4
};

REFGUID WicGuidFromSaveFormat(SaveFormat format)
{
    switch (format)
    {
        case SaveFormatBmp:
        {
            return GUID_ContainerFormatBmp;
        }

        case SaveFormatPng:
        {
            return GUID_ContainerFormatPng;
        }

        case SaveFormatGif:
        {
            return GUID_ContainerFormatGif;
        }

        case SaveFormatTiff:
        {
            return GUID_ContainerFormatTiff;
        }

        case SaveFormatJpg:
        {
            return GUID_ContainerFormatJpeg;
        }

        default:
        {
            return GUID_ContainerFormatJpeg;
        }
    }
}

bool FileExists(LPTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);
    bool b = !!(dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

    return b;
}

HRESULT CreateSaveFile(LPTSTR pszFile, HBITMAP hBMP) 
{
    int i = 1;
    WCHAR filePathWithExtension[MAX_PATH] = {};
    
    StringCchPrintfW(filePathWithExtension,
        _countof(filePathWithExtension),
        L"%s.jpg",
        pszFile);

    while (FileExists(filePathWithExtension))
    {
        StringCchPrintfW(filePathWithExtension,
        _countof(filePathWithExtension),
        L"%s %d.jpg",
        pszFile,
        i);

        i++;
    }
    *pszFile = *filePathWithExtension;

    IWICImagingFactory2* pFactory = nullptr;
    IWICBitmap* pBitmap = nullptr;
    IWICStream* pStream = nullptr;
    IWICBitmapEncoder* pEncoder = nullptr;
    IWICBitmapFrameEncode* pFrame = nullptr;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Create the WIC factory
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory2),
            reinterpret_cast<void**>(&pFactory));
    }

    BITMAP bm;
    GetObject(hBMP, sizeof(bm), &bm);
    SIZE size = {bm.bmWidth, bm.bmHeight};


    // Create an IWICBitmap from our hBmp
    if (SUCCEEDED(hr))
    {
        hr = pFactory->CreateBitmapFromHBITMAP(
            hBMP,
            NULL,
            WICBitmapUsePremultipliedAlpha,
            &pBitmap);
    }

    // Create the destination file stream and initialize from the filename
    if (SUCCEEDED(hr))
    {
        hr = pFactory->CreateStream(&pStream);
    }

    if (SUCCEEDED(hr))
    {
        hr = pStream->InitializeFromFilename(filePathWithExtension, GENERIC_WRITE);
    }

    // Create our encoder
    if (SUCCEEDED(hr))
    {
        hr = pFactory->CreateEncoder(
            WicGuidFromSaveFormat(SaveFormatJpg),
            &GUID_VendorMicrosoftBuiltIn,
            &pEncoder);
    }

    // Initialize the decoder to write to the file
    if (SUCCEEDED(hr))
    {
        hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
    }

    // Create a new frame and initialize
    if (SUCCEEDED(hr))
    {
        hr = pEncoder->CreateNewFrame(&pFrame, nullptr);
    }

    if (SUCCEEDED(hr))
    {
        hr = pFrame->Initialize(nullptr /*pIEncoderOptions*/);
    }

    // Set the size of the output
    if (SUCCEEDED(hr))
    {
        hr = pFrame->SetSize(size.cx, size.cy);
    }

    // Write our bitmap in the frame
    if (SUCCEEDED(hr))
    {
        // WICRect rc = { 0, 0, size.cx, size.cy };

        hr = pFrame->WriteSource(pBitmap, nullptr);
    }

    // Commit everything
    if (SUCCEEDED(hr))
    {
        hr = pFrame->Commit();
    }

    if (SUCCEEDED(hr))
    {
        hr = pEncoder->Commit();
    }

    // Cleanup
    if (pFrame != nullptr)
    {
        pFrame->Release();
    }

    if (pEncoder != nullptr)
    {
        pEncoder->Commit();
    }

    if (pStream != nullptr)
    {
        pStream->Commit(STGC_DEFAULT);
        
        pStream->Release();
    }

    if (pBitmap != nullptr)
    {
        pBitmap->Release();
    }

    if (pFactory != nullptr)
    {
        pFactory->Release();
    }

    return hr;
}
