#include "stdafx.h"

float g_fLastFrameRateUpdate = 0.0f;
void FrameLimiter()
{
	if (!g_fMaxFPS) 
		return;
	
	static LARGE_INTEGER liFrequency = {0};
	static LARGE_INTEGER liStart = {0};
	LARGE_INTEGER liEnd = {0};
	
	if(!liFrequency.QuadPart)
		QueryPerformanceFrequency(&liFrequency);
	
	QueryPerformanceCounter(&liEnd);
	
	double fDifference = double(liEnd.QuadPart - liStart.QuadPart);
	fDifference /= double(liFrequency.QuadPart / 1000);
	
	double fps = g_fMaxFPS;
	
	//if(fps)
	{
		// slow down the client shell
		while(fDifference < 1000.0 / fps)
		{
			Sleep(g_nFrameLimiterSleep);
			QueryPerformanceCounter(&liEnd);
			fDifference = double(liEnd.QuadPart - liStart.QuadPart);
			fDifference /= double(liFrequency.QuadPart / 1000);
		}

		if (g_bShowFPS)
		{
			float fTime = ILTCSBase_GetTime(g_pLTClient);

			if (fTime - g_fLastFrameRateUpdate > FRAME_RATE_UPDATE_TIME)
			{
				g_fLastFrameRateUpdate = fTime;
				g_nLastFrameRate = (int)(1000.0f / fDifference + 0.1f);
			}	
		}
	}
	
	QueryPerformanceCounter(&liStart);
}

FakeIDDrawSurface7::FakeIDDrawSurface7(LPDIRECTDRAWSURFACE7 pOriginal)
{
	m_pIDDrawSurface = pOriginal;
}


FakeIDDrawSurface7::~FakeIDDrawSurface7(void)
{

}


HRESULT FakeIDDrawSurface7::QueryInterface(REFIID a, LPVOID FAR * b)
{	
	*b = NULL;
		
	HRESULT hResult = m_pIDDrawSurface->QueryInterface(a, b); 
		
	return hResult;	
}

ULONG FakeIDDrawSurface7::AddRef(void)
{
	ULONG dwResult = m_pIDDrawSurface->AddRef();
	return dwResult;	
}

ULONG FakeIDDrawSurface7::Release(void)
{
	ULONG dwCount = m_pIDDrawSurface->Release();
	
	if (dwCount == 0) 
	{		
		logf(this, LDEBUG, "FakeIDDrawSurface7::Release - release success");
		m_pIDDrawSurface = NULL;		
		delete(this); 
	}
	else
	{
		logf(this, LERROR, "FakeIDDrawSurface7::Release - release failed, reference count = %d", dwCount);
		m_pIDDrawSurface = NULL;		
		delete(this); 
	}

	g_ddsBackBuffer = NULL;
		
	return dwCount;	
}

HRESULT FakeIDDrawSurface7::AddAttachedSurface(LPDIRECTDRAWSURFACE7 a)
{
	HRESULT hResult = m_pIDDrawSurface->AddAttachedSurface(a);
	return hResult;	
}

HRESULT FakeIDDrawSurface7::AddOverlayDirtyRect(LPRECT a)
{
	return(m_pIDDrawSurface->AddOverlayDirtyRect(a));	
}

HRESULT FakeIDDrawSurface7::Blt(LPRECT a,LPDIRECTDRAWSURFACE7 b, LPRECT c,DWORD d, LPDDBLTFX e)
{
	/*if (g_dwWindowed && g_ddsBackBuffer == b)
	{
		FrameLimiter();
	}*/

	FrameLimiter();

	return(m_pIDDrawSurface->Blt(a, b, c, d, e));	
}


HRESULT FakeIDDrawSurface7::BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c)
{
	return(m_pIDDrawSurface->BltBatch(a, b, c));	
}


HRESULT FakeIDDrawSurface7::BltFast(DWORD a,DWORD b,LPDIRECTDRAWSURFACE7 c, LPRECT d,DWORD e)
{
	return(m_pIDDrawSurface->BltFast(a, b, c, d, e));	
}


HRESULT FakeIDDrawSurface7::DeleteAttachedSurface(DWORD a,LPDIRECTDRAWSURFACE7 b)
{
	return(m_pIDDrawSurface->DeleteAttachedSurface(a,((FakeIDDrawSurface7*)b)->m_pIDDrawSurface));	
}


HRESULT FakeIDDrawSurface7::EnumAttachedSurfaces(LPVOID a,LPDDENUMSURFACESCALLBACK7 b)
{
	return(m_pIDDrawSurface->EnumAttachedSurfaces(a, b));	
}


HRESULT FakeIDDrawSurface7::EnumOverlayZOrders(DWORD a,LPVOID b,LPDDENUMSURFACESCALLBACK7 c)
{
	return(m_pIDDrawSurface->EnumOverlayZOrders(a, b, c));	
}


HRESULT FakeIDDrawSurface7::Flip(LPDIRECTDRAWSURFACE7 a, DWORD b)
{	
	FrameLimiter();
	
	HRESULT hResult = m_pIDDrawSurface->Flip(a, b);
	return hResult;	
}


HRESULT FakeIDDrawSurface7::GetAttachedSurface(LPDDSCAPS2 lpDDSCaps, LPDIRECTDRAWSURFACE7 FAR * lplpDDAttachedSurface)
{
	HRESULT hResult = m_pIDDrawSurface->GetAttachedSurface(lpDDSCaps, lplpDDAttachedSurface);
	return hResult;	
}


HRESULT FakeIDDrawSurface7::GetBltStatus(DWORD a)
{
	return(m_pIDDrawSurface->GetBltStatus(a));	
}


HRESULT FakeIDDrawSurface7::GetCaps(LPDDSCAPS2 a)
{
	return(m_pIDDrawSurface->GetCaps(a));	
}


HRESULT FakeIDDrawSurface7::GetClipper(LPDIRECTDRAWCLIPPER FAR* a)
{
	return(m_pIDDrawSurface->GetClipper(a));	
}


HRESULT FakeIDDrawSurface7::GetColorKey(DWORD a, LPDDCOLORKEY b)
{
	return(m_pIDDrawSurface->GetColorKey(a, b));	
}


HRESULT FakeIDDrawSurface7::GetDC(HDC FAR * a)
{
	return(m_pIDDrawSurface->GetDC(a));	
}


HRESULT FakeIDDrawSurface7::GetFlipStatus(DWORD a)
{
	return(m_pIDDrawSurface->GetFlipStatus(a));	
}


HRESULT FakeIDDrawSurface7::GetOverlayPosition(LPLONG a, LPLONG b)
{
	return(m_pIDDrawSurface->GetOverlayPosition(a, b));	
}


HRESULT FakeIDDrawSurface7::GetPalette(LPDIRECTDRAWPALETTE FAR* a)
{
	return(m_pIDDrawSurface->GetPalette(a));	
}


HRESULT FakeIDDrawSurface7::GetPixelFormat(LPDDPIXELFORMAT a)
{
	HRESULT hResult = m_pIDDrawSurface->GetPixelFormat(a);
	return hResult;	
}


HRESULT FakeIDDrawSurface7::GetSurfaceDesc(LPDDSURFACEDESC2 a)
{
	HRESULT hResult = m_pIDDrawSurface->GetSurfaceDesc(a);
	return hResult;	
}


HRESULT FakeIDDrawSurface7::Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC2 b)
{
	return(m_pIDDrawSurface->Initialize(a, b));	
}


HRESULT FakeIDDrawSurface7::IsLost()
{
	return(m_pIDDrawSurface->IsLost());	
}


HRESULT FakeIDDrawSurface7::Lock(LPRECT lpDestRect,LPDDSURFACEDESC2 lpDDSurfaceDesc,DWORD dwFlags ,HANDLE hEvent)
{
	HRESULT hResult = m_pIDDrawSurface->Lock(lpDestRect, lpDDSurfaceDesc, dwFlags, hEvent);

	return hResult;	
}


HRESULT FakeIDDrawSurface7::ReleaseDC(HDC a)
{
	return(m_pIDDrawSurface->ReleaseDC(a));	
}


HRESULT FakeIDDrawSurface7::Restore()
{
	return(m_pIDDrawSurface->Restore());	
}


HRESULT FakeIDDrawSurface7::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
	HRESULT hResult = m_pIDDrawSurface->SetClipper(lpDDClipper);
	return hResult;	
}


HRESULT FakeIDDrawSurface7::SetColorKey(DWORD a, LPDDCOLORKEY b)
{
	return(m_pIDDrawSurface->SetColorKey(a, b));	
}


HRESULT FakeIDDrawSurface7::SetOverlayPosition(LONG a, LONG b)
{
	return(m_pIDDrawSurface->SetOverlayPosition(a, b));	
}


HRESULT FakeIDDrawSurface7::SetPalette(LPDIRECTDRAWPALETTE a)
{
	return(m_pIDDrawSurface->SetPalette(a));	
}


HRESULT FakeIDDrawSurface7::Unlock(LPRECT lpRect)
{
	HRESULT hResult = m_pIDDrawSurface->Unlock(lpRect);	
	return hResult;	
}


HRESULT FakeIDDrawSurface7::UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE7 b,LPRECT c ,DWORD d, LPDDOVERLAYFX e)
{
	return(m_pIDDrawSurface->UpdateOverlay(a, ((FakeIDDrawSurface7*)b)->m_pIDDrawSurface, c, d, e));	
}


HRESULT FakeIDDrawSurface7::UpdateOverlayDisplay(DWORD a)
{
	return(m_pIDDrawSurface->UpdateOverlayDisplay(a));	
}


HRESULT FakeIDDrawSurface7::UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE7 b)
{
	return(m_pIDDrawSurface->UpdateOverlayZOrder(a, ((FakeIDDrawSurface7*)b)->m_pIDDrawSurface));	
}



HRESULT FakeIDDrawSurface7::GetDDInterface(LPVOID FAR * a)
{
	return(m_pIDDrawSurface->GetDDInterface(a));	
}


HRESULT FakeIDDrawSurface7::PageLock(DWORD a)
{
	return(m_pIDDrawSurface->PageLock(a));	
}


HRESULT FakeIDDrawSurface7::PageUnlock(DWORD a)
{
	return(m_pIDDrawSurface->PageUnlock(a));	
}



HRESULT FakeIDDrawSurface7::SetSurfaceDesc(LPDDSURFACEDESC2 a, DWORD b)
{
	return(m_pIDDrawSurface->SetSurfaceDesc(a, b));	
}



HRESULT FakeIDDrawSurface7::SetPrivateData(REFGUID a, LPVOID b, DWORD c, DWORD d)
{
	return(m_pIDDrawSurface->SetPrivateData(a, b, c, d));	
}


HRESULT FakeIDDrawSurface7::GetPrivateData(REFGUID a, LPVOID b, LPDWORD c)
{
	return(m_pIDDrawSurface->GetPrivateData(a, b, c));	
}


HRESULT FakeIDDrawSurface7::FreePrivateData(REFGUID a)
{
	return(m_pIDDrawSurface->FreePrivateData(a));	
}


HRESULT FakeIDDrawSurface7::GetUniquenessValue(LPDWORD a)
{
	return(m_pIDDrawSurface->GetUniquenessValue(a));	
}


HRESULT FakeIDDrawSurface7::ChangeUniquenessValue()
{
	return(m_pIDDrawSurface->ChangeUniquenessValue());	
}



HRESULT FakeIDDrawSurface7::SetPriority(DWORD a)
{
	HRESULT hResult = m_pIDDrawSurface->SetPriority(a);
	return hResult;	
}


HRESULT FakeIDDrawSurface7::GetPriority(LPDWORD a)
{
	return(m_pIDDrawSurface->GetPriority(a));	
}


HRESULT FakeIDDrawSurface7::SetLOD(DWORD a)
{
	return(m_pIDDrawSurface->SetLOD(a));	
}


HRESULT FakeIDDrawSurface7::GetLOD(LPDWORD a)
{
	return(m_pIDDrawSurface->GetLOD(a));	
}

