#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib") 
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"Shell32.lib")

#define RETURN_IF_FAILED(hr) { if(FAILED(hr)) return hr; }

#define ALIGN_SIZE( sizeToAlign, PowerOfTwo )       \
        (((sizeToAlign) + (PowerOfTwo) - 1) & ~((PowerOfTwo) - 1))

#define UNICODE

#include <windows.h>
#include <windowsx.h>
#include <pathcch.h>
#include <shellapi.h>
#include "decode.h"
#include "popup.h"
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
extern CPopup* pOptionsMenu;
extern bool popupActive;
extern HWND g_highlightHwnd;

BOOL LayerWindow(
    _In_ HWND hwnd,
    _In_ HBITMAP hBitmap,
    _In_ SIZE windowSize,
    _In_ POINT ptWinPos);
void ShowOptionsPopup(HWND menuBarHwnd);
void HideOptionsPopup(HWND menuBarHwnd);
HWND GetTopLevelWindowFromPoint(POINT pt);
