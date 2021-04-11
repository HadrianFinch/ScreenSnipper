#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib") 
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Dwmapi.lib")
#pragma comment(lib,"Comdlg32.lib")

#define RETURN_IF_FAILED(hr) { if(FAILED(hr)) return hr; }

#define ALIGN_SIZE( sizeToAlign, PowerOfTwo )       \
        (((sizeToAlign) + (PowerOfTwo) - 1) & ~((PowerOfTwo) - 1))

#define UNICODE

#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <pathcch.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <commdlg.h>

#include "decode.h"
#include "popup.h"
#include "button.h"
#include "settings.h"

__inline void Assert(
    _In_ bool value)
{
    if (false)
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
}

extern HINSTANCE g_hInstance;
extern IWICImagingFactory* g_pWICFactory;
extern CPopup* pOptionsMenu;
extern bool popupActive;
extern HWND g_highlightHwnd;
extern HWND g_zoneSnipHwnd;
extern bool g_zoneActive;
extern RECT g_snipRect;
extern bool g_settingsOpen;


BOOL LayerWindow(
    _In_ HWND hwnd,
    _In_ HBITMAP hBitmap,
    _In_ SIZE windowSize,
    _In_ POINT ptWinPos);
void ShowOptionsPopup(HWND menuBarHwnd);
void HideOptionsPopup();
