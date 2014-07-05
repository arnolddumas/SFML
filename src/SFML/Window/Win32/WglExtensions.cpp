#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <SFML/Window/Win32/WglExtensions.hpp>

#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#endif

static int TestPointer(const PROC pTest)
{
    ptrdiff_t iTest;
    if(!pTest) return 0;
    iTest = (ptrdiff_t)pTest;

    if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;

    return 1;
}

static PROC WinGetProcAddress(const char *name)
{
    HMODULE glMod = NULL;
    PROC pFunc = wglGetProcAddress((LPCSTR)name);
    if(TestPointer(pFunc))
    {
        return pFunc;
    }
    glMod = GetModuleHandleA("OpenGL32.dll");
    return (PROC)GetProcAddress(glMod, (LPCSTR)name);
}

#define IntGetProcAddress(name) WinGetProcAddress(name)

int sfwgl_ext_EXT_swap_control = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_multisample = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_pixel_format = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_create_context = sfwgl_LOAD_FAILED;
int sfwgl_ext_ARB_create_context_profile = sfwgl_LOAD_FAILED;

int (CODEGEN_FUNCPTR *sf_ptrc_wglGetSwapIntervalEXT)() = NULL;
BOOL (CODEGEN_FUNCPTR *sf_ptrc_wglSwapIntervalEXT)(int) = NULL;

static int Load_EXT_swap_control()
{
    int numFailed = 0;
    sf_ptrc_wglGetSwapIntervalEXT = (int (CODEGEN_FUNCPTR *)())IntGetProcAddress("wglGetSwapIntervalEXT");
    if(!sf_ptrc_wglGetSwapIntervalEXT) numFailed++;
    sf_ptrc_wglSwapIntervalEXT = (BOOL (CODEGEN_FUNCPTR *)(int))IntGetProcAddress("wglSwapIntervalEXT");
    if(!sf_ptrc_wglSwapIntervalEXT) numFailed++;
    return numFailed;
}

BOOL (CODEGEN_FUNCPTR *sf_ptrc_wglChoosePixelFormatARB)(HDC, const int *, const FLOAT *, UINT, int *, UINT *) = NULL;
BOOL (CODEGEN_FUNCPTR *sf_ptrc_wglGetPixelFormatAttribfvARB)(HDC, int, int, UINT, const int *, FLOAT *) = NULL;
BOOL (CODEGEN_FUNCPTR *sf_ptrc_wglGetPixelFormatAttribivARB)(HDC, int, int, UINT, const int *, int *) = NULL;

static int Load_ARB_pixel_format()
{
    int numFailed = 0;
    sf_ptrc_wglChoosePixelFormatARB = (BOOL (CODEGEN_FUNCPTR *)(HDC, const int *, const FLOAT *, UINT, int *, UINT *))IntGetProcAddress("wglChoosePixelFormatARB");
    if(!sf_ptrc_wglChoosePixelFormatARB) numFailed++;
    sf_ptrc_wglGetPixelFormatAttribfvARB = (BOOL (CODEGEN_FUNCPTR *)(HDC, int, int, UINT, const int *, FLOAT *))IntGetProcAddress("wglGetPixelFormatAttribfvARB");
    if(!sf_ptrc_wglGetPixelFormatAttribfvARB) numFailed++;
    sf_ptrc_wglGetPixelFormatAttribivARB = (BOOL (CODEGEN_FUNCPTR *)(HDC, int, int, UINT, const int *, int *))IntGetProcAddress("wglGetPixelFormatAttribivARB");
    if(!sf_ptrc_wglGetPixelFormatAttribivARB) numFailed++;
    return numFailed;
}

HGLRC (CODEGEN_FUNCPTR *sf_ptrc_wglCreateContextAttribsARB)(HDC, HGLRC, const int *) = NULL;

static int Load_ARB_create_context()
{
    int numFailed = 0;
    sf_ptrc_wglCreateContextAttribsARB = (HGLRC (CODEGEN_FUNCPTR *)(HDC, HGLRC, const int *))IntGetProcAddress("wglCreateContextAttribsARB");
    if(!sf_ptrc_wglCreateContextAttribsARB) numFailed++;
    return numFailed;
}


static const char * (CODEGEN_FUNCPTR *sf_ptrc_wglGetExtensionsStringARB)(HDC) = NULL;

typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct sfwgl_StrToExtMap_s
{
    const char *extensionName;
    int *extensionVariable;
    PFN_LOADFUNCPOINTERS LoadExtension;
} sfwgl_StrToExtMap;

static sfwgl_StrToExtMap ExtensionMap[5] = {
    {"WGL_EXT_swap_control", &sfwgl_ext_EXT_swap_control, Load_EXT_swap_control},
    {"WGL_ARB_multisample", &sfwgl_ext_ARB_multisample, NULL},
    {"WGL_ARB_pixel_format", &sfwgl_ext_ARB_pixel_format, Load_ARB_pixel_format},
    {"WGL_ARB_create_context", &sfwgl_ext_ARB_create_context, Load_ARB_create_context},
    {"WGL_ARB_create_context_profile", &sfwgl_ext_ARB_create_context_profile, NULL},
};

static int g_extensionMapSize = 5;

static sfwgl_StrToExtMap *FindExtEntry(const char *extensionName)
{
    int loop;
    sfwgl_StrToExtMap *currLoc = ExtensionMap;
    for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
    {
        if(strcmp(extensionName, currLoc->extensionName) == 0)
            return currLoc;
    }

    return NULL;
}

static void ClearExtensionVars()
{
    sfwgl_ext_EXT_swap_control = sfwgl_LOAD_FAILED;
    sfwgl_ext_ARB_multisample = sfwgl_LOAD_FAILED;
    sfwgl_ext_ARB_pixel_format = sfwgl_LOAD_FAILED;
    sfwgl_ext_ARB_create_context = sfwgl_LOAD_FAILED;
    sfwgl_ext_ARB_create_context_profile = sfwgl_LOAD_FAILED;
}


static void LoadExtByName(const char *extensionName)
{
    sfwgl_StrToExtMap *entry = NULL;
    entry = FindExtEntry(extensionName);
    if(entry)
    {
        if(entry->LoadExtension)
        {
            int numFailed = entry->LoadExtension();
            if(numFailed == 0)
            {
                *(entry->extensionVariable) = sfwgl_LOAD_SUCCEEDED;
            }
            else
            {
                *(entry->extensionVariable) = sfwgl_LOAD_SUCCEEDED + numFailed;
            }
        }
        else
        {
            *(entry->extensionVariable) = sfwgl_LOAD_SUCCEEDED;
        }
    }
}


static void ProcExtsFromExtString(const char *strExtList)
{
    size_t iExtListLen = strlen(strExtList);
    const char *strExtListEnd = strExtList + iExtListLen;
    const char *strCurrPos = strExtList;
    char strWorkBuff[256];

    while(*strCurrPos)
    {
        /*Get the extension at our position.*/
        int iStrLen = 0;
        const char *strEndStr = strchr(strCurrPos, ' ');
        int iStop = 0;
        if(strEndStr == NULL)
        {
            strEndStr = strExtListEnd;
            iStop = 1;
        }

        iStrLen = (int)((ptrdiff_t)strEndStr - (ptrdiff_t)strCurrPos);

        if(iStrLen > 255)
            return;

        strncpy(strWorkBuff, strCurrPos, iStrLen);
        strWorkBuff[iStrLen] = '\0';

        LoadExtByName(strWorkBuff);

        strCurrPos = strEndStr + 1;
        if(iStop) break;
    }
}

int sfwgl_LoadFunctions(HDC hdc)
{
    ClearExtensionVars();

    sf_ptrc_wglGetExtensionsStringARB = (const char * (CODEGEN_FUNCPTR *)(HDC))IntGetProcAddress("wglGetExtensionsStringARB");
    if(!sf_ptrc_wglGetExtensionsStringARB) return sfwgl_LOAD_FAILED;

    ProcExtsFromExtString((const char *)sf_ptrc_wglGetExtensionsStringARB(hdc));
    return sfwgl_LOAD_SUCCEEDED;
}
