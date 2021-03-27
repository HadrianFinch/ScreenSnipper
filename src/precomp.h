#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib") 
#pragma comment(lib,"ole32.lib") 

#define RETURN_IF_FAILED(hr) { if(FAILED(hr)) return hr; }

#define ALIGN_SIZE( sizeToAlign, PowerOfTwo )       \
        (((sizeToAlign) + (PowerOfTwo) - 1) & ~((PowerOfTwo) - 1))

#define UNICODE

#include <windows.h>
#include "decode.h"
#include "calander.h"
#include "button.h"

__inline void Assert(
    _In_ bool value)
{
    if (value != false)
    {
        return;
    }
    else
    {
        __debugbreak();
    }
}

extern HINSTANCE g_hInstance;
extern IWICImagingFactory* g_pWICFactory;

BOOL LayerWindow(
    _In_ HWND hwnd,
    _In_ HBITMAP hBitmap,
    _In_ SIZE windowSize,
    _In_ POINT ptWinPos);
