//------------------------------------------------------------------------------
// FDraw Proxy
//------------------------------------------------------------------------------
#include "StdAfx.h"

#define TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR  | D3DFVF_TEX1 )

struct TLVertex
{	
	float m_VecX;
	float m_VecY;
	float m_VecZ;
	float rhw;	
	DWORD color1;
	DWORD color2;
	float tu, tv;
};

#define WIN32_LEAN_AND_MEAN	

//HHOOK g_hKeyboardHook;
extern FILE* g_LogFile;

typedef HRESULT (WINAPI* DirectDrawCreate_Type)(GUID FAR *, LPDIRECTDRAW FAR *, IUnknown FAR *);
typedef HRESULT (WINAPI* DirectDrawCreateEx_Type)( GUID FAR *, LPVOID *, REFIID,IUnknown FAR *);

struct ddraw_dll
{
	HMODULE dll;
	FARPROC	AcquireDDThreadLock;
	FARPROC	CheckFullscreen;
	FARPROC	CompleteCreateSysmemSurface;
	FARPROC	D3DParseUnknownCommand;
	FARPROC	DDGetAttachedSurfaceLcl;
	FARPROC	DDInternalLock;
	FARPROC	DDInternalUnlock;
	FARPROC	DSoundHelp;
	FARPROC	DirectDrawCreate;
	FARPROC	DirectDrawCreateClipper;
	FARPROC	DirectDrawCreateEx;
	FARPROC	DirectDrawEnumerateA;
	FARPROC	DirectDrawEnumerateExA;
	FARPROC	DirectDrawEnumerateExW;
	FARPROC	DirectDrawEnumerateW;
	FARPROC	DllCanUnloadNow;
	FARPROC	DllGetClassObject;
	FARPROC	GetDDSurfaceLocal;
	FARPROC	GetOLEThunkData;
	FARPROC	GetSurfaceFromDC;
	FARPROC	RegisterSpecialCase;
	FARPROC	ReleaseDDThreadLock;
} g_DDraw;

__declspec(naked) void FakeAcquireDDThreadLock()			{ _asm { jmp [g_DDraw.AcquireDDThreadLock] } }
__declspec(naked) void FakeCheckFullscreen()				{ _asm { jmp [g_DDraw.CheckFullscreen] } }
__declspec(naked) void FakeCompleteCreateSysmemSurface()	{ _asm { jmp [g_DDraw.CompleteCreateSysmemSurface] } }
__declspec(naked) void FakeD3DParseUnknownCommand()			{ _asm { jmp [g_DDraw.D3DParseUnknownCommand] } }
__declspec(naked) void FakeDDGetAttachedSurfaceLcl()		{ _asm { jmp [g_DDraw.DDGetAttachedSurfaceLcl] } }
__declspec(naked) void FakeDDInternalLock()					{ _asm { jmp [g_DDraw.DDInternalLock] } }
__declspec(naked) void FakeDDInternalUnlock()				{ _asm { jmp [g_DDraw.DDInternalUnlock] } }
__declspec(naked) void FakeDSoundHelp()						{ _asm { jmp [g_DDraw.DSoundHelp] } }
//HRESULT WINAPI FakeDirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
__declspec(naked) void FakeDirectDrawCreate()				{ _asm { jmp [g_DDraw.DirectDrawCreate] } }
// HRESULT WINAPI DirectDrawCreateClipper( DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter );
__declspec(naked) void FakeDirectDrawCreateClipper()		{ _asm { jmp [g_DDraw.DirectDrawCreateClipper] } }
HRESULT WINAPI FakeDirectDrawCreateEx( GUID FAR * lpGuid, LPVOID *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter);
//__declspec(naked) void FakeDirectDrawCreateEx()				{ _asm { jmp [g_DDraw.DirectDrawCreateEx] } }
//HRESULT WINAPI DirectDrawEnumerateA( LPDDENUMCALLBACKA lpCallback, LPVOID lpContext );
__declspec(naked) void FakeDirectDrawEnumerateA()			{ _asm { jmp [g_DDraw.DirectDrawEnumerateA] } }
// HRESULT WINAPI DirectDrawEnumerateExA( LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags );
__declspec(naked) void FakeDirectDrawEnumerateExA()			{ _asm { jmp [g_DDraw.DirectDrawEnumerateExA] } }
// HRESULT WINAPI DirectDrawEnumerateExW( LPDDENUMCALLBACKEXW lpCallback, LPVOID lpContext, DWORD dwFlags );
__declspec(naked) void FakeDirectDrawEnumerateExW()			{ _asm { jmp [g_DDraw.DirectDrawEnumerateExW] } }
// HRESULT WINAPI DirectDrawEnumerateW( LPDDENUMCALLBACKW lpCallback, LPVOID lpContext );
__declspec(naked) void FakeDirectDrawEnumerateW()			{ _asm { jmp [g_DDraw.DirectDrawEnumerateW] } }
__declspec(naked) void FakeDllCanUnloadNow()				{ _asm { jmp [g_DDraw.DllCanUnloadNow] } }
__declspec(naked) void FakeDllGetClassObject()				{ _asm { jmp [g_DDraw.DllGetClassObject] } }
__declspec(naked) void FakeGetDDSurfaceLocal()				{ _asm { jmp [g_DDraw.GetDDSurfaceLocal] } }
__declspec(naked) void FakeGetOLEThunkData()				{ _asm { jmp [g_DDraw.GetOLEThunkData] } }
__declspec(naked) void FakeGetSurfaceFromDC()				{ _asm { jmp [g_DDraw.GetSurfaceFromDC] } }
__declspec(naked) void FakeRegisterSpecialCase()			{ _asm { jmp [g_DDraw.RegisterSpecialCase] } }
__declspec(naked) void FakeReleaseDDThreadLock()			{ _asm { jmp [g_DDraw.ReleaseDDThreadLock] } }

HRESULT __stdcall FakeD3DDevice_Load(LPDIRECT3DDEVICE7 lpDevice, LPDIRECTDRAWSURFACE7 lpDestTex,LPPOINT lpDestPoint,LPDIRECTDRAWSURFACE7 lpSrcTex,LPRECT lprcSrcRect,DWORD dwFlags)
{
	return lpDestTex->Blt(NULL, lpSrcTex, NULL, DDBLT_WAIT, 0);
}

void ReadConfig(char* szFilename)
{
	g_bNoCompatWarning = GetPrivateProfileInt("Misc", "NoCompatWarning", FALSE, szFilename);
	g_bDontShutdownRenderer = GetPrivateProfileInt("Misc", "DontShutdownRenderer", FALSE, szFilename);
	g_bShowFPS = GetPrivateProfileInt("Misc", "ShowFPS", FALSE, szFilename);
	g_nFrameLimiterSleep = GetPrivateProfileInt("Misc", "FrameLimiterSleep", FALSE, szFilename);

	g_fMaxFPS = GetPrivateProfileInt("Compatibility", "MaxFPS", 60, szFilename);
	g_bIntelHDFix = GetPrivateProfileInt("Compatibility", "Fix_IntelHD", FALSE, szFilename);
	g_bRadeon5700Fix = GetPrivateProfileInt("Compatibility", "Fix_Radeon5700", FALSE, szFilename);
	g_bCameraFOVFix = GetPrivateProfileInt("Compatibility", "Fix_CameraFOV", FALSE, szFilename);
	g_bCameraFOVFix2 = g_bCameraFOVFix;
	g_bViewModelFOVFix = GetPrivateProfileInt("Compatibility", "Fix_ViewModelFOV", FALSE, szFilename);
	g_bSolidDrawingFix = GetPrivateProfileInt("Compatibility", "Fix_SolidDrawing", FALSE, szFilename);
	g_bLightLoadFix = GetPrivateProfileInt("Compatibility", "Fix_LightLoad", FALSE, szFilename);
	g_bTWMDetailTexFix = GetPrivateProfileInt("Compatibility", "Fix_TWMDetailTex", FALSE, szFilename);
	g_bMiscCCFix = GetPrivateProfileInt("Compatibility", "Fix_MiscCC", FALSE, szFilename);
	g_bRawMouseInputFix = GetPrivateProfileInt("Compatibility", "Fix_RawMouseInput", FALSE, szFilename);
	g_bRawMouseInputFix2 = g_bRawMouseInputFix;

	g_fRMIScaleGlobal = (float)GetPrivateProfileInt("RawMouseInput", "ScaleGlobal", 1000, szFilename) / 1000000.0f;
	g_fRMIScaleY = GetPrivateProfileInt("RawMouseInput", "ScaleY", 1100000, szFilename) / 1000000.0f;

	g_bDgVoodooMode = GetPrivateProfileInt("Main", "DgVoodooMode", FALSE, szFilename);

	if (g_bDgVoodooMode)
	{
		g_bIntelHDFix = FALSE;
		g_bRadeon5700Fix = FALSE;
		//g_bSolidDrawingFix = FALSE;
		g_bLightLoadFix = FALSE;
		g_bTWMDetailTexFix = FALSE;
	}
}

void ClearMultiLinesHolder();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			char path[MAX_PATH];
			HANDLE hProcess = NULL;	
			ReadConfig(".\\ddraw_avp2.ini");

			if (g_bDgVoodooMode)
			{
				g_DDraw.dll = LoadLibrary(".\\vdraw.dll");
				if (!g_DDraw.dll)
				{
					MessageBox(0, "Cannot load dgVoodoo2's vdraw.dll (renamed ddraw.dll) library", APP_NAME, MB_ICONERROR);
					ExitProcess(0);
				}
			}
			else
			{
				CopyMemory(path + GetSystemDirectory(path,MAX_PATH-10), "\\ddraw.dll",11);
				g_DDraw.dll = LoadLibrary(path);
				if (!g_DDraw.dll)
				{
					MessageBox(0, "Cannot load original ddraw.dll library", APP_NAME, MB_ICONERROR);
					ExitProcess(0);
				}
			}
			
			g_DDraw.AcquireDDThreadLock			= GetProcAddress(g_DDraw.dll, "AcquireDDThreadLock");
			g_DDraw.CheckFullscreen				= GetProcAddress(g_DDraw.dll, "CheckFullscreen");
			g_DDraw.CompleteCreateSysmemSurface	= GetProcAddress(g_DDraw.dll, "CompleteCreateSysmemSurface");
			g_DDraw.D3DParseUnknownCommand		= GetProcAddress(g_DDraw.dll, "D3DParseUnknownCommand");
			g_DDraw.DDGetAttachedSurfaceLcl		= GetProcAddress(g_DDraw.dll, "DDGetAttachedSurfaceLcl");
			g_DDraw.DDInternalLock				= GetProcAddress(g_DDraw.dll, "DDInternalLock");
			g_DDraw.DDInternalUnlock			= GetProcAddress(g_DDraw.dll, "DDInternalUnlock");
			g_DDraw.DSoundHelp					= GetProcAddress(g_DDraw.dll, "DSoundHelp");
			g_DDraw.DirectDrawCreate			= GetProcAddress(g_DDraw.dll, "DirectDrawCreate");
			g_DDraw.DirectDrawCreateClipper		= GetProcAddress(g_DDraw.dll, "DirectDrawCreateClipper");
			g_DDraw.DirectDrawCreateEx			= GetProcAddress(g_DDraw.dll, "DirectDrawCreateEx");
			g_DDraw.DirectDrawEnumerateA		= GetProcAddress(g_DDraw.dll, "DirectDrawEnumerateA");
			g_DDraw.DirectDrawEnumerateExA		= GetProcAddress(g_DDraw.dll, "DirectDrawEnumerateExA");
			g_DDraw.DirectDrawEnumerateExW		= GetProcAddress(g_DDraw.dll, "DirectDrawEnumerateExW");
			g_DDraw.DirectDrawEnumerateW		= GetProcAddress(g_DDraw.dll, "DirectDrawEnumerateW");
			g_DDraw.DllCanUnloadNow				= GetProcAddress(g_DDraw.dll, "DllCanUnloadNow");
			g_DDraw.DllGetClassObject			= GetProcAddress(g_DDraw.dll, "DllGetClassObject");
			g_DDraw.GetDDSurfaceLocal			= GetProcAddress(g_DDraw.dll, "GetDDSurfaceLocal");
			g_DDraw.GetOLEThunkData				= GetProcAddress(g_DDraw.dll, "GetOLEThunkData");
			g_DDraw.GetSurfaceFromDC			= GetProcAddress(g_DDraw.dll, "GetSurfaceFromDC");
			g_DDraw.RegisterSpecialCase			= GetProcAddress(g_DDraw.dll, "RegisterSpecialCase");
			g_DDraw.ReleaseDDThreadLock			= GetProcAddress(g_DDraw.dll, "ReleaseDDThreadLock");

			g_LogFile = fopen("ddraw_avp2.log", "w");
			logf(0, LINFO, "Original ddraw.dll address = %08X", g_DDraw.dll);

			if (!g_bNoCompatWarning)
			{
				char szTitle[64];
				sprintf(szTitle, APP_NAME, APP_VERSION);

				MessageBox(NULL, 
				"D3D7FIX wrapper is installed and it is not compatible with MSP v2.0+ and PaybackTime v0.2+! If you are starting one of these mods, please remove ddraw.dll from game directory.", 
				szTitle,
				MB_ICONWARNING);
			}

			if (g_bDgVoodooMode)
			{
				logf(0, LINFO, "dgVoodoo mode enabled");
			}
				
			logf(0, LINFO, "Framelimiter = %d FPS", (int)g_fMaxFPS);
			timeBeginPeriod(1);
		}
		break;

		case DLL_PROCESS_DETACH:
			timeEndPeriod(1);
			ClearMultiLinesHolder();
			FontList_Clear(FALSE);
			fclose( g_LogFile );
			FreeLibrary(g_DDraw.dll);
		break;
	}
	return TRUE;
}

void ApplyIntelHD_RHW_Fix()
{
	DWORD dwDLLAddress = (DWORD)GetModuleHandle("d3d.ren");
	logf(0, LINFO, "d3d.ren address = %08X, applying IntelHD RHW fix", dwDLLAddress);
	
	float fIntelHDFix = 0.5f;
	TLVertex* pVert = (TLVertex*)(dwDLLAddress + 0x58500);
	pVert[0].rhw = fIntelHDFix;
	pVert[1].rhw = fIntelHDFix;
	pVert[2].rhw = fIntelHDFix;
	pVert[3].rhw = fIntelHDFix;
	
	pVert = (TLVertex*)(dwDLLAddress + 0x58668);
	pVert[0].rhw = fIntelHDFix;
	pVert[1].rhw = fIntelHDFix;
	pVert[2].rhw = fIntelHDFix;
	pVert[3].rhw = fIntelHDFix;
}

float g_fTotalSensFull = 0.5f;
void (*OldGetAxisOffsets)(float *offsets);
void MyGetAxisOffsets(float *offsets)
{
	if (g_bRawMouseInputFix2)
	{
		/*float fSens = 1.0f;
		
		if (g_hMouseSensitivity)
			fSens = ILTCSBase_GetVarValueFloat(g_hMouseSensitivity);

		float fTotalSensFull = (0.166f + (fSens / 2.75f));*/
		
		offsets[0] = (float)g_lRMILastX * g_fRMIScaleGlobal * g_fTotalSensFull;
		offsets[1] = (float)g_lRMILastY * g_fRMIScaleGlobal * (g_fTotalSensFull * g_fRMIScaleY);
	}
	else
	{
		OldGetAxisOffsets(offsets);
	}
}

void (*OldRunConsoleString)(char *pString);
void MyRunConsoleString(char *pString)
{
	if (g_bRawMouseInputFix2 && (strstr(pString, "scale") == pString) && strstr(pString, "X"))
	{
		int nLength = strlen(pString);
		char* szArrow = pString + nLength - 1;

		while (*szArrow != ' ')
			szArrow--;
		
		szArrow++;

		g_fTotalSensFull = atof(szArrow) / 0.001f;

		//logf(0, LTRACE, "MyRunConsoleString - %s [%f]", pString, g_fTotalSensFull);
	}

	OldRunConsoleString(pString);
}

void ApplyRawMouseInput_Fix()
{
	logf(0, LINFO, "Registering raw mouse input = %d", RegisterRawMouseDevice());

	OldGetAxisOffsets = g_pLTClient->GetAxisOffsets;
	g_pLTClient->GetAxisOffsets = MyGetAxisOffsets;
	OldRunConsoleString = g_pLTClient->RunConsoleString;
	g_pLTClient->RunConsoleString = MyRunConsoleString;
}

float increaseHorFOV(float fFOVX, float fAspectRatio)
{
	float tempVradian = 2.0f * atanf(tanf(fFOVX / 2.0f) * 0.75f);
	return (2.0f * atanf(tanf(tempVradian / 2.0f) * fAspectRatio));
}

void (*OldSetCameraFOV)(DWORD hObj, float fovX, float fovY);
void MySetCameraFOV(DWORD hObj, float fovX, float fovY)
{
	fovX = increaseHorFOV(fovX, (float)g_dwWidth / (float)g_dwHeight);
	OldSetCameraFOV(hObj, fovX, fovY);	
}

void ApplyCameraFOV_Fix()
{	
	logf(0, LINFO, "Applying camera FOV fix");

	OldSetCameraFOV = g_pLTClient->SetCameraFOV;
	g_pLTClient->SetCameraFOV = MySetCameraFOV;
}

DWORD (*OldDeleteSurface)(DWORD hSurface);
DWORD (*OldScaleSurfaceToSurfaceSolidColor)(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect, DWORD hTransColor, DWORD hFillColor);
DWORD (*OldDrawSurfaceSolidColor)(DWORD hDest, DWORD hSrc, LTRect *pSrcRect, int destX, int destY, DWORD hTransColor, DWORD hFillColor);
DWORD (*OldFillRect)(DWORD hDest, LTRect *pRect, DWORD hColor);
void (*OldDrawStringToSurface)(DWORD hDest, DWORD hFont, DWORD hString, LTRect* pRect, DWORD hForeColor, DWORD hBackColor);

DWORD MyDeleteSurface(DWORD hSurface)
{
	//if (g_pLTClient->GetSurfaceUserData(hSurface))
	//	return 0;

	return OldDeleteSurface(hSurface);
}

FontString* g_pPrevFontString = NULL;
FSLines g_MultiLines;
FSLines::iterator g_MultiLinesIter = NULL;

void ClearMultiLinesHolder()
{
	logf(0, LINFO, "Multi-lines holder size on destruction = %d (iter = %08X)", g_MultiLines.size(), g_MultiLinesIter);
	g_MultiLines.clear();
}

DWORD MyDrawSurfaceSolidColor(DWORD hDest, DWORD hSrc, LTRect *pSrcRect, int destX, int destY, DWORD hTransColor, DWORD hFillColor)
{	
	/*CisSurface* pCisSurface = NULL;
	RSurface* pSurface = NULL;
	if (hSrc)
	{
		pCisSurface = (CisSurface*)hSrc;
		pSurface = (RSurface*)pCisSurface->m_hBuffer;
	}*/

	if (hSrc)
	{
		FontString* pString = (FontString*)g_pLTClient->GetSurfaceUserData(hSrc);
		//g_pLTClient->SetSurfaceUserData(hSrc, NULL);
		if (pString)
		{
			hSrc = pString->m_hSurface;
			
			g_pLTClient->SetOptimized2DColor(hFillColor); // OldDrawSurfaceSolidColor(hDest, hSrc, pSrcRect, destX, destY, hTransColor, hFillColor);
			//DWORD dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, hSrc, pSrcRect, destX, destY, hTransColor);	

			DWORD dwRet;
			if (g_MultiLines.size())
			{
				int nSize = g_MultiLines.size();
				if (!g_MultiLinesIter)
				{
					dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, hSrc, pSrcRect, destX, destY, hTransColor);	
					g_MultiLinesIter = g_MultiLines.begin();
				}
				else
				{
					FontString* pMultiString = *g_MultiLinesIter;
					dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, pMultiString->m_hSurface, pSrcRect, destX, destY, hTransColor);
					g_MultiLinesIter++;
				}
				
				if (g_MultiLinesIter == g_MultiLines.end())
				{
					g_MultiLinesIter = NULL;
					g_MultiLines.clear();
				}
			}
			else
			{
				dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, hSrc, pSrcRect, destX, destY, hTransColor);	
			}

			/*if (pString->m_pLines)
			{
				for (int i = 0; i < pString->m_pLines->size() ; i++)
				{
					FontString* pMultiString = pString->m_pLines->at(i);
					dwRet = g_pLTClient->DrawSurfaceToSurfaceTransparent(hDest, pMultiString->m_hSurface, pSrcRect, destX, destY, hTransColor); // &pMultiString->m_rcSrc	
				}

				pString->m_pLines->clear();
			}*/

			g_pLTClient->SetOptimized2DColor(0xFFFFFFFF);
			
			return dwRet;
		}
	}

	return 0;
}

DWORD MyScaleSurfaceToSurfaceSolidColor(DWORD hDest, DWORD hSrc, LTRect *pDestRect, LTRect *pSrcRect, DWORD hTransColor, DWORD hFillColor)
{
	if (!g_hWhitePixelSurface)
	{
		g_hWhitePixelSurface = g_pLTClient->CreateSurface(1, 1);
		//OldFillRect(g_hWhitePixelSurface, NULL, 0xFFFFFFFF);
		g_pLTClient->FillRect(g_hWhitePixelSurface, NULL, 0xFFFFFFFF);
	}
	
	g_pLTClient->SetOptimized2DColor(hFillColor);
	//DWORD dwRet = g_pLTClient->ScaleSurfaceToSurfaceTransparent(hDest, hSrc, pDestRect, pSrcRect, hTransColor);
	DWORD dwRet = g_pLTClient->ScaleSurfaceToSurfaceTransparent(hDest, g_hWhitePixelSurface, pDestRect, NULL, hTransColor);
	g_pLTClient->SetOptimized2DColor(0xFFFFFFFF);

	return dwRet;
}

/*DWORD MyFillRect(DWORD hDest, LTRect *pRect, DWORD hColor)
{
	if (hColor == 0x00FF00FF)
		return 0;

	return OldFillRect(hDest, pRect, hColor);
}*/

void MyDrawStringToSurface(DWORD hDest, DWORD hFont, DWORD hString, LTRect* pRect, DWORD hForeColor, DWORD hBackColor)
{	
	DWORD hOldDest = hDest;
	float fTime = ILTCSBase_GetTime(g_pLTClient);
	
	/*FontString* pPrevString = (FontString*)g_pLTClient->GetSurfaceUserData(hDest);
	if (pPrevString && (pRect->left || pRect->top))
	{
		pPrevString->m_fTime = fTime;
		OldDrawStringToSurface(pPrevString->m_hSurface, hFont, hString, pRect, hForeColor, hBackColor);

		return;
	}*/
	
	char* szString = ILTCSBase_GetStringData(hString);
	Font* pFont = FontList_FindItem(hFont);

	if (!pFont)
	{
		pFont = new Font(hFont, fTime);
		g_FontList.push_back(pFont);
	}

	FontString* pString = FontStringList_FindItem(&pFont->m_StringList, szString);	
	if (!pString)
	{		
		//pString = FontStringList_FindItem(&pFont->m_StringList, hDest);
		//if (pString)
		{
			DWORD dwHeight, dwWidth;
			g_pLTClient->GetSurfaceDims(hDest, &dwWidth, &dwHeight);
			hDest = g_pLTClient->CreateSurface(dwWidth, dwHeight);
			//OldFillRect(hDest, NULL, hBackColor);
			g_pLTClient->FillRect(hDest, NULL, hBackColor);
		}

		OldDrawStringToSurface(hDest, hFont, hString, pRect, hForeColor, hBackColor);
		pString = new FontString(hDest, szString, fTime);
		pFont->m_StringList.push_back(pString);
	}
	else
	{
		pString->m_fTime = fTime;
	}

	/*FontString* pPrevString = (FontString*)g_pLTClient->GetSurfaceUserData(hOldDest);
	if (pPrevString && (pRect->left || pRect->top))
	{
		if (!pPrevString->m_pLines)
		{
			pPrevString->m_pLines = new FSLines();
			pPrevString->m_pLines->reserve(FONT_STRING_LINE_RESERVE);
		}
		
		int nSize = pPrevString->m_pLines->size();

		pString->m_rcSrc = *pRect;
		pPrevString->m_pLines->push_back(pString);

		return;
	}*/

	if (pRect->left || pRect->top)
	{
		//OldDrawStringToSurface(g_pPrevFontString->m_hSurface, hFont, hString, pRect, hForeColor, hBackColor);	
		if (std::find(g_MultiLines.begin(), g_MultiLines.end(), pString) == g_MultiLines.end())
			g_MultiLines.push_back(pString);

		g_pLTClient->SetSurfaceUserData(hOldDest, g_pPrevFontString);

		return;
	}

	g_pPrevFontString = pString;
	g_pLTClient->SetSurfaceUserData(hOldDest, pString);
}

void ApplySolidDrawing_Fix()
{
	logf(0, LINFO, "Applying solid drawing fix");

	OldDrawSurfaceSolidColor = g_pLTClient->DrawSurfaceSolidColor;
	g_pLTClient->DrawSurfaceSolidColor = MyDrawSurfaceSolidColor;

	OldScaleSurfaceToSurfaceSolidColor = g_pLTClient->ScaleSurfaceToSurfaceSolidColor;
	g_pLTClient->ScaleSurfaceToSurfaceSolidColor = MyScaleSurfaceToSurfaceSolidColor;
	
	//OldFillRect = g_pLTClient->FillRect;
	//g_pLTClient->FillRect = MyFillRect;
	//OldDeleteSurface = g_pLTClient->DeleteSurface;
	//g_pLTClient->DeleteSurface = MyDeleteSurface;
	OldDrawStringToSurface = g_pLTClient->DrawStringToSurface;
	g_pLTClient->DrawStringToSurface = MyDrawStringToSurface;
}

void (*OldShutdown)();
void (*OldShutdownWithMessage)( char *pMsg, ... );
void MyShutdown()
{
	FontList_Clear(TRUE);
	OldShutdown();
}

void MyShutdownWithMessage( char *pMsg, ... )
{
	FontList_Clear(TRUE);

	va_list argp;
	
	va_start(argp, pMsg);
	OldShutdownWithMessage(pMsg, argp);
	va_end(argp);
}

/*DWORD g_nNumberCoords[10][2];
void CreateFrameRateFontSurface()
{
	DWORD hFont = g_pLTClient->CreateFont("Terminal", FRAME_RATE_FONT_WIDTH, FRAME_RATE_FONT_HEIGHT, FALSE, FALSE, FALSE);
	DWORD hString = ILTCSBase_CreateString("0123456789");
	g_hFrameRateFontSurface = g_pLTClient->CreateSurfaceFromString(hFont, hString, 0x0000FF00, 0, 0, 0);
	ILTCSBase_FreeString(hString);
	g_pLTClient->DeleteFont(hFont);
	
	
	BOOL bSpace = TRUE;
	DWORD dwWidth, dwHeight;
	g_pLTClient->GetSurfaceDims(g_hFrameRateFontSurface, &dwWidth, &dwHeight);
	
	int nIndex = -1;		
	for (DWORD i = 0; i < dwWidth ; i++)
	{
		for (DWORD j = 0; j < dwHeight ; j++)
		{
			DWORD dwColor;
			g_pLTClient->GetPixel(g_hFrameRateFontSurface, i, j, &dwColor);
			
			if (dwColor)
			{
				if (bSpace) nIndex++;
				break;
			}
		}
		
		if (j == dwHeight)
		{
			if (!bSpace)
			{
				bSpace = TRUE;
				g_nNumberCoords[nIndex][1] = i;
			}
		}
		else
		{
			if (bSpace)
			{
				bSpace = FALSE;
				g_nNumberCoords[nIndex][0] = i;
			}
		}
	}
}*/

/*void DrawIntroduction()
{
	float fTime = ILTCSBase_GetTime(g_pLTClient);

	if (!g_hIntroductionSurface)
		CreateIntroductionSurface();

	DWORD hScreen = g_pLTClient->GetScreenSurface();
}*/

void DrawFrameRate()
{
	/*DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");
	bool & g_bInOptimized2D = *(bool*)(dwDllAddress + 0x5DE44);
	bool & g_bIn3D = *(bool*)(dwDllAddress + 0x5DE40);*/
	
	
	if (!g_hFrameRateFontSurface[0])
		CreateFrameRateFontSurfaces();

	DWORD hScreen = g_pLTClient->GetScreenSurface();

	char szBuffer[64];
	itoa(g_nLastFrameRate, szBuffer, 10);

	int nLength = strlen(szBuffer);
	LTRect rcDest;
	rcDest.top = g_dwHeight - (FRAME_RATE_FONT_HEIGHT * FRAME_RATE_FONT_SCALE) - 5;
	rcDest.bottom = rcDest.top + (FRAME_RATE_FONT_HEIGHT * FRAME_RATE_FONT_SCALE);

	DWORD hFontColor = 0x0000FF00;
	if (g_nLastFrameRate < FRAME_RATE_LEVEL_RED)
		hFontColor = 0x00FF0000;
	else if (g_nLastFrameRate < FRAME_RATE_LEVEL_YELLOW)
		hFontColor = 0x00FFFF00;

	g_pLTClient->SetOptimized2DColor(hFontColor);
	for (int k = 0; k < nLength ; k++)
	{
		//g_pLTClient->DrawSurfaceToSurface(hScreen, g_hFrameRateFontSurface[szBuffer[k] - 0x30], NULL, nStartX + (k * (FRAME_RATE_FONT_HEIGHT + 1)), nStartY);	
		
		rcDest.left = 5 + (k * (FRAME_RATE_FONT_WIDTH + 1) * FRAME_RATE_FONT_SCALE);
		rcDest.right = rcDest.left + (FRAME_RATE_FONT_WIDTH * FRAME_RATE_FONT_SCALE);
		g_pLTClient->ScaleSurfaceToSurfaceTransparent(hScreen, g_hFrameRateFontSurface[szBuffer[k] - 0x30], &rcDest, NULL, 0);
	}
	g_pLTClient->SetOptimized2DColor(0xFFFFFFFF);
}

DWORD g_hViewModelBaseFOVX = 0;
DWORD g_hViewModelBaseFOVY = 0;
DWORD g_hExtraFOVXOffset = 0;
DWORD g_hExtraFOVYOffset = 0;
float g_fLastExtraFOVXOffset = 0.0f;

#define MATH_PI 3.141592f
#define VIEW_MODE_BASE_ASPECT 1.333333f

void __fastcall MyIClientShell_Update(void* pShell)
{
	IClientShell_Update(pShell);

	if (g_bViewModelFOVFix)
	{
		if (!g_hViewModelBaseFOVX)
		{
			g_hViewModelBaseFOVX = g_pLTClient->GetConsoleVar("ViewModelBaseFOVX");
			if (g_hViewModelBaseFOVX)
			{
				float fOldBaseX = ILTCSBase_GetVarValueFloat(g_hViewModelBaseFOVX);
				float fNewBaseX = atanf(tanf(fOldBaseX*MATH_PI/360.0f) * (float)g_dwHeight/(float)g_dwWidth * VIEW_MODE_BASE_ASPECT)*360.0f/MATH_PI;
				
				char szTemp[64];
				sprintf(szTemp, "ViewModelBaseFOVX %f", fNewBaseX);
				g_pLTClient->RunConsoleString(szTemp);
			}
			
		}
		
		if (!g_hExtraFOVXOffset)
			g_hExtraFOVXOffset = g_pLTClient->GetConsoleVar("ExtraFOVXOffset");
		
		if (g_hViewModelBaseFOVX)
		{
			float fCurrXOffset = ILTCSBase_GetVarValueFloat(g_hExtraFOVXOffset);
			if (fCurrXOffset != g_fLastExtraFOVXOffset)
			{
				float fBase = ILTCSBase_GetVarValueFloat(g_hViewModelBaseFOVX);			
				float fCamera = fBase - fCurrXOffset;
				
				fCamera = atanf(tanf(fCamera * MATH_PI/360.0f) * 0.75f * (float)g_dwWidth/(float)g_dwHeight) * 360.0f/MATH_PI;
				g_fLastExtraFOVXOffset = fBase - fCamera;
				
				char szTemp[64];
				sprintf(szTemp, "ExtraFOVXOffset %f", g_fLastExtraFOVXOffset);
				g_pLTClient->RunConsoleString(szTemp);
			}
		}
	}
	
	if (g_FontList.size())
	{
		float fTime = ILTCSBase_GetTime(g_pLTClient);
		
		if (fTime - g_fLastFontListUpdate > FONT_LIST_UPDATE_TIME)
		{
			g_fLastFontListUpdate = fTime;
			FontList_Update();
		}	
	}
	
	if (g_bRawMouseInputFix2)
	{
		//if (!g_hMouseSensitivity)
		//	g_hMouseSensitivity = g_pLTClient->GetConsoleVar("MouseSensitivity");	
		
		g_lRMILastX = 0;
		g_lRMILastY = 0;
	}

	if (g_bMiscCCFix)
		g_pLTClient->RunConsoleString("UpdateRate 60");

	//DWORD hTestVar = g_pLTClient->GetConsoleVar("UpdateRate");
	//float fTest = ILTCSBase_GetVarValueFloat(hTestVar);
}

DWORD (*OldEndOptimized2D)();
DWORD MyEndOptimized2D()
{
	if (g_bShowFPS && g_bDrawFPS)
		DrawFrameRate();
	
	return OldEndOptimized2D();
}

void HookEngineStuff()
{
	logf(0, LINFO, "Hooking engine stuff");
	
	DWORD dwRead;
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwExeAddress = (DWORD)GetModuleHandle("lithtech.exe");
	g_pClientMgr = (CClientMgrBase*)(dwExeAddress + 0x0DEFAC);
	g_pLTClient = g_pClientMgr->m_pClientMgr->m_pLTClient;

	DWORD* pOrigTable = (DWORD*)*(DWORD*)g_pLTClient;
	ILTCSBase_CPrint = (ILTCSBase_CPrint_type)pOrigTable[39]; 

	ILTCSBase_CreateString = (ILTCSBase_CreateString_type)pOrigTable[46];
	ILTCSBase_FreeString = (ILTCSBase_FreeString_type)pOrigTable[47];
	
	ILTCSBase_GetStringData = (ILTCSBase_GetStringData_type)pOrigTable[50];
	ILTCSBase_GetVarValueFloat = (ILTCSBase_GetVarValueFloat_type)pOrigTable[51];
	ILTCSBase_GetVarValueString = (ILTCSBase_GetVarValueString_type)pOrigTable[52];
	ILTCSBase_GetTime = (ILTCSBase_GetTime_type)pOrigTable[53];
	ILTCSBase_GetFrameTime = (ILTCSBase_GetFrameTime_type)pOrigTable[54];

	pOrigTable = (DWORD*)*(DWORD*)g_pClientMgr->m_pClientMgr->m_pClientShell;

	if (g_pLTClient->Shutdown != MyShutdown)
	{
		OldShutdown = g_pLTClient->Shutdown;
		OldShutdownWithMessage = g_pLTClient->ShutdownWithMessage;
		OldEndOptimized2D = g_pLTClient->EndOptimized2D;
		g_pLTClient->Shutdown = MyShutdown;
		g_pLTClient->ShutdownWithMessage = MyShutdownWithMessage;
		g_pLTClient->EndOptimized2D = MyEndOptimized2D;

		IClientShell_Update = (IClientShell_Update_type)pOrigTable[15];
		EngineHack_WriteFunction(hProcess, (LPVOID)(pOrigTable + 15), (DWORD)MyIClientShell_Update, dwRead);
	}

	//DWORD& g_CV_TraceConsole = *(DWORD*)(dwExeAddress + 0xE370C);
	//g_CV_TraceConsole = 1;

	if (g_bMiscCCFix)
	{
		//g_pLTClient->RunConsoleString("UpdateRateInitted 1");
		//g_pLTClient->RunConsoleString("UpdateRate 60");	
		g_pLTClient->RunConsoleString("MaxModelShadows 0");
		g_pLTClient->RunConsoleString("MaxModelLights 10");
		g_pLTClient->RunConsoleString("NearZ 4");
		g_pLTClient->RunConsoleString("ReallyCloseNearZ 0.01");
	}
}

void ApplyLightLoad_Fix()
{
	logf(0, LINFO, "Applying light load fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");
	//DWORD dwRenderStruct = dwDllAddress + 0x58470;

	EngineHack_WriteCall(hProcess, (LPVOID)(DWORD(dwDllAddress) + 0x34CF2), (DWORD)FakeD3DDevice_Load, TRUE);
}

BOOL* g_pDetailTextureCapable;
typedef void (*sub_3F0A2A7_type)();
void (*sub_3F0A2A7)();
void My_sub_3F0A2A7()
{
	*g_pDetailTextureCapable = FALSE;
	sub_3F0A2A7();
	*g_pDetailTextureCapable = TRUE;
}

void ApplyTWMDetailTex_Fix()
{
	logf(0, LINFO, "Applying TWM detail textures fix");

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwDllAddress = (DWORD)GetModuleHandle("d3d.ren");

	g_pDetailTextureCapable = (BOOL*)(dwDllAddress + 0x5DE2C);
	sub_3F0A2A7 = (sub_3F0A2A7_type)(dwDllAddress + 0xA2A7);
	EngineHack_WriteCall(hProcess, (LPVOID)(dwDllAddress + 0x9C5A), (DWORD)My_sub_3F0A2A7, FALSE);
}

HRESULT WINAPI FakeDirectDrawCreateEx(GUID FAR * lpGUID, LPVOID *lplpDD, REFIID iid, IUnknown FAR *pUnkOuter)
{
	HookEngineStuff();
	
	if (g_bIntelHDFix)
		ApplyIntelHD_RHW_Fix();

	if (g_bCameraFOVFix)
	{
		g_bCameraFOVFix = FALSE;
		ApplyCameraFOV_Fix();
	}

	if (g_bSolidDrawingFix)
	{
		g_bSolidDrawingFix = FALSE;
		ApplySolidDrawing_Fix();
	}

	if (g_bRawMouseInputFix)
	{
		g_bRawMouseInputFix = FALSE;
		ApplyRawMouseInput_Fix();
	}

	if (g_bLightLoadFix)
		ApplyLightLoad_Fix();

	if (g_bTWMDetailTexFix)
		ApplyTWMDetailTex_Fix();

	DirectDrawCreateEx_Type DirectDrawCreateEx_fn = (DirectDrawCreateEx_Type) g_DDraw.DirectDrawCreateEx;

	LPVOID FAR dummy;
	
	HRESULT hResult = DirectDrawCreateEx_fn(lpGUID, &dummy, iid, pUnkOuter);
	*lplpDD = (LPDIRECTDRAW7) new FakeIDDraw7((LPDIRECTDRAW7)dummy);
	
	return hResult;
}