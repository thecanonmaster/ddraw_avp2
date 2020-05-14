#include "StdAfx.h"
#include "numfont.h"

BOOL g_bNoCompatWarning = FALSE;
BOOL g_bDontShutdownRenderer = FALSE;
BOOL g_bShowFPS = FALSE;
BOOL g_bDrawFPS = TRUE;
DWORD g_dwWidth = 1024;
DWORD g_dwHeight = 768;
BOOL g_bWindowedSet = FALSE;
DWORD g_bWindowed = 0;
float g_fMaxFPS = 60;
BOOL g_bCameraFOVFix = FALSE;
BOOL g_bCameraFOVFix2 = FALSE;
BOOL g_bViewModelFOVFix = FALSE;
BOOL g_bSolidDrawingFix = FALSE;
BOOL g_bIntelHDFix = FALSE;
BOOL g_bRadeon5700Fix = FALSE;
BOOL g_bLightLoadFix = FALSE;
BOOL g_bMiscCCFix = FALSE;
BOOL g_bRawMouseInputFix = FALSE;
BOOL g_bRawMouseInputFix2 = FALSE;
LPDIRECTDRAWSURFACE7 g_ddsBackBuffer = NULL;
LPDIRECT3DDEVICE7 g_d3dDevice = NULL;
LPDIRECTDRAW7 g_ddMainDDraw = NULL;
HWND g_hWindowHandle = NULL;
BOOL g_bWindowHooked = FALSE;
CClientMgrBase* g_pClientMgr = NULL;
ILTClient* g_pLTClient = NULL;
float g_fLastFontListUpdate = 0.0f;

float g_fRMIScaleGlobal = 0.001f;
float g_fRMIScaleY = 1.1f;
LONG g_lRMILastX = 0;
LONG g_lRMILastY = 0;
//DWORD g_hMouseSensitivity = 0;

FontList g_FontList;
DWORD g_hWhitePixelSurface = NULL;
//DWORD g_hIntroductionSurface = NULL;
DWORD g_hFrameRateFontSurface[10] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
int g_nLastFrameRate = 0;
//SolidSurfaceList g_SolidSurfaceList;

DWORD g_hFontSearch;
char* g_szStringSearch;
DWORD g_hSurfaceSearch;
DWORD g_hColorSearch;

void (__cdecl *ILTCSBase_CPrint)(ILTCSBase* pBase, char *pMsg, ...);
DWORD (__stdcall *ILTCSBase_CreateString)(char *pString);
void (__stdcall *ILTCSBase_FreeString)(DWORD hString);
char* (__stdcall *ILTCSBase_GetStringData)(DWORD hString);
float (__stdcall *ILTCSBase_GetVarValueFloat)(DWORD hVar);
char* (__stdcall *ILTCSBase_GetVarValueString)(DWORD hVar);
float (__fastcall *ILTCSBase_GetTime)(ILTCSBase* pBase);
float (__fastcall *ILTCSBase_GetFrameTime)(ILTCSBase* pBase);

void (__fastcall *IClientShell_Update)(void* pShell);

/*bool SolidSurfaceList_HandleEqualFn(const SolidSurface* pItem)
{
	return (pItem->m_hColor == g_hColorSearch);
}

SolidSurface* SolidSurfaceList_FindItem(DWORD hColor)
{
	g_hColorSearch = hColor;
	SolidSurfaceList::iterator iter = std::find_if(g_SolidSurfaceList.begin(), g_SolidSurfaceList.end(), SolidSurfaceList_HandleEqualFn);
	
	if (iter != g_SolidSurfaceList.end())
		return (*iter);
	
	return NULL;
}*/

bool FontStringList_HandleEqualFn_ByString(const FontString* pItem)
{
	return (strcmp(pItem->m_szString, g_szStringSearch) == 0);
}

bool FontStringList_HandleEqualFn_BySurface(const FontString* pItem)
{
	return (pItem->m_hSurface == g_hSurfaceSearch);
}

bool FontList_HandleEqualFn(const Font* pItem)
{
	return pItem->m_hFont == g_hFontSearch;
}

Font* FontList_FindItem(DWORD hFont)
{
	g_hFontSearch = hFont;
	FontList::iterator iter = std::find_if(g_FontList.begin(), g_FontList.end(), FontList_HandleEqualFn);
	
	if (iter != g_FontList.end())
		return (*iter);

	return NULL;
}

FontString* FontStringList_FindItem(FontStringList* pList, char* szString)
{
	g_szStringSearch = szString;
	FontStringList::iterator iter = std::find_if(pList->begin(), pList->end(), FontStringList_HandleEqualFn_ByString);
	
	if (iter != pList->end())
		return (*iter);
	
	return NULL;
}

FontString* FontStringList_FindItem(FontStringList* pList, DWORD hSurface)
{
	g_hSurfaceSearch = hSurface;
	FontStringList::iterator iter = std::find_if(pList->begin(), pList->end(), FontStringList_HandleEqualFn_BySurface);
	
	if (iter != pList->end())
		return (*iter);
	
	return NULL;
}

FontString* FontStringList_FindItem(DWORD hSurface)
{
	FontList::iterator iter = g_FontList.begin();
	while (true)
	{			
		if (iter == g_FontList.end()) 
			break;
		
		Font* pItem = *iter;
		
		FontString* pString = FontStringList_FindItem(&pItem->m_StringList, hSurface);
		if (pString)
			return pString;
			
		iter++;
	}

	return NULL;
}

void FontList_Update()
{
	FontList::iterator iter = g_FontList.begin();
	float fTime = ILTCSBase_GetTime(g_pLTClient);
	
	while (true)
	{			
		if (iter == g_FontList.end()) 
			break;
		
		BOOL bDelete = FALSE;
		Font* pItem = *iter;
		
		if (pItem->m_StringList.size())
		{
			FontStringList::iterator string_iter = pItem->m_StringList.begin();
			while (true)
			{			
				if (string_iter == pItem->m_StringList.end()) 
					break;
				
				FontString* pString = *string_iter;

				if (fTime - pString->m_fTime > FONT_STRING_CLEANUP_TIME)
				{
					//g_pLTClient->SetSurfaceUserData(pString->m_hSurface, NULL);

					//if (pString->m_hSurface)
						g_pLTClient->DeleteSurface(pString->m_hSurface);

					delete pString;
					string_iter = pItem->m_StringList.erase(string_iter);
				}
				else
				{
					string_iter++;
				}			
			}
		}
		else
		{
			if (fTime - pItem->m_fTime > FONT_LIST_CLEANUP_TIME)
			{
				delete pItem;
				iter = g_FontList.erase(iter);
				bDelete = TRUE;
			}
		}

		if (!bDelete)
			iter++;
	}
}

void FontList_Clear(BOOL bDeleteSurfaces)
{
	if (bDeleteSurfaces)
	{
		if (g_hWhitePixelSurface)
		{
			g_pLTClient->DeleteSurface(g_hWhitePixelSurface);
			g_hWhitePixelSurface = NULL;
		}

		/*if (g_hIntroductionSurface)
		{
			g_pLTClient->DeleteSurface(g_hIntroductionSurface);
			g_hIntroductionSurface = NULL;
		}*/
		
		for (int i = 0; i < 10 ; i++)
		{
			if (g_hFrameRateFontSurface[i])
			{
				g_pLTClient->DeleteSurface(g_hFrameRateFontSurface[i]);
				g_hFrameRateFontSurface[i] = NULL;
			}			
		}
	}
	
	//int a = g_FontList.size();
	logf(0, LINFO, "Font list size on destruction = %d", g_FontList.size());
	FontList::iterator iter = g_FontList.begin();
	
	while (true)
	{			
		if (iter == g_FontList.end()) 
			break;
		
		Font* pItem = *iter;

		logf(0, LINFO, "Font's string list (%08X) size on destruction = %d", &pItem->m_StringList, pItem->m_StringList.size());
		if (pItem->m_StringList.size())
		{
			FontStringList::iterator string_iter = pItem->m_StringList.begin();
			while (true)
			{			
				if (string_iter == pItem->m_StringList.end()) 
					break;
				
				FontString* pString = *string_iter;
				//logf(0, LTRACE, "[%08X] %s (%d)", pString->m_hSurface, pString->m_szString, pString->m_pLines ? pString->m_pLines->size() : -1);
				logf(0, LTRACE, "[%08X] %s", pString->m_hSurface, pString->m_szString);

				if (bDeleteSurfaces /*&& pString->m_hSurface*/)
				{
					//g_pLTClient->SetSurfaceUserData(pString->m_hSurface, NULL);
					g_pLTClient->DeleteSurface(pString->m_hSurface);
				}

				delete pString;
				string_iter++;
			}
		}
		
		delete pItem;
		iter++;
	}
	
	g_FontList.clear();
}

BOOL RegisterRawMouseDevice()
{
	RAWINPUTDEVICE Rid;
	
	Rid.usUsagePage = 0x01; 
	Rid.usUsage = 0x02; 
	Rid.dwFlags = 0; //RIDEV_NOLEGACY; 
	Rid.hwndTarget = 0;
	
	return RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
}

void ProcessRawMouseInput(LPARAM lParam, LONG& lLastX, LONG& lLastY)
{
	UINT dwSize;
				
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (lpb) 
	{
		UINT dwRetSize = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		if (dwRetSize == dwSize)
		{
			RAWINPUT* raw = (RAWINPUT*)lpb;	

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				lLastX += raw->data.mouse.lLastX;
				lLastY += raw->data.mouse.lLastY;
			}
		}
	} 
}

void EngineHack_WriteFunction(HANDLE hProcess, LPVOID lpAddr, DWORD dwNew, DWORD& dwOld)
{
	DWORD dwOldProtect, dwTemp;
	DWORD* dwContent = (DWORD*)lpAddr;
	
	VirtualProtectEx(hProcess, lpAddr, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);	
	dwOld = dwContent[0];
	dwContent[0] = dwNew;
	VirtualProtectEx(hProcess, lpAddr, 4, dwOldProtect, &dwTemp);
}

void EngineHack_WriteCall(HANDLE hProcess, LPVOID lpAddr, DWORD dwNew, BOOL bStructCall)
{
	DWORD dwOldProtect, dwTemp;
	BYTE* pContent = (BYTE*)lpAddr;
	DWORD* pCodeContent = (DWORD*)(pContent + 1);
	DWORD dwCallCode = dwNew - (DWORD)lpAddr - 5;
	
	if (bStructCall)
	{
		VirtualProtectEx(hProcess, lpAddr, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		
		pContent[0] = 0xE8; 
		pCodeContent[0] = dwCallCode;
		pContent[5] = 0x90;
		
		VirtualProtectEx(hProcess, lpAddr, 6, dwOldProtect, &dwTemp);
	}
	else
	{
		VirtualProtectEx(hProcess, lpAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		
		pContent[0] = 0xE8; 
		pCodeContent[0] = dwCallCode;
		
		VirtualProtectEx(hProcess, lpAddr, 5, dwOldProtect, &dwTemp);
	}
}

void CreateFrameRateFontSurfaces()
{
	for (int i = 0; i < 10 ; i++)
	{
		g_hFrameRateFontSurface[i] = g_pLTClient->CreateSurface(FRAME_RATE_FONT_WIDTH, FRAME_RATE_FONT_HEIGHT);
		for (int j = 0; j < (FRAME_RATE_FONT_WIDTH * FRAME_RATE_FONT_HEIGHT) ; j++)
		{
			if (g_nNums[i][j])
			{			
				DWORD y = j / FRAME_RATE_FONT_WIDTH;
				DWORD x = j % FRAME_RATE_FONT_WIDTH;
				g_pLTClient->SetPixel(g_hFrameRateFontSurface[i], x, y, 0x00FFFFFF);
			}
		}
	}	
}

/*void CreateIntroductionSurface()
{
	char szTitle[64];
	char szIntro[128];
	sprintf(szTitle, APP_NAME, APP_VERSION);
	sprintf(szIntro, "%s\nPage Up - borderless window toggle\nPage Down - draw FPS counter toggle", szTitle);

	DWORD hFont = g_pLTClient->CreateFont("Terminal", INTRODUCTION_FONT_WIDTH, INTRODUCTION_FONT_HEIGHT, FALSE, FALSE, FALSE);
	DWORD hString = ILTCSBase_CreateString(szIntro);
	g_hIntroductionSurface = g_pLTClient->CreateSurfaceFromString(hFont, hString, 0x0000FF00, 0, 0, 0);
	ILTCSBase_FreeString(hString);
	g_pLTClient->DeleteFont(hFont);
}*/