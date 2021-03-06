#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib") 
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Dwmapi.lib")
// #pragma comment(lib,"Comdlg32.lib")
#pragma comment(lib,"Advapi32.lib")

#define RETURN_IF_FAILED(hr) { if(FAILED(hr)) return hr; }

#define ALIGN_SIZE( sizeToAlign, PowerOfTwo )       \
        (((sizeToAlign) + (PowerOfTwo) - 1) & ~((PowerOfTwo) - 1))

#define UNICODE

#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <wincodec.h>   // for IWICImagingFactory
#include <pathcch.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <commdlg.h>
#include <shobjidl.h>

#include "resources.h"

#include "decode.h"
#include "encode.h"
#include "popup.h"
#include "button.h"
#include "settings.h"


extern int dbg;

__inline void Assert(
    _In_ bool value)
{
    if (dbg > 0)
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
extern bool popupActive;
extern HWND g_highlightHwnd;
extern HWND g_zoneSnipHwnd;
extern bool g_zoneActive;
extern RECT g_snipRect;
extern bool g_settingsOpen;

extern WCHAR g_szFavoriteFolder1[MAX_PATH];
extern WCHAR g_szFavoriteFolder2[MAX_PATH];
extern WCHAR g_szFavoriteFolder3[MAX_PATH];
extern WCHAR g_currentSnipPath[MAX_PATH];



BOOL LayerWindow(
    _In_ HWND hwnd,
    _In_ HBITMAP hBitmap,
    _In_ SIZE windowSize,
    _In_ POINT ptWinPos);
CPopup* ShowOptionsPopup(HWND menuBarHwnd);
void HideOptionsPopup(CPopup* pOptionsPopup);

// Recording Includes
