//
// globals.h
//
// Global variable declarations.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <windows.h>
#include <ole2.h>
#include <olectl.h>
#include <assert.h>
#include "msctf.h"

void DllAddRef();
void DllRelease();

void InsertTextAtSelection(TfEditCookie ec, ITfContext *pContext, const WCHAR *pchText, ULONG cchText);

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

#define CASE_LANGID_EN    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)
#define CASE_LANGID_AM    MAKELANGID(LANG_AMHARIC, SUBLANG_AMHARIC_ETHIOPIA)

#define CASE_ICON_INDEX  0

#define SafeRelease(punk)       \
{                               \
    if ((punk) != NULL)         \
    {                           \
        (punk)->Release();      \
    }                           \
}                   

#define SafeReleaseClear(punk)  \
{                               \
    if ((punk) != NULL)         \
    {                           \
        (punk)->Release();      \
        (punk) = NULL;          \
    }                           \
}                   

extern HINSTANCE g_hInst;

extern LONG g_cRefDll;

extern CRITICAL_SECTION g_cs;

extern const CLSID c_clsidCaseTextService;

extern const GUID c_guidCaseProfile_EN;
extern const GUID c_guidCaseProfile_AM;

#endif // GLOBALS_H
