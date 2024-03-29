#include "StdAfx.h"
#include "ThemeHelper.h"

// theme function definitions
typedef BOOL (STDAPICALLTYPE *PFNISTHEMEACTIVE)();
typedef DWORD (STDAPICALLTYPE *PFNGETTHEMEAPPPROPERTIES)();
typedef HTHEME (STDAPICALLTYPE *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (STDAPICALLTYPE *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
														 const RECT *pRect, const RECT *pClipRect);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEPARENTBACKGROUND)(HWND hWnd, HDC hdc, RECT *pRect);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
												   LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
												   DWORD dwTextFlags2, const RECT *pRect);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEEDGE)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
												   const RECT *pDestRect, UINT uEdge, UINT uFlags, 
												   RECT *pContentRect);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEICON)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
												   const RECT *pRect, HIMAGELIST himl, int iImageIndex);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEBORDER)(HTHEME hTheme, HDC hdc, int iStateId, const RECT *pRect);
typedef HRESULT (STDAPICALLTYPE *PFNGETTHEMEPARTSIZE)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId,
													  RECT *prc, THEMESIZE eSize, SIZE *psz);
typedef HRESULT (STDAPICALLTYPE *PFNGETTHEMETEXTEXTENT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
														LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
														const RECT *pBoundingRect, RECT *pExtentRect);
typedef HRESULT (STDAPICALLTYPE *PFNGETTHEMECOLOR)(HTHEME hTheme, int iPartId, int iStateId, int iPropID,
												   COLORREF* pColor);
typedef HRESULT (STDAPICALLTYPE *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme, HDC hdc, int iPartId,
																   int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect);


#define WSTR(string) (LPCWSTR)COleVariant(string).bstrVal

#ifndef DFCS_TRANSPARENT
#	define DFCS_TRANSPARENT        0x0800
#	define DFCS_HOT                0x1000
#	define DFC_POPUPMENU           5
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HMODULE CThemeHelper::s_hUxTheme = HMODULE(-1);

CThemeHelper::CThemeHelper(CWnd* pWnd, LPCTSTR szClassList) : m_hWnd(NULL), m_hTheme(NULL)
{
	InitUxTheme();

	if (pWnd && pWnd->GetSafeHwnd())
		Open(pWnd, szClassList);
}

CThemeHelper::~CThemeHelper()
{
	Close();
}

BOOL CThemeHelper::InitUxTheme()
{
	if (s_hUxTheme == HMODULE(-1)) // one time only
		s_hUxTheme = LoadLibrary(_T("UxTheme.dll"));

	return (NULL != s_hUxTheme);
}

BOOL CThemeHelper::SupportsTheming(int nFlag)
{
	return (IsThemeActive() && (GetAppThemeProperties() & nFlag));
}

BOOL CThemeHelper::Open(const CWnd* pWnd, LPCTSTR szClassList)
{
	if (pWnd && pWnd->GetSafeHwnd())
	{
		if (!s_hUxTheme)
		{
			s_hUxTheme = LoadLibrary(_T("UxTheme.dll"));

			if (!s_hUxTheme)
				return FALSE;
		}
		else
			Close();

		CString sClassList(szClassList);

		if (sClassList.IsEmpty())
		{
			::GetClassName(pWnd->GetSafeHwnd(), sClassList.GetBuffer(128), 128);
			sClassList.ReleaseBuffer();
		}

		m_hTheme = OpenThemeData(pWnd->GetSafeHwnd(), WSTR(sClassList));
		m_hWnd = pWnd->GetSafeHwnd();

		return (NULL != m_hTheme);
	}

	return FALSE;
}

BOOL CThemeHelper::Close()
{
	if (CloseThemeData(m_hTheme))
		m_hTheme = NULL;

	return (NULL == m_hTheme);
}

BOOL CThemeHelper::AreControlsThemed()
{
	return SupportsTheming(STAP_ALLOW_CONTROLS);
}

BOOL CThemeHelper::IsNonClientThemed()
{
	return SupportsTheming(STAP_ALLOW_NONCLIENT);
}

BOOL CThemeHelper::IsWebContentThemed()
{
	return SupportsTheming(STAP_ALLOW_WEBCONTENT);
}

// static 
BOOL CThemeHelper::DrawFrameControl(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, LPCRECT pClip)
{
	CThemeHelper th;

	if (th.SupportsTheming(STAP_ALLOW_CONTROLS) && th.SupportsTheming(STAP_ALLOW_NONCLIENT))
	{
		// determine the appropriate class, part and state
		CString sThClass;
		int nThPart = 0, nThState = 0;

		if (!th.GetThemeClassPartState(nType, nState, sThClass, nThPart, nThState))
			return FALSE;

		if (!th.Open(pWnd, sThClass))
			return FALSE;

		th.DrawParentBackground(pWnd, pDC, (LPRECT)(pClip ? pClip : pRect));
		th.DrawBackground(pDC, nThPart, nThState, pRect, pClip);

		return TRUE;
	}
	else
		return pDC->DrawFrameControl(pRect, nType, nState);
}

BOOL CThemeHelper::DrawEdge(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, UINT nEdge, UINT nFlags)
{
	CThemeHelper th;

	if (th.SupportsTheming(STAP_ALLOW_CONTROLS) && th.SupportsTheming(STAP_ALLOW_NONCLIENT))
	{
		// determine the appropriate class, part and state
		CString sThClass;
		int nThPart = 0, nThState = 0;

		if (!th.GetThemeClassPartState(nType, nState, sThClass, nThPart, nThState))
			return FALSE;

		if (!th.Open(pWnd, sThClass))
			return FALSE;

		th.DrawEdge(pDC, nThPart, nThState, pRect, nEdge, nFlags);

		return TRUE;
	}
	else
		return pDC->DrawFrameControl(pRect, nType, nState);
}

BOOL CThemeHelper::DrawBackground(CDC* pDC, int nPart, int nState, const CRect& rect, LPCRECT prClip)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeBackground(*pDC, nPart, nState, rect, prClip);
}

BOOL CThemeHelper::DrawParentBackground(const CWnd* pWnd, CDC* pDC, LPRECT pRect)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pWnd);

	return DrawThemeParentBackground(*pWnd, *pDC, pRect);
}

BOOL CThemeHelper::DrawText(CDC* pDC, int nPart, int nState, const CString& sText, DWORD dwTextFlags, DWORD dwTextFlags2, const CRect& rect)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeText(*pDC, nPart, nState, WSTR(sText), sText.GetLength(), dwTextFlags, dwTextFlags2, rect);
}

BOOL CThemeHelper::DrawEdge(CDC* pDC, int nPart, int nState, const CRect& rDest, UINT nEdge, UINT nFlags, LPRECT prContent)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeEdge(*pDC, nPart, nState, rDest, nEdge, nFlags, prContent);
}

BOOL CThemeHelper::DrawIcon(CDC* pDC, int nPart, int nState, const CRect& rect, const CImageList* pIL, int nImage)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pIL);

	return DrawThemeIcon(*pDC, nPart, nState, rect, *pIL, nImage);
}

BOOL CThemeHelper::DrawBorder(CDC* pDC, int nState, const CRect& rect)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeBorder(*pDC, nState, rect);
}

BOOL CThemeHelper::GetSize(int nPart, int nState, CSize& size, BOOL bMin)
{
	ASSERT (m_hTheme);

	return GetThemePartSize(nPart, nState, bMin ? TS_MIN : TS_TRUE, &size);
}

BOOL CThemeHelper::GetTextExtent(CDC* pDC, int nPart, int nState, const CString& sText, DWORD dwTextFlags, CRect& rExtent, LPCRECT prBounding)
{
	ASSERT (m_hTheme);

	return GetThemeTextExtent(pDC ? *pDC : (HDC)NULL, nPart, nState, WSTR(sText), sText.GetLength(), dwTextFlags, prBounding, rExtent);
}

BOOL CThemeHelper::BuildImageList(CImageList& il, int nPart, const int nStates[], int nNumStates, COLORREF crMask)
{
	ASSERT (!il.GetSafeHandle());

	if (il.GetSafeHandle())
		return FALSE;

	if (!nNumStates)
		return FALSE;

	CSize size;
	GetSize(nPart, 1, size);

	// little cheat for check boxes
	const UINT BORDER = (nPart == BP_CHECKBOX) ? 1 : 0;

	// create a bitmap containing the required images
	HWND hWnd = m_hWnd ? m_hWnd : GetDesktopWindow();
	CClientDC dc(CWnd::FromHandle(hWnd));
	CDC dcBack;

	if (dcBack.CreateCompatibleDC(&dc))
	{
		CBitmap bitmap;

		int nImWidth = (size.cx + 2 * BORDER);
		int nImHeight = (size.cy + 2 * BORDER);

		if (bitmap.CreateCompatibleBitmap(&dc, nImWidth * nNumStates, nImHeight))
		{
			CGdiObject* pOld = dcBack.SelectObject(&bitmap);
			dcBack.FillSolidRect(0, 0, nImWidth * nNumStates, nImHeight, crMask);

			CRect rState(BORDER, BORDER, size.cx + BORDER, size.cy + BORDER);

			for (int nState = 0; nState < nNumStates; nState++)
			{
				if (nStates[nState] != -1)
					DrawBackground(&dcBack, nPart, nStates[nState], rState);

				// next state
				rState.OffsetRect(nImWidth, 0);
			}

			dcBack.SelectObject(pOld);

			// create imagelist
			if (il.Create(nImWidth, nImHeight, ILC_COLOR32 | ILC_MASK, 0, 1))
				il.Add(&bitmap, crMask);
		}
	}

	return (il.GetSafeHandle() != NULL);
}

COLORREF CThemeHelper::GetThemeColor(int nPart, int nState, int nProp)
{
	ASSERT (m_hTheme);

	COLORREF color;

	if (GetThemeColor(nPart, nState, nProp, &color))
		return color;

	return 0; // black
}

BOOL CThemeHelper::GetThemeBackgroundContentRect(CDC* pDC, int nPart, int nState, const CRect& rBounding, CRect& rContent)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return GetThemeBackgroundContentRect(pDC->GetSafeHdc(), nPart, nState, rBounding, rContent);
}

// -----------------------------------------------------------------------------------------------------------

BOOL CThemeHelper::IsThemeActive()
{
	if (s_hUxTheme)
	{
		PFNISTHEMEACTIVE fnIsThemeActive = (PFNISTHEMEACTIVE)GetProcAddress(s_hUxTheme, "IsThemeActive");

		if (fnIsThemeActive)
			return fnIsThemeActive();
	}

	return FALSE;
}

DWORD CThemeHelper::GetAppThemeProperties()
{
	if (s_hUxTheme)
	{
		PFNGETTHEMEAPPPROPERTIES fnGetThemeAppProperties = (PFNGETTHEMEAPPPROPERTIES)GetProcAddress(s_hUxTheme, "GetThemeAppProperties");

		if (fnGetThemeAppProperties)
			return fnGetThemeAppProperties();
	}

	return 0;
}

HTHEME CThemeHelper::OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	if (s_hUxTheme)
	{
		PFNOPENTHEMEDATA fnOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(s_hUxTheme, "OpenThemeData");

		if (fnOpenThemeData)
			return fnOpenThemeData(hwnd, pszClassList);
	}

	return NULL;
}

BOOL CThemeHelper::CloseThemeData(HTHEME hTheme)
{
	if (s_hUxTheme && hTheme)
	{
		PFNCLOSETHEMEDATA fnCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(s_hUxTheme, "CloseThemeData");

		if (fnCloseThemeData)
			return SUCCEEDED(fnCloseThemeData(m_hTheme));
	}

	return FALSE;
}

BOOL CThemeHelper::DrawThemeBackground(HDC hdc, int iPartId, int iStateId, const RECT *pRect,
								  const RECT *pClipRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEBACKGROUND fnDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProcAddress(s_hUxTheme, "DrawThemeBackground");

		if (fnDrawThemeBackground)
			return (SUCCEEDED(fnDrawThemeBackground(m_hTheme, hdc, iPartId, iStateId, pRect, pClipRect)));
	}

	return FALSE;
}

BOOL CThemeHelper::DrawThemeParentBackground(HWND hWnd, HDC hdc, RECT *pRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEPARENTBACKGROUND fnDrawThemeParentBackground = (PFNDRAWTHEMEPARENTBACKGROUND)GetProcAddress(s_hUxTheme, "DrawThemeParentBackground");

		if (fnDrawThemeParentBackground)
			return (SUCCEEDED(fnDrawThemeParentBackground(hWnd, hdc, pRect)));
	}

	return FALSE;
}

BOOL CThemeHelper::DrawThemeText(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
							DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMETEXT fnDrawThemeText = (PFNDRAWTHEMETEXT)GetProcAddress(s_hUxTheme, "DrawThemeText");

		if (fnDrawThemeText)
			return (SUCCEEDED(fnDrawThemeText(m_hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect)));
	}

	return FALSE;
}

BOOL CThemeHelper::DrawThemeEdge(HDC hdc, int iPartId, int iStateId, const RECT *pDestRect, UINT uEdge, 
							UINT uFlags, RECT *pContentRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEEDGE fnDrawThemeEdge = (PFNDRAWTHEMEEDGE)GetProcAddress(s_hUxTheme, "DrawThemeEdge");

		if (fnDrawThemeEdge)
			return (SUCCEEDED(fnDrawThemeEdge(m_hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect)));
	}

	return FALSE;
}

BOOL CThemeHelper::DrawThemeIcon(HDC hdc, int iPartId, int iStateId, const RECT *pRect, HIMAGELIST himl, 
							int iImageIndex)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEICON fnDrawThemeIcon = (PFNDRAWTHEMEICON)GetProcAddress(s_hUxTheme, "DrawThemeIcon");

		if (fnDrawThemeIcon)
			return (SUCCEEDED(fnDrawThemeIcon(m_hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex)));
	}

	return FALSE;
}

BOOL CThemeHelper::DrawThemeBorder(HDC hdc, int iStateId, const RECT *pRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEBORDER fnDrawThemeBorder = (PFNDRAWTHEMEBORDER)GetProcAddress(s_hUxTheme, "DrawThemeBorder");

		if (fnDrawThemeBorder)
			return (SUCCEEDED(fnDrawThemeBorder(m_hTheme, hdc, iStateId, pRect)));
	}

	return FALSE;
}

BOOL CThemeHelper::GetThemePartSize(int iPartId, int iStateId, THEMESIZE eSize, SIZE *psz)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNGETTHEMEPARTSIZE fnGetThemePartSize = (PFNGETTHEMEPARTSIZE)GetProcAddress(s_hUxTheme, "GetThemePartSize");

		if (fnGetThemePartSize)
			return (SUCCEEDED(fnGetThemePartSize(m_hTheme, NULL, iPartId, iStateId, NULL, eSize, psz)));
	}

	return FALSE;
}

BOOL CThemeHelper::GetThemeTextExtent(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
								 DWORD dwTextFlags, const RECT *pBoundingRect, RECT *pExtentRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNGETTHEMETEXTEXTENT fnGetThemeTextExtent = (PFNGETTHEMETEXTEXTENT)GetProcAddress(s_hUxTheme, "GetThemeTextExtent");

		if (fnGetThemeTextExtent)
			return (SUCCEEDED(fnGetThemeTextExtent(m_hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, pBoundingRect, pExtentRect)));
	}

	return FALSE;
}

BOOL CThemeHelper::GetThemeColor(int iPartId, int iStateId, int iPropId, COLORREF *pColor)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNGETTHEMECOLOR fnGetThemeColor = (PFNGETTHEMECOLOR)GetProcAddress(s_hUxTheme, "GetThemeColor");

		if (fnGetThemeColor)
			return (SUCCEEDED(fnGetThemeColor(m_hTheme, iPartId, iStateId, iPropId, pColor)));
	}

	return FALSE;
}

BOOL CThemeHelper::GetThemeBackgroundContentRect(HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNGETTHEMEBACKGROUNDCONTENTRECT fnGetContentRect = (PFNGETTHEMEBACKGROUNDCONTENTRECT)
			GetProcAddress(s_hUxTheme, "GetThemeBackgroundContentRect");

		if (fnGetContentRect)
			return (SUCCEEDED(fnGetContentRect(m_hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect)));
	}

	return FALSE;
}

// -----------------------------------------------------------------------------------------------------------

BOOL CThemeHelper::GetThemeClassPartState(int nType, int nState, CString& sThClass, int& nThPart, int& nThState)
{
	sThClass.Empty();
	nThPart = 0;
	nThState = 0;

	switch (nType)
	{
	case DFC_BUTTON:
		{
			sThClass = "BUTTON";
			nThState = PBS_NORMAL;

			if (nState & DFCS_BUTTONPUSH) 
			{
				nThPart = BP_PUSHBUTTON;

				if (nState & (DFCS_CHECKED | DFCS_PUSHED))
					nThState = PBS_PRESSED;

				else if (nState & DFCS_INACTIVE)
					nThState = PBS_DISABLED;

				else if (nState & DFCS_HOT)
					nThState = PBS_HOT;
			}
			/*			else if (nState & DFCS_BUTTONRADIO) 
			{
			nThPart = BP_RADIOBUTTON;
			}*/
			else if ((nState & DFCS_BUTTONCHECK) == DFCS_BUTTONCHECK) 
			{
				nThPart = BP_CHECKBOX;

				if (nState & (DFCS_CHECKED | DFCS_PUSHED))
				{
					if (nState & DFCS_INACTIVE)
						nThState = CBS_CHECKEDDISABLED;

					else if (nState & DFCS_HOT)
						nThState = CBS_CHECKEDHOT;
					else
						nThState = CBS_CHECKEDNORMAL;
				}
				else
				{
					if (nState & DFCS_INACTIVE)
						nThState = CBS_UNCHECKEDDISABLED;

					else if (nState & DFCS_HOT)
						nThState = CBS_UNCHECKEDHOT;
					else
						nThState = CBS_UNCHECKEDNORMAL;
				}
			}
			else 
				return FALSE;
		}
		break;

	case DFC_CAPTION:
		break;

	case DFC_MENU:
		break;

	case DFC_POPUPMENU:
		break;

	case DFC_SCROLL:
		{
			if (nState & DFCS_SCROLLCOMBOBOX) 
			{
				sThClass = "COMBOBOX";
				nThPart = CP_DROPDOWNBUTTON;
				nThState = CBXS_NORMAL;

				if (nState & (DFCS_CHECKED | DFCS_PUSHED))
					nThState = CBXS_PRESSED;

				else if (nState & DFCS_INACTIVE)
					nThState = CBXS_DISABLED;

				else if (nState & DFCS_HOT)
					nThState = CBXS_HOT;
			}
			else if (nState & DFCS_SCROLLSIZEGRIP)
			{
				sThClass = "SCROLLBAR";
				nThPart = SBP_SIZEBOX;
				nThState = (nState & DFCS_SCROLLLEFT) ? SZB_LEFTALIGN : SZB_RIGHTALIGN;
			}
			// else
		}
		break;
	}

	return (!sThClass.IsEmpty() && (nThPart && nThState));
}
