#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib") 
#pragma comment(lib,"ole32.lib") 

#define RETURN_IF_FAILED(hr) { if(FAILED(hr)) return hr; }

#define ALIGN_SIZE( sizeToAlign, PowerOfTwo )       \
        (((sizeToAlign) + (PowerOfTwo) - 1) & ~((PowerOfTwo) - 1))

#include <windows.h>
#include "decode.h"
#include "calander.h"
#include "button.h"