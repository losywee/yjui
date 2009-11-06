// CYJButton.cpp : 实现文件
//

#include "stdafx.h"
#include "YJButton.h"
#include "CreateGrayscaleIcon.h"


// CYJButton

IMPLEMENT_DYNAMIC(CYJButton, CButton)
CYJButton::CYJButton()
{
	m_hIcon				= NULL;
	m_hGrayIcon			= NULL;
	m_hIconHover		= NULL;
	m_hIconPressed		= NULL;
	m_hMenu				= NULL;
	m_hCursor			= AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hParentWndMenu	= NULL;
	m_bIsDrawBorder		= TRUE;
	m_bIsCheckBox		= FALSE;
	m_bIsChecked		= FALSE;
	m_bIsFat			= FALSE;
	m_bTracking			= FALSE;
	m_bMouseOver		= FALSE;
	m_eIconAlignment	= ICON_LEFT;
	m_bIsDrawTransparent= FALSE;
	m_ptPressOffset		= CPoint(1,1);
	m_bMenuDisplayed	= FALSE;
	m_dwToolTipStyle	= 0;
	m_bActiveTips		= FALSE;
	m_clrBackground		= GetSysColor(COLOR_BTNFACE);
	m_clrText			= 0;
	m_clrBorderNormal = m_clrBorderHover = m_clrBorderPressed = RGB(0,0,0);
	m_bEnablePress		= TRUE;
	::ZeroMemory(&m_csMenuCallback, sizeof(m_csMenuCallback));
	::ZeroMemory(&m_csBitmaps, sizeof(m_csBitmaps));
}

CYJButton::~CYJButton()
{
	if (m_hIcon)
		DestroyIcon(m_hIcon);
	m_hIcon = NULL;

	if (m_hGrayIcon)
		DestroyIcon(m_hGrayIcon);
	m_hGrayIcon = NULL;

	if (m_hIconHover)
		DestroyIcon(m_hIconHover);
	m_hIconHover = NULL;

	if (m_hIconPressed)
		DestroyIcon(m_hIconPressed);
	m_hIconPressed = NULL;

	if (m_hCursor)
		DestroyCursor(m_hCursor);
	m_hCursor = NULL;

	if (m_hMenu)
		DestroyMenu(m_hMenu);
	m_hMenu = NULL;

	if (m_csBitmaps[0].hBitmap)
		::DeleteObject(m_csBitmaps[0].hBitmap);
	if (m_csBitmaps[1].hBitmap)
		::DeleteObject(m_csBitmaps[1].hBitmap);
	if (m_csBitmaps[2].hBitmap)
		::DeleteObject(m_csBitmaps[1].hBitmap);
	if (m_csBitmaps[3].hBitmap)
		::DeleteObject(m_csBitmaps[3].hBitmap);

	// Destroy mask bitmaps
	if (m_csBitmaps[0].hMask)
		::DeleteObject(m_csBitmaps[0].hMask);
	if (m_csBitmaps[1].hMask)
		::DeleteObject(m_csBitmaps[1].hMask);
	if (m_csBitmaps[2].hMask)
		::DeleteObject(m_csBitmaps[2].hMask);
	if (m_csBitmaps[3].hMask)
		::DeleteObject(m_csBitmaps[3].hMask);

	// Restore old bitmap (if any)
	if (m_dcMemBk.m_hDC && m_pbmpOldBk)
	{
		m_dcMemBk.SelectObject(m_pbmpOldBk);
	}
}


BEGIN_MESSAGE_MAP(CYJButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_SETCURSOR()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnBnClicked)
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)
END_MESSAGE_MAP()


//=============================================================================	
// Sets icon from resource id
void CYJButton::SetIcon(BUTTON_STATUS it, UINT nIDResource, int nWidth, int nHeight, BOOL bRepaint /* = TRUE*/)
{
	HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(nIDResource),
		IMAGE_ICON,
		nWidth,
		nHeight,
		0);

	SetIcon(it, hIcon, bRepaint);
}
//=============================================================================	
// Sets icon from HICON
void CYJButton::SetIcon(BUTTON_STATUS it, HICON hIcon, BOOL bRepaint /* = TRUE*/)
{
	switch (it)
	{
	case STATUS_NORMAL:
		if (m_hIcon)
			DestroyIcon(m_hIcon);
		m_hIcon = hIcon;

		// create grayscale icon for disabled button
		if (m_hGrayIcon)
			DestroyIcon(m_hGrayIcon);
		m_hGrayIcon = NULL;
		if (hIcon)
			m_hGrayIcon = CreateGrayscaleIcon(hIcon);
		break;
	case STATUS_HOVER:
		if (m_hIconHover)
			DestroyIcon(m_hIconHover);

		m_hIconHover = hIcon;
		break;
	case STATUS_PRESSED:
		if (m_hIconPressed)
			DestroyIcon(m_hIconPressed);

		m_hIconPressed = hIcon;
		break;
	}

	if (bRepaint)
		RedrawWindow();
}
//=============================================================================	
// Set icon's alignment
void CYJButton::SetIconAlignment(ICON_ALIGNMENT ia, BOOL bRepaint /* = TRUE*/)
{ 
	m_eIconAlignment = ia; 
	if (bRepaint)
		RedrawWindow();
}

//=============================================================================	
void CYJButton::PrepareImageRect(CRect& rectItem,CRect& rectText,BOOL bHasText,BOOL bIsPressed,DWORD dwWidth,DWORD dwHeight,CRect& rectImage)
{
	rectImage.CopyRect(&rectItem);
	if (bHasText)
	{
		int nTextWidth = rectText.right - rectText.left;
		if (m_eIconAlignment == ICON_RIGHT)
		{
			rectText.right = nTextWidth - dwWidth - 5;
			rectText.left  = 5;
			rectImage.left = rectItem.right - dwWidth - 5;
		}
		else	// icon is on the left
		{
			rectText.right = nTextWidth - 5;
			rectText.left  = dwWidth + 10;
			rectImage.left = 5;
		}
	}
	else
	{
		// center image horizontally
		int nImageWidth = rectImage.right - rectImage.left;
		rectImage.left += ((nImageWidth - dwWidth)/2);
	}

	// center image vertically
	int nImageHeight = rectImage.bottom - rectImage.top;
	rectImage.top += ((nImageHeight - dwHeight)/2);

	// if button is pressed then press image also
	if (bIsPressed)
		rectImage.OffsetRect(m_ptPressOffset);

}
//=============================================================================	
// get icon's size
CSize CYJButton::GetIconSize(HICON hIcon)
{
	DWORD dwWidth = 32;		// assume 32x32
	DWORD dwHeight = 32;
	ICONINFO iconinfo;

	if (GetIconInfo(m_hIcon, &iconinfo))
	{
		CBitmap* pBitmap = CBitmap::FromHandle(iconinfo.hbmColor);
		if (pBitmap)
		{
			BITMAP bm;
			pBitmap->GetBitmap(&bm);
			dwWidth = bm.bmWidth;
			dwHeight = bm.bmHeight;
		}

		if (iconinfo.hbmColor) 
			::DeleteObject(iconinfo.hbmColor);
		if (iconinfo.hbmMask) 
			::DeleteObject(iconinfo.hbmMask);
	}
	return CSize(dwWidth,dwHeight);
}
//=============================================================================	
void CYJButton::DrawIcon(CDC *pDC,
						 CRect& rectItem,		// from LPDRAWITEMSTRUCT
						 CRect& rectText,
						 BOOL bHasText,
						 BOOL bIsPressed,
						 BOOL bIsDisabled)
//=============================================================================	
{
	if (!m_bMouseOver || bIsDisabled)
	{
		if (m_hIcon == NULL)
			return;
		// first get size of icon
		CSize sizeIcon = GetIconSize(m_hIcon);
		
		CRect rectImage;
		PrepareImageRect(rectItem,rectText,bHasText,bIsPressed,sizeIcon.cx,sizeIcon.cy,rectImage);

		HICON hIcon = m_hIcon;
		UINT nFlags = bIsDisabled ? DSS_DISABLED : DSS_NORMAL;

		if (bIsDisabled && m_hGrayIcon)
		{
			hIcon = m_hGrayIcon;
			nFlags = DSS_NORMAL;
		}

		nFlags |= DST_ICON;

		pDC->DrawState(CPoint(rectImage.left, rectImage.top), sizeIcon, hIcon, nFlags, (CBrush *) NULL);
	}
	else if (!bIsPressed)
	{
		HICON hIcon = m_hIcon;
		if (m_hIconHover)
			hIcon = m_hIconHover;

		if (hIcon == NULL)
			return;

		// first get size of icon
		CSize sizeIcon = GetIconSize(hIcon);

		CRect rectImage;
		PrepareImageRect(rectItem,rectText,bHasText,bIsPressed,sizeIcon.cx,sizeIcon.cy,rectImage);

		UINT nFlags = DSS_NORMAL | DST_ICON;
		pDC->DrawState(CPoint(rectImage.left, rectImage.top), sizeIcon, hIcon, nFlags, (CBrush *) NULL);
	}
	else
	{
		HICON hIcon = m_hIcon;
		if (m_hIconPressed)
			hIcon = m_hIconPressed;
		else if (m_hIconHover)
			hIcon = m_hIconHover;

		if (hIcon == NULL)
			return;

		// first get size of icon
		CSize sizeIcon = GetIconSize(hIcon);

		CRect rectImage;
		PrepareImageRect(rectItem,rectText,bHasText,bIsPressed,sizeIcon.cx,sizeIcon.cy,rectImage);

		UINT nFlags = DSS_NORMAL | DST_ICON;
		pDC->DrawState(CPoint(rectImage.left, rectImage.top), sizeIcon, hIcon, nFlags, (CBrush *) NULL);
	}
}

//=============================================================================	
// paint background
void CYJButton::PaintBk(CDC* pDC)
{
	if (m_bIsDrawTransparent)
	{
		CRect rect;
		GetClientRect(rect);

		if (m_dcMemBk.m_hDC == NULL)
		{
			CClientDC clDC(GetParent());
			CRect rect1;

			GetWindowRect(rect1);
			GetParent()->ScreenToClient(rect1);

			m_dcMemBk.CreateCompatibleDC(&clDC);
			m_bmpBk.CreateCompatibleBitmap(&clDC, rect.Width(), rect.Height());
			m_pbmpOldBk = m_dcMemBk.SelectObject(&m_bmpBk);
			m_dcMemBk.BitBlt(0, 0, rect.Width(), rect.Height(), &clDC, rect1.left, rect1.top, SRCCOPY);
		}

		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_dcMemBk, 0, 0, SRCCOPY);
	}
	else
	{
// 		CBrush brBackground;
// 		brBackground.CreateSolidBrush(m_clrBackground);
// 
 		CRect rectClient;
 		GetClientRect(rectClient);
		//InflateRect(&rectClient, -1, -1);
		//pDC->FillRect(&rectClient, &brBackground);
		pDC->FillSolidRect(&rectClient, m_clrBackground);
	}
}

void CYJButton::DrawText(CDC *pDC, LPCTSTR lpszText, CRect& rectText, BOOL bIsPressed, BOOL bIsDisabled)
{
	ASSERT(pDC);
	
	if (_tcslen(lpszText) == 0)
		return;

	UINT uTextAlignment = DT_LEFT;
	if (m_hIcon == NULL)
		uTextAlignment = DT_CENTER;
	uTextAlignment |= DT_VCENTER;
	uTextAlignment |= (GetStyle() & BS_MULTILINE) ? DT_WORDBREAK : DT_SINGLELINE;
	// center text vertically (DT_VCENTER does not work if BS_MULTILINE is set)
	
	if (bIsDisabled)
	{
		rectText.OffsetRect(1, 1);
		pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
		pDC->DrawText(lpszText, -1, &rectText, uTextAlignment);
		rectText.OffsetRect(-1, -1);
		pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
		pDC->DrawText(lpszText, -1, &rectText, uTextAlignment);
	}
	else
	{
		if (bIsPressed)
			rectText.OffsetRect(m_ptPressOffset);

		if (m_clrText == ((COLORREF) -1))
			pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
		else
			pDC->SetTextColor(m_clrText);
		pDC->DrawText(lpszText, &rectText, uTextAlignment);
	}
}

void CYJButton::DrawBorder(CDC* pDC, CRect& rectItem, BOOL bIsPressed, BOOL bIsFocused, BOOL bIsDisabled)
{
	if (!m_bIsDrawBorder)
		return;

	if (m_bIsFat)
	{
		COLORREF clrBorder = m_clrBorderNormal;
		if (bIsPressed)
			clrBorder = m_clrBorderPressed;
		else if (m_bMouseOver)
			clrBorder = m_clrBorderHover;

		CPen pen(PS_SOLID, 1, clrBorder);
		CPen* pOldPen = pDC->SelectObject(&pen);
		HBRUSH hOldBrush = (HBRUSH)pDC->SelectObject(GetStockObject(NULL_BRUSH));
		pDC->Rectangle(&rectItem);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(hOldBrush);
	}
	else
	{
		if (bIsFocused)
		{
			CBrush brush(RGB(0,0,0));
			pDC->FrameRect(&rectItem, &brush);
			InflateRect(&rectItem, -1, -1);
		}

		pDC->DrawEdge(&rectItem, bIsPressed ? EDGE_SUNKEN:EDGE_RAISED, BF_RECT);
	}
}

void CYJButton::DrawFocus(CDC* pDC, CRect rectItem, BOOL bIsFocused)
{
	if (!bIsFocused || !m_bIsDrawFocus)
		return;

	if (m_bIsDrawBorder)
	{
		if (m_bIsFat)
			rectItem.InflateRect(-2, -2);
		else
			rectItem.InflateRect(-3, -3);
	}
	pDC->SetTextColor(0);
	pDC->DrawFocusRect(&rectItem);
}

void CYJButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC *pDC			= CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rectClient	= lpDrawItemStruct->rcItem;
	CRect rectItem		= lpDrawItemStruct->rcItem;
	CRect rectText		= lpDrawItemStruct->rcItem;
	CString strTitle	= _T("");
	GetWindowText(strTitle);
	BOOL bHasText       = !strTitle.IsEmpty();
	BOOL bIsFocused     = lpDrawItemStruct->itemState & ODS_FOCUS;
	BOOL bIsDisabled    = lpDrawItemStruct->itemState & ODS_DISABLED;
	BOOL bIsPressed		= lpDrawItemStruct->itemState & ODS_SELECTED;

	if (!m_bEnablePress)
		bIsPressed = FALSE;
	else if (m_hMenu && m_bMenuDisplayed)
	{
		bIsPressed = TRUE;
		m_bMouseOver = TRUE;
	}

	//init double buffering
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rectClient.Width(), rectClient.Height());
	CBitmap *pOldBitmap = memDC.SelectObject(&bmp);

	CFont *pFont = GetParent()->GetFont();
	if (pFont)
		memDC.SelectObject(pFont);
	memDC.SetBkMode(TRANSPARENT);

	PaintBk(&memDC);

	if (m_csBitmaps[0].hBitmap)
	{
		pDC->SetBkColor(RGB(255,255,255));
		DrawBitmap(&memDC,rectItem,rectText,bIsPressed,bIsDisabled);
	}

	DrawBorder(&memDC, rectItem, bIsPressed, bIsFocused, bIsDisabled);

	DrawIcon(&memDC, rectItem,rectText, bHasText, bIsPressed, bIsDisabled);

	DrawText(&memDC, strTitle, rectText, bIsPressed, bIsDisabled);

	// draw the focus rect
	DrawFocus(&memDC, rectItem, bIsFocused);

	//end double buffering
	pDC->BitBlt(0, 0, rectClient.Width(), rectClient.Height(),&memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
}

BOOL CYJButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CButton::OnEraseBkgnd(pDC);
}

void CYJButton::ShowBorder(BOOL isDrawBorder /* = TRUE */, BOOL bRepaint /* = TRUE*/)
{
	m_bIsDrawBorder = isDrawBorder;
	if (bRepaint)
		RedrawWindow();
}

void CYJButton::ShowFocus(BOOL isDrawFocus /* = TRUE */, BOOL bRepaint /* = TRUE*/)
{
	m_bIsDrawFocus = isDrawFocus;
	if (bRepaint)
		RedrawWindow();
}

void CYJButton::DrawTransparent(BOOL isDrawTransparent /* = TRUE */, BOOL bRepaint /* = TRUE */)
{
	m_bIsDrawTransparent = isDrawTransparent;

	if (!m_bIsDrawTransparent)
	{
		// Restore old bitmap (if any)
		if (m_dcMemBk.m_hDC != NULL && m_pbmpOldBk != NULL)
		{
			m_dcMemBk.SelectObject(m_pbmpOldBk);
		}
		m_bmpBk.DeleteObject();
		m_dcMemBk.DeleteDC();
	}
	if (bRepaint)
		RedrawWindow();
}

void CYJButton::SetBk(CDC* pDC)
{
	if (m_bIsDrawTransparent && pDC)
	{
		// Restore old bitmap (if any)
		if (m_dcMemBk.m_hDC != NULL && m_pbmpOldBk != NULL)
			m_dcMemBk.SelectObject(m_pbmpOldBk);
		m_bmpBk.DeleteObject();
		m_dcMemBk.DeleteDC();

		CRect rect;
		CRect rect1;

		GetClientRect(rect);
		GetWindowRect(rect1);
		GetParent()->ScreenToClient(rect1);

		m_dcMemBk.CreateCompatibleDC(pDC);
		m_bmpBk.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		m_pbmpOldBk = m_dcMemBk.SelectObject(&m_bmpBk);
		m_dcMemBk.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rect1.left, rect1.top, SRCCOPY);
	}
}

void CYJButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CYJButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = FALSE;
	m_bTracking = FALSE;
	InvalidateRect(NULL, FALSE);

	if (m_ctrlToolTip.m_hWnd != NULL) {
		m_ctrlToolTip.DelTool(this, 1);
		if (m_strTipText.IsEmpty() != TRUE) {
			CRect rectBtn;
			GetClientRect(rectBtn);
			m_ctrlToolTip.AddTool(this, m_strTipText, rectBtn, 1);
			m_ctrlToolTip.Activate(m_bActiveTips);
		}
	}

	return 0;
}

LRESULT CYJButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bMouseOver = TRUE;
	InvalidateRect(NULL);
	return 0;
}

BOOL CYJButton::SetBtnCursor(UINT nIDResource)
{
	HINSTANCE	hInstResource = NULL;
	// Destroy any previous cursor
	if (m_hCursor)
	{
		DestroyCursor(m_hCursor);
		m_hCursor = NULL;
	}

	// Load cursor
	if (nIDResource)
	{
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nIDResource), RT_GROUP_CURSOR);
		// Load cursor resource
		m_hCursor = (HCURSOR)::LoadImage(hInstResource, MAKEINTRESOURCE(nIDResource), IMAGE_CURSOR, 0, 0, 0);
		
		// If something wrong
		if (m_hCursor == NULL)
			return FALSE;
	}

	return TRUE;
}

BOOL CYJButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_hCursor != NULL)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void CYJButton::SetPressOffset(CPoint ptOffset)
{
	m_ptPressOffset = ptOffset;
}

BOOL CYJButton::SetMenu(UINT nMenu, HWND hParentWnd, BOOL bRepaint)
{
	HINSTANCE	hInstResource	= NULL;

	// Destroy any previous menu
	if (m_hMenu)
	{
		::DestroyMenu(m_hMenu);
		m_hMenu = NULL;
		m_hParentWndMenu = NULL;
		m_bMenuDisplayed = FALSE;
	}

	// Load menu
	if (nMenu)
	{
		// Find correct resource handle
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nMenu), RT_MENU);
		// Load menu resource
		m_hMenu = ::LoadMenu(hInstResource, MAKEINTRESOURCE(nMenu));
		m_hParentWndMenu = hParentWnd;
		// If something wrong
		if (m_hMenu == NULL)
			return FALSE;
	} // if

	// Repaint the button
	if (bRepaint)
		Invalidate();

	return TRUE;
}

BOOL CYJButton::OnBnClicked()
{
	if (!m_bEnablePress) return FALSE;

	SetFocus();
	if (m_bIsCheckBox)
	{
		m_bIsChecked = !m_bIsChecked;
		Invalidate();
		return FALSE;
	}

	if (m_hMenu)
	{
		CRect	rWnd;
		GetWindowRect(rWnd);

		m_bMenuDisplayed = TRUE;
		Invalidate();

		HMENU hSubMenu = ::GetSubMenu(m_hMenu, 0);
		if (m_csMenuCallback.hWnd)
			::SendMessage(m_csMenuCallback.hWnd, m_csMenuCallback.nMessage, (WPARAM)hSubMenu, m_csMenuCallback.lParam);
		DWORD dwRetValue = ::TrackPopupMenuEx(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, rWnd.left, rWnd.bottom, m_hParentWndMenu, NULL);

		m_bMenuDisplayed = FALSE;
		m_bMouseOver = FALSE;
		Invalidate();

		if (dwRetValue)
			::PostMessage(m_hParentWndMenu, WM_COMMAND, MAKEWPARAM(dwRetValue, 0), (LPARAM)NULL);
	}
	return FALSE;
}

void CYJButton::SetMenuCallback(HWND hWnd, UINT nMessage, LPARAM lParam)
{
	m_csMenuCallback.hWnd = hWnd;
	m_csMenuCallback.nMessage = nMessage;
	m_csMenuCallback.lParam = lParam;
}

BOOL CYJButton::SetBMP(BUTTON_STATUS it, UINT nIDResource, int x/* = 0*/, int y/* = 0*/, COLORREF crTransColor/* = -1*/, BOOL bRepaint /* = TRUE */)
{
	HBITMAP hBitmap = (HBITMAP) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIDResource),IMAGE_BITMAP,0,0,0);
	return SetBMP(it, hBitmap, x, y, crTransColor, bRepaint);
}

BOOL CYJButton::SetBMP(BUTTON_STATUS it, HBITMAP hBitmap, int x/* = 0*/, int y/* = 0*/, COLORREF crTransColor/* = -1*/, BOOL bRepaint /* = TRUE */)
{
	HBITMAP hMask = CreateBitmapMask(hBitmap, crTransColor);
	
	SetBMP(it, hBitmap, x, y, hMask, bRepaint);
	return TRUE;
}

BOOL CYJButton::SetBMP(BUTTON_STATUS it, HBITMAP hBitmap, int x /* = 0 */, int y /* = 0 */, HBITMAP hMask /* = NULL */, BOOL bRepaint /* = TRUE */)
{
	ASSERT(hBitmap);

	if (m_csBitmaps[it].hBitmap)
		::DeleteObject(m_csBitmaps[it].hBitmap);
	m_csBitmaps[it].hBitmap = hBitmap;
	m_csBitmaps[it].x		= x;
	m_csBitmaps[it].y		= y;

	BITMAP	bitmap;
	if (!::GetObject(hBitmap, sizeof(bitmap), &bitmap))
	{
		m_csBitmaps[it].hBitmap = NULL;
		return FALSE;
	}
	m_csBitmaps[it].bmWidth = bitmap.bmWidth;
	m_csBitmaps[it].bmHeight = bitmap.bmHeight;

	// create bitmap mask
	if (m_csBitmaps[it].hMask)
		::DeleteObject(m_csBitmaps[it].hMask);

	m_csBitmaps[it].hMask = hMask;

	if (bRepaint)
		RedrawWindow();
	return TRUE;
}

HBITMAP CYJButton::CreateBitmapMask(HBITMAP hSourceBitmap, COLORREF crTransColor)
{
	if (crTransColor == -1)
		return NULL;

	HBITMAP		hMask		= NULL;
	HDC			hdcSrc		= NULL;
	HDC			hdcDest		= NULL;
	HBITMAP		hbmSrcT		= NULL;
	HBITMAP		hbmDestT	= NULL;
	COLORREF	crSaveBk,crSaveDestText;
	DWORD		dwWidth,dwHeight;

	BITMAP	bitmap;
	if (!::GetObject(hSourceBitmap, sizeof(bitmap), &bitmap))
		return NULL;

	dwWidth = bitmap.bmWidth;
	dwHeight = bitmap.bmHeight;

	hMask = ::CreateBitmap(dwWidth, dwHeight, 1, 1, NULL);
	if (hMask == NULL)	return NULL;

	hdcSrc	= ::CreateCompatibleDC(NULL);
	hdcDest	= ::CreateCompatibleDC(NULL);

	hbmSrcT = (HBITMAP)::SelectObject(hdcSrc, hSourceBitmap);
	hbmDestT = (HBITMAP)::SelectObject(hdcDest, hMask);

	crSaveBk = ::SetBkColor(hdcSrc, crTransColor);

	::BitBlt(hdcDest, 0, 0, dwWidth, dwHeight, hdcSrc, 0, 0, SRCCOPY);

	crSaveDestText = ::SetTextColor(hdcSrc, RGB(255, 255, 255));
	::SetBkColor(hdcSrc,RGB(0, 0, 0));

	::BitBlt(hdcSrc, 0, 0, dwWidth, dwHeight, hdcDest, 0, 0, SRCAND);

	::SetTextColor(hdcDest, crSaveDestText);

	::SetBkColor(hdcSrc, crSaveBk);
	::SelectObject(hdcSrc, hbmSrcT);
	::SelectObject(hdcDest, hbmDestT);

	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);

	return hMask;
} // End of CreateBitmapMask


void CYJButton::DrawBitmap(CDC* pDC, CRect& rectItem,CRect& rectText, BOOL bIsPressed, BOOL bIsDisabled)
{
	HDC			hdcBmpMem	= NULL;
	HBITMAP		hbmOldBmp	= NULL;
	HDC			hdcMem		= NULL;
	HBITMAP		hbmT		= NULL;

	BYTE		byIndex		= 0;

	// Select the bitmap to use
	BOOL		bIsChecked	= FALSE;
	if (m_bIsCheckBox && m_bIsChecked)
		bIsChecked = TRUE;

	if (bIsDisabled)
		byIndex = 0;
	else if (!m_bMouseOver)
	{
		if (bIsChecked)
			byIndex = (m_csBitmaps[3].hBitmap == NULL ? (m_csBitmaps[2].hBitmap == NULL ? 0 : 2) : 3);
		else
			byIndex = 0;
	}
	else if (!bIsPressed)
	{
		if (bIsChecked && m_csBitmaps[4].hBitmap != NULL)
			byIndex = 4;
		else
			byIndex = (m_csBitmaps[1].hBitmap == NULL ? 0 : 1);
	}
	else
	{
		if (bIsChecked)
			byIndex = m_csBitmaps[5].hBitmap == NULL ? (m_csBitmaps[4].hBitmap == NULL ? (m_csBitmaps[3].hBitmap == NULL ? 0 : 3): 4):5;
		else
			byIndex = (m_csBitmaps[2].hBitmap == NULL ? (m_csBitmaps[1].hBitmap == NULL ? 0 : 1) : 2);

		
	}

// 	if (!m_bMouseOver || bIsDisabled)
// 		byIndex = 0;
// 	else if (m_bMouseOver && !bIsPressed)
// 		byIndex = (m_csBitmaps[1].hBitmap == NULL ? 0 : 1);
// 	else
// 		byIndex = (m_csBitmaps[2].hBitmap == NULL ? (m_csBitmaps[1].hBitmap == NULL ? 0 : 1) : 2);

//	CRect	rImage;
//	PrepareImageRect(rectItem,rectText,FALSE, bIsPressed, m_csBitmaps[byIndex].bmWidth, m_csBitmaps[byIndex].bmHeight, rImage);

	hdcBmpMem = ::CreateCompatibleDC(pDC->m_hDC);

	hbmOldBmp = (HBITMAP)::SelectObject(hdcBmpMem, m_csBitmaps[byIndex].hBitmap);

	hdcMem = ::CreateCompatibleDC(NULL);

	hbmT = (HBITMAP)::SelectObject(hdcMem, m_csBitmaps[byIndex].hMask);

	if (bIsDisabled)
	{
		HDC		hDC = NULL;
		HBITMAP	hBitmap = NULL;

		hDC = ::CreateCompatibleDC(pDC->m_hDC);
		hBitmap = ::CreateCompatibleBitmap(pDC->m_hDC, m_csBitmaps[byIndex].bmWidth, m_csBitmaps[byIndex].bmHeight);
		HBITMAP	hOldBmp2 = (HBITMAP)::SelectObject(hDC, hBitmap);

// 		RECT	rRect;
// 		rRect.left = 0;
// 		rRect.top = 0;
// 		rRect.right = rImage.right + 1;
// 		rRect.bottom = rImage.bottom + 1;
		::FillRect(hDC, &rectItem, (HBRUSH)RGB(255, 255, 255));

		COLORREF crOldColor = ::SetBkColor(hDC, RGB(255,255,255));

		::BitBlt(hDC, 0, 0, m_csBitmaps[byIndex].bmWidth, m_csBitmaps[byIndex].bmHeight, hdcMem, m_csBitmaps[byIndex].x, m_csBitmaps[byIndex].y, SRCAND);
		::BitBlt(hDC, 0, 0, m_csBitmaps[byIndex].bmWidth, m_csBitmaps[byIndex].bmHeight, hdcBmpMem, m_csBitmaps[byIndex].x, m_csBitmaps[byIndex].y, SRCPAINT);

		::SetBkColor(hDC, crOldColor);
		::SelectObject(hDC, hOldBmp2);
		::DeleteDC(hDC);

		pDC->DrawState(	CPoint(rectItem.left/*+1*/, rectItem.top), 
			CSize(m_csBitmaps[byIndex].bmWidth, m_csBitmaps[byIndex].bmHeight), 
			hBitmap, DST_BITMAP | DSS_DISABLED);

		::DeleteObject(hBitmap);
	} // if
	else
	{
		::BitBlt(pDC->m_hDC, rectItem.left, rectItem.top, m_csBitmaps[byIndex].bmWidth, m_csBitmaps[byIndex].bmHeight, hdcMem, m_csBitmaps[byIndex].x, m_csBitmaps[byIndex].y, SRCAND);

		::BitBlt(pDC->m_hDC, rectItem.left, rectItem.top, m_csBitmaps[byIndex].bmWidth, m_csBitmaps[byIndex].bmHeight, hdcBmpMem, m_csBitmaps[byIndex].x, m_csBitmaps[byIndex].y, SRCPAINT);
	} // else

	::SelectObject(hdcMem, hbmT);
	::DeleteDC(hdcMem);

	::SelectObject(hdcBmpMem, hbmOldBmp);
	::DeleteDC(hdcBmpMem);
} // End of DrawTheBitmap

void CYJButton::SetTextColor(COLORREF clrText, BOOL bRepaint /* = TRUE */)
{
	m_clrText = clrText;
	if (bRepaint)
		Invalidate();
}

void CYJButton::EnableBalloonTooltip()
{
	m_dwToolTipStyle |= TTS_BALLOON;
}

void CYJButton::InitToolTip()
{
	if (m_ctrlToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ctrlToolTip.Create(this, m_dwToolTipStyle|TTS_ALWAYSTIP);
		// Create inactive
		m_ctrlToolTip.Activate(FALSE);
		// Enable multiline
		m_ctrlToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
		//m_ToolTip.SendMessage(TTM_SETTITLE, TTI_INFO, (LPARAM)_T("Title"));
	} // if
}

void CYJButton::SetTooltipText(int nText, BOOL bActivate/* = TRUE*/)
{
	CString sText;

	// Load string resource
	sText.LoadString(nText);
	// If string resource is not empty
	if (sText.IsEmpty() == FALSE) SetTooltipText((LPCTSTR)sText, bActivate);
}

void CYJButton::SetTooltipText(LPCTSTR lpszText, BOOL bActivate/* = TRUE*/)
{
	if (lpszText == NULL) return;

	m_strTipText = lpszText;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ctrlToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ctrlToolTip.AddTool(this, m_strTipText, rectBtn, 1);
	} // if
	else
		// Set text for tooltip
		m_ctrlToolTip.UpdateTipText(m_strTipText, this, 1);

	m_bActiveTips = bActivate;
	m_ctrlToolTip.Activate(m_bActiveTips);
}

void CYJButton::ActivateTooltip(BOOL bActivate /* = TRUE */)
{
	if (m_ctrlToolTip.GetToolCount() == 0) return;

	m_bActiveTips = bActivate;
	m_ctrlToolTip.Activate(m_bActiveTips);
}

BOOL CYJButton::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ctrlToolTip.RelayEvent(pMsg);

	return CButton::PreTranslateMessage(pMsg);
}

void CYJButton::SetBackgroundColor(COLORREF clrBackground, BOOL bRepaint /* = TRUE */)
{
	m_clrBackground = clrBackground;
	if (bRepaint)
		Invalidate();
}

void CYJButton::SetFat(BOOL bIsFat, BOOL bRepaint /* = TRUE */)
{
	m_bIsFat = bIsFat;
	if (bRepaint)
		Invalidate();
}

void CYJButton::SetBorderColor(COLORREF clrBorderNormal, COLORREF clrBorderHover, COLORREF clrBorderPressed, BOOL bRepaint /* = TRUE */)
{
	m_clrBorderNormal	= clrBorderNormal;
	m_clrBorderHover	= clrBorderHover;
	m_clrBorderPressed	= clrBorderPressed;

	if (bRepaint)
		Invalidate();
}
void CYJButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	const MSG* pstMSG = GetCurrentMessage();
	DefWindowProc( WM_LBUTTONDOWN, pstMSG->wParam, pstMSG->lParam );
}

void CYJButton::PreSubclassWindow()
{
	UINT nBS = GetButtonStyle();
	if (nBS & BS_CHECKBOX) m_bIsCheckBox = TRUE;

	// Switch to owner-draw
	if ((nBS & BS_TYPEMASK) != BS_OWNERDRAW)
		ModifyStyle(BS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

LRESULT CYJButton::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(BM_SETSTYLE,
		(wParam & ~BS_TYPEMASK) | BS_OWNERDRAW, lParam);
}

void CYJButton::SetCheck(BOOL bIsChecked, BOOL bRepaint /* = TRUE */)
{
	if (m_bIsCheckBox)
	{
		m_bIsChecked = bIsChecked;

		if (bRepaint) Invalidate();
	}
}

LRESULT CYJButton::OnSetCheck(WPARAM wParam, LPARAM lParam)
{
	if (!m_bIsCheckBox) return 0;

	switch (wParam)
	{
	case BST_CHECKED:
	case BST_INDETERMINATE:	// Indeterminate state is handled like checked state
		SetCheck(TRUE);
		break;
	default:
		SetCheck(FALSE);
		break;
	} // switch

	return 0;
}

void CYJButton::EnablePress(BOOL bEnable /* = TRUE */, BOOL bRepaint /* = FALSE */)
{
	m_bEnablePress = bEnable;
	if (bRepaint) Invalidate();
}