#include "ElectronJs.h"
#include "HookStuff.h"
#include <atlbase.h>
#include <atlstr.h>
#include <vector>
#include <cstddef>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "version.lib")

CString g_strExeVersion;
CString g_szExeName;

static HOOK_INFO hkInfo;
static const ptrdiff_t g_offset_DidFinishNavigation = /*0x2B3340;*/ 0x2B4880;
static const ptrdiff_t g_offset_MainFrame = /*0x439DD80;*/0x437B740;
static const ptrdiff_t g_offset_ExecuteJavaScript = /*0x2D9970;*/0x2DAEB0;
// static const ptrdiff_t g_offset_GetIsolate = 0x3188A0;
static const ptrdiff_t g_offset_FromRenderFrameHost = /*0x2D9070;*/0x2DA5B0;

// dev tool
BOOL hookCreateProcess()
{
    return TRUE;
}

BOOL GetFileVersion(const CString &strFilePath, CString &strVersion)
{
	typedef struct _LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	}LANGANDCODEPAGE;

	BOOL bRet = FALSE;

	DWORD dwHandle = 0;
	DWORD dwSize = GetFileVersionInfoSize(strFilePath, &dwHandle);
	if (dwSize == 0)
		return bRet;

	CHAR *pInfoBuf = new CHAR[dwSize];
	if (!pInfoBuf)
		return bRet;

	do
	{
		if (!GetFileVersionInfo(strFilePath, dwHandle, dwSize, pInfoBuf))
			break;

		PVOID pInfo = NULL;
		UINT nLen = 0;
		BOOL bQuery = VerQueryValue(pInfoBuf, _T("\\"), &pInfo, &nLen);
		if (!bQuery || sizeof(VS_FIXEDFILEINFO) != nLen)
			break;

		VS_FIXEDFILEINFO *pFixInfo = (VS_FIXEDFILEINFO *)pInfo;

		int nMajorVersion = HIWORD(pFixInfo->dwFileVersionMS);
		int nMinorVersion = LOWORD(pFixInfo->dwFileVersionMS);
		int nBuildVersion = HIWORD(pFixInfo->dwFileVersionLS);
		int nReviVersion = LOWORD(pFixInfo->dwFileVersionLS);

		TCHAR szVerBuff[100] = { 0 };
		wsprintf(szVerBuff, _T("%d.%d.%d.%d"), nMajorVersion, nMinorVersion, nBuildVersion, nReviVersion);

		strVersion = szVerBuff;
		bRet = TRUE;
	} while (FALSE);

	if (pInfoBuf)
		delete[] pInfoBuf;

	return bRet;
}

BOOL getExeInfo()
{
    TCHAR szPath[MAX_PATH] = {0};
    GetModuleFileName(NULL, szPath, MAX_PATH);
    CString szExePath = szPath;
    int nPos = szExePath.ReverseFind(_T('\\'));
    if (nPos == -1)
        return FALSE;
    g_szExeName = szExePath.Mid(nPos + 1);
    // if (g_szExeName.CompareNoCase(_T("qq.exe")) != 0)
    //     return FALSE;
    return GetFileVersion(szExePath, g_strExeVersion);
}

struct RenderFrameHost
{

};

struct WebFrameMain
{
    typedef WebFrameMain* (*PFN_FromRenderFrameHost)(RenderFrameHost *render_frame_host);    
    //   v8::Local<v8::Promise> ExecuteJavaScript(gin::Arguments* args, const std::u16string& code);
    typedef void* (WebFrameMain::*PNF_ExecuteJavaScript)(void *ret, void *args, void *code);
};

struct WebContents
{
    //typedef void (WebContents::*PFN_DidFinishNavigation)(void* navigation_handle); //override from WebContentsObserver
    typedef RenderFrameHost* (WebContents::*PFN_MainFrame)();
    //content::RenderFrameHost* MainFrame();
};

void proxyDidFinishNavigation(WebContents *pThis, void *navigation_handle)
{
    WebContents::PFN_MainFrame pfnMainFrame = nullptr;
    *(void **)&pfnMainFrame = (PVOID)((intptr_t)GetModuleHandle(NULL) + g_offset_MainFrame);
    
    WebFrameMain::PNF_ExecuteJavaScript pfnExecuteJavaScript = nullptr;
    *(void **)&pfnExecuteJavaScript = (PVOID)((intptr_t)GetModuleHandle(NULL) + g_offset_ExecuteJavaScript);
    
    WebFrameMain::PFN_FromRenderFrameHost pfnFromRenderFrameHost = nullptr;
    *(void **)&pfnFromRenderFrameHost = (PVOID)((intptr_t)GetModuleHandle(NULL) + g_offset_FromRenderFrameHost);

    struct Scope
    {
        //??0HandleScope@v8@@QEAA@PEAVIsolate@1@@Z
        Scope(void * isolate)
        {
            memset(temp, 0, sizeof(temp));
            //public: __cdecl v8::HandleScope::HandleScope(class v8::Isolate *)
            typedef void* (Scope::*PFN_HandleScope_C)(void *);
            PFN_HandleScope_C pfn = nullptr;
            *(void **)&pfn = GetProcAddress(GetModuleHandle(NULL), "??0HandleScope@v8@@QEAA@PEAVIsolate@1@@Z");
            (this->*pfn)(isolate);
        }
        //??1HandleScope@v8@@QEAA@XZ
        ~Scope()
        {
            //public: __cdecl v8::HandleScope::~HandleScope(void)
            typedef void (Scope::*PFN_HandleScope_D)();
            PFN_HandleScope_D pfn = nullptr;
            *(void **)&pfn = GetProcAddress(GetModuleHandle(NULL), "??1HandleScope@v8@@QEAA@XZ");
            (this->*pfn)();
        }
        char temp[100];
    };
     //static JavascriptEnvironment::GetIsolate();
    // typedef void* (*PFN_GetIsolate)();
    // PFN_GetIsolate pfnGetIsolate = (PFN_GetIsolate)((intptr_t)GetModuleHandle(NULL) + g_offset_GetIsolate);
    // void *isolate = pfnGetIsolate();

    //?GetCurrent@Isolate@v8@@SAPEAV12@XZ
    //public: static class v8::Isolate * __cdecl v8::Isolate::GetCurrent(void)
    typedef void * (*PFN_GetCurrent)();
    PFN_GetCurrent pfnGetIsolate = nullptr;
    *(void **)&pfnGetIsolate = GetProcAddress(GetModuleHandle(NULL), "?GetCurrent@Isolate@v8@@SAPEAV12@XZ");
    void *isolate = pfnGetIsolate();

    Scope scope(isolate);
    *(void **)&pThis = *(void **)((intptr_t)pThis + 0x18);
    RenderFrameHost *frameHost = (pThis->*pfnMainFrame)();
    WebFrameMain *webFrameMain = pfnFromRenderFrameHost(frameHost);
    struct 
    {
        size_t k1;
        size_t len;
        const wchar_t *js = L"window.alert('fasdf');window.asdf=123;";
    }jss;
    jss.k1 = 0x11;
    jss.len = wcslen(jss.js);
    char buffer[1000] = {0};
    struct 
    {
        void *ioslate;
        void *stuff;
        size_t flags;//offset 0x15
    }args;
    args.ioslate = isolate;
    args.stuff = nullptr;
    args.flags = -1;
    
    (webFrameMain->*pfnExecuteJavaScript)(&buffer, &args, &jss);
}

DWORD WINAPI installHookProc(LPVOID lpThreadParameter)
{
    PVOID pfnDidFinishNavigation = (PVOID)((intptr_t)GetModuleHandle(NULL) + g_offset_DidFinishNavigation);
    HookByAddress(pfnDidFinishNavigation, &hkInfo, proxyDidFinishNavigation,
        FALSE, TRUE);
    return 0;
}

BOOL initDLL()
{
    DWORD dwPid = 0;
    HANDLE hThread = CreateThread(NULL, 0, &installHookProc, NULL, 0, &dwPid);
    if (hThread)
    {
        CloseHandle(hThread);
    }

    return TRUE;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
#ifndef NDEBUG
        MessageBox(NULL, _T("ElectronJs"), NULL, MB_OK);
#endif
        if (getExeInfo() && g_strExeVersion == "9.9.9.22741")
        {
            initDLL();
        }
        break;
    }
    return TRUE;
}
