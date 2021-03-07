
#include "precomp.h"

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
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