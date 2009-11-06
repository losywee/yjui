// YJListWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "YJListWnd.h"

// CYJDropScrollBar

IMPLEMENT_DYNAMIC(CYJDropScrollBar, CScrollBar)
CYJDropScrollBar::CYJDropScrollBar()
{
	m_bTracking		= FALSE;
	m_bIsCapture	= FALSE;
}

CYJDropScrollBar::~CYJDropScrollBar()
{
}


BEGIN_MESSAGE_MAP(CYJDropScrollBar, CScrollBar)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()




void CYJDropScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	CScrollBar::OnMouseMove(nFlags, point);
}


LRESULT CYJDropScrollBar::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	CRect rcParentClient;
	GetParent()->GetClientRect(&rcParentClient);
	GetParent()->ClientToScreen(&rcParentClient);
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	if (!rcParentClient.PtInRect(ptCursor))
		((CYJListWnd*)GetParent())->StartCapture();

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// CYJListWnd

IMPLEMENT_DYNAMIC(CYJListWnd, CWnd)
CYJListWnd::CYJListWnd()
{
	m_bIsInit		= FALSE;
	m_clrText		= RGB(0,0,0);
	m_nRowHeight	= 15;
	m_nCurSel		= -1;
	m_nVisibleItemCount = 0;
	m_nTopIndex		= 0;
	m_bIsCapture	= FALSE;
	m_bTracking		= FALSE;
	m_bSizing		= FALSE;
	m_nMouseDiffX	= 0;
	m_nMouseDiffY	= 0;
	m_pDesktopDC	= NULL;
	m_cxScreen		= GetSystemMetrics(SM_CXSCREEN);
	m_cyScreen		= GetSystemMetrics(SM_CYSCREEN);
	m_bIsFirstShow	= TRUE;
	m_nMinHeight	= 100;
}

CYJListWnd::~CYJListWnd()
{
	if (m_pDesktopDC)
		ReleaseDC(m_pDesktopDC);
}


BEGIN_MESSAGE_MAP(CYJListWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



void CYJListWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rcWnd,rcClient,rcSize;
	CDC dcMem;
	CBitmap  bitmap, *pOldBitmap;

	GetClientRect(&rcWnd);
	rcSize = rcClient = rcWnd;
	//rcClient.DeflateRect(1,1);
	dcMem.CreateCompatibleDC(&dc);

	if (m_ctrlVertBar.IsWindowVisible())
		rcClient.right -= GetSystemMetrics(SM_CYVSCROLL);
	rcSize.left = rcSize.right - GetSystemMetrics(SM_CXHSCROLL);
	rcSize.top = rcSize.bottom - GetSystemMetrics(SM_CYVSCROLL);
// 	rcSize.right -= 1;
// 	rcSize.bottom -= 1;

	bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	pOldBitmap = dcMem.SelectObject(&bitmap);

// 	CPen m_Pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
// 	CPen *pOldPen = (CPen*)dcMem.SelectObject(&m_Pen);

	dcMem.FillSolidRect(rcClient,::GetSysColor(COLOR_WINDOW));
	//dcMem.Rectangle(rcWnd);

	DrawItem(&dcMem,rcClient.Width());

	if (m_ctrlVertBar.IsWindowVisible())
	{
		dc.FillSolidRect(rcSize, ::GetSysColor(COLOR_BTNFACE) );
		DrawSize(&dc,rcSize);
	}
	else
		DrawSize(&dcMem,rcSize);



	dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
		&dcMem, rcClient.left, rcClient.top, SRCCOPY);

//	dcMem.SelectObject(pOldPen);
	dcMem.SelectObject(pOldBitmap);
}

void CYJListWnd::DrawSize(CDC* pDC, CRect& rect)
{
	CPen pen1(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOW));
	CPen pen2(PS_SOLID, 1, ::GetSysColor(COLOR_BTNFACE));
	CPen pen3(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
	CPen *pOldPen = pDC->GetCurrentPen();

	int i = 0;
	int y2 = rect.bottom -1;
	int x2 = rect.right;
	while (true)
	{
		int x = rect.left + i + 1;
		int y = rect.top + i;
		if(x >= x2 && y >= y2)
			break;

		switch (i % 3)
		{
		case 0:
			pDC->SelectObject(&pen1);
			break;
		case 1:
			pDC->SelectObject(&pen2);
			break;
		case 2:
			pDC->SelectObject(&pen3);
			break;
		}
		
		pDC->MoveTo(x, y2);
		pDC->LineTo(x2, y);

		i++;
	}
	if (i > 0)
		pDC->SelectObject(pOldPen);
}

void CYJListWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		m_nCurSel = -1;
		m_nTopIndex = 0;

		CalculateWindowRect();
		
		SetWindowPos(&wndTopMost,m_rcWnd.left,m_rcWnd.top,m_rcWnd.Width(),m_rcWnd.Height(),0);

		SetScroller();

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_bIsCapture = TRUE;
		SetCapture();
	}
	else if (m_bIsCapture)
	{
		m_bIsCapture = FALSE;
		ReleaseCapture();
	}

	CWnd::OnShowWindow(bShow, nStatus);
//	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}


void CYJListWnd::Init(CWnd* pParent)
{
	if (!m_bIsInit)
	{
		m_bIsInit = TRUE;
		m_pParent = pParent;

		VERIFY(m_ctrlVertBar.Create(WS_VISIBLE|SBS_VERT|SBS_LEFTALIGN,
			CRect(0,0,GetSystemMetrics(SM_CYVSCROLL),100),this,0));
		
		SetScroller();
	}
	
}

void CYJListWnd::SetScroller()
{
	if(m_ctrlVertBar.GetSafeHwnd())
	{
		CRect rcWnd,rcBar;
		GetClientRect(rcWnd);
		rcBar = rcWnd;

		m_nVisibleItemCount = rcWnd.Height()/m_nRowHeight;
		if (m_nVisibleItemCount < GetItemCount())
		{
			if (!m_ctrlVertBar.IsWindowVisible())
				m_ctrlVertBar.ShowWindow(SW_SHOW);

			rcBar.left = (rcBar.Width()-GetSystemMetrics(SM_CYVSCROLL));
			rcBar.bottom-= GetSystemMetrics(SM_CYHSCROLL);
			m_ctrlVertBar.MoveWindow(rcBar);

			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			if( m_ctrlVertBar.GetScrollInfo( &info, SIF_ALL|SIF_DISABLENOSCROLL ) )
			{
				info.nMin = 0;
				info.nMax = GetItemCount() - 1;
				info.nPage = m_nVisibleItemCount;
				m_ctrlVertBar.SetScrollInfo( &info );
			}
		}
		else
		{
			m_ctrlVertBar.ShowWindow(SW_HIDE);
		}

		if(m_nTopIndex + m_nVisibleItemCount > GetItemCount())
		{
			m_nTopIndex = GetItemCount() - m_nVisibleItemCount;
			if(m_nTopIndex < 0)
				m_nTopIndex = 0;
			m_ctrlVertBar.SetScrollPos(m_nTopIndex,true);
		}
	}
}

void CYJListWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	ShowWindow(SW_HIDE);
}

BOOL CYJListWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CRect rectClient;
	CPoint ptCursor;

	GetClientRect(rectClient);

	rectClient.left = rectClient.right - GetSystemMetrics(SM_CYVSCROLL);
	rectClient.top = rectClient.bottom - GetSystemMetrics(SM_CXVSCROLL);

	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	if(rectClient.PtInRect(ptCursor))
	{
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return TRUE;

/*	return CWnd::OnSetCursor(pWnd, nHitTest, message);*/
}

void CYJListWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CYJListWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN )
	{
		int nOldSel = m_nCurSel;

		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			ShowWindow(SW_HIDE);
			return TRUE;
			break;
		case VK_RETURN:
			ShowWindow(SW_HIDE);
			if (m_nCurSel >= 0)
				m_pParent->SendMessage(LIST_SELECTED,0,m_nCurSel);
			else
				m_pParent->PostMessage(WM_KEYDOWN,pMsg->wParam,pMsg->lParam);
			return TRUE;
			break;
		case VK_UP:
			if (m_nCurSel > 0)
				m_nCurSel--;
			if (m_nCurSel < 0)
				m_nCurSel = 0;

			if (m_nCurSel < m_nTopIndex)
				SetTopIndex(m_nCurSel, (nOldSel == m_nCurSel));

			if (nOldSel != m_nCurSel)
			{
				m_pParent->SendMessage(LIST_SELECT_CHANGED,0,m_nCurSel);
				Invalidate();
			}

			return TRUE;
			break;
		case VK_DOWN:
			if (m_nCurSel < GetItemCount() - 1)
				m_nCurSel++;

			if (m_nCurSel >= m_nTopIndex + m_nVisibleItemCount)
				SetTopIndex(m_nCurSel - m_nVisibleItemCount + 1, (nOldSel == m_nCurSel));

			if (nOldSel != m_nCurSel)
			{
				m_pParent->SendMessage(LIST_SELECT_CHANGED,0,m_nCurSel);
				Invalidate();
			}

			return TRUE;
			break;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CYJListWnd::AddString(LPCTSTR szText, LPCTSTR szNote/* = NULL*/)
{
	LIST_ITEM newItem(szText,szNote);
	m_arrItemList.Add(newItem);

	int count = (int)m_arrItemList.GetCount();
	if (count > 10)
		m_nMinHeight = 10 * m_nRowHeight + 2;
	else
		m_nMinHeight = count * m_nRowHeight + 2;

	m_bIsFirstShow = TRUE;

	SortString(m_arrItemList);
}

void CYJListWnd::AddStrings(CItemArray& arrItem)
{
	m_arrItemList.Append(arrItem);
	
	int count = (int)m_arrItemList.GetCount();
	if (count > 10)
		m_nMinHeight = 10 * m_nRowHeight + 2;
	else
		m_nMinHeight = count * m_nRowHeight + 2;

	m_bIsFirstShow = TRUE;

	SortString(m_arrItemList);
}

void CYJListWnd::SetTextColor(COLORREF clrText, BOOL bRepaint /* = FALSE */)
{
	m_clrText = clrText;
	if (bRepaint)
		Invalidate();
}

void CYJListWnd::SetRowHeight(int nHeight, BOOL bRepaint /* = FALSE */)
{
	if (nHeight > 0)
	{
		m_nRowHeight = nHeight;
		if (bRepaint)
			Invalidate();
	}
}

void CYJListWnd::DrawItem(CDC* pDC, int nWidth)
{
	pDC->SetTextColor(m_clrText);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT)); 

	CRect rcItem(0,0,nWidth,m_nRowHeight);
	CRect rcItemL(rcItem),rcItemR(rcItem);
	rcItemL.left = 2;
	rcItemL.right = 100;
	rcItemR.left = 100;

	int j = m_nTopIndex+m_nVisibleItemCount;
	j = j > GetItemCount() ? GetItemCount():j;
	for (int i = m_nTopIndex;i < j;i++)
	{
		if (i == m_nCurSel)
		{
			pDC->FillSolidRect(rcItem,::GetSysColor(COLOR_HIGHLIGHT));
			pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->DrawText(m_arrItemDisplayList[i].strItemText, rcItemL, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
			pDC->DrawText(m_arrItemDisplayList[i].strNote, rcItemR, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
		}
		else
		{
			pDC->SetTextColor(m_clrText);
			pDC->DrawText(m_arrItemDisplayList[i].strItemText, rcItemL, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
			pDC->SetTextColor(RGB(128,128,128));
			pDC->DrawText(m_arrItemDisplayList[i].strNote, rcItemR, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
		}
		rcItem.OffsetRect(0,m_nRowHeight);
		rcItemL.OffsetRect(0,m_nRowHeight);
		rcItemR.OffsetRect(0,m_nRowHeight);
	}
}
UINT CYJListWnd::OnNcHitTest(CPoint point)
{
// 	CRect rcClient;
// 	GetWindowRect(rcClient);
// 
// 	rcClient.left = rcClient.right - GetSystemMetrics(SM_CYVSCROLL);
// 	rcClient.top = rcClient.bottom - GetSystemMetrics(SM_CXVSCROLL);
// 
// 	if(rcClient.PtInRect(point))
// 		return HTBOTTOMRIGHT;
// 	else
		return HTCLIENT;
}

BOOL CYJListWnd::OnEraseBkgnd(CDC* pDC)
{
	return false;
//	return CWnd::OnEraseBkgnd(pDC);
}

void CYJListWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	SetScroller();
}

void CYJListWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CRect rcParent;
	m_pParent->GetWindowRect(rcParent);

	lpMMI->ptMinTrackSize = CPoint(rcParent.Width(), 100);

	CWnd::OnGetMinMaxInfo(lpMMI);
}

int CYJListWnd::HitTest(CPoint ptPoint)
{
	CRect rcClient,rcItem;
	GetClientRect(rcClient);
	rcClient.right -= GetScrollBarWidth();

	int j = m_nTopIndex+m_nVisibleItemCount;
	j = j > GetItemCount() ? GetItemCount():j;
	for (int i = m_nTopIndex; i < j; i++)
	{
		rcItem.SetRect(rcClient.left,rcClient.top + (i - m_nTopIndex) * m_nRowHeight,rcClient.right,rcClient.top + (i - m_nTopIndex + 1) * m_nRowHeight);
		if (rcItem.PtInRect(ptPoint))
		{
			m_nCurSel = i;
			return i;
		}
	}
	return -1;
}

long CYJListWnd::GetScrollBarWidth()
{
	if(m_ctrlVertBar.IsWindowVisible())
		return GetSystemMetrics(SM_CYVSCROLL);
	else
		return 0;
}

void CYJListWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bSizing)
	{
		CRect rcParent;
		m_pParent->GetWindowRect(rcParent);
		int x = point.x + m_nMouseDiffX;
		int y = point.y + m_nMouseDiffY;

		if (x < rcParent.Width())
			x = rcParent.Width();
		else if (m_rcOldFocusRect.left + x > m_cxScreen)
			x = m_cxScreen - m_rcOldFocusRect.left;
		if (y < m_nMinHeight)
			y = m_nMinHeight;
		else if (m_rcOldFocusRect.top + y > m_cyScreen)
			y = m_cyScreen - m_rcOldFocusRect.top;


		m_pDesktopDC->DrawFocusRect(&m_rcOldFocusRect);

		m_rcOldFocusRect.right = m_rcOldFocusRect.left + x;
		m_rcOldFocusRect.bottom = m_rcOldFocusRect.top + y;
		
		m_pDesktopDC->DrawFocusRect(&m_rcOldFocusRect);
		return;
	}

	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	CRect rcClient,rcSize;
	GetClientRect(&rcClient);
	rcSize = rcClient;
	rcSize.left = rcSize.right - GetSystemMetrics(SM_CYVSCROLL);;
	rcSize.top = rcSize.bottom - GetSystemMetrics(SM_CYHSCROLL);

	if (m_bIsCapture && rcClient.PtInRect(point))
	{
		m_bIsCapture = FALSE;
		ReleaseCapture();
	}

	if (rcSize.PtInRect(point))
	{
		//TRACE("Size\\");
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
	}
	else
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	int sel = HitTest(point);
	if(sel >= 0)
	{
		Invalidate();
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CYJListWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	long nOldIndex = m_nTopIndex;

	switch(nSBCode)
	{
	case SB_ENDSCROLL:
		break;

	case SB_PAGEUP:
		m_nTopIndex -= m_nVisibleItemCount;
		if(m_nTopIndex < 0)
			m_nTopIndex = 0;
		break;

	case SB_PAGEDOWN:
		m_nTopIndex += m_nVisibleItemCount;
		if(m_nTopIndex >= GetItemCount()-m_nVisibleItemCount)
			m_nTopIndex = GetItemCount()-m_nVisibleItemCount;
		break;

	case SB_LINEUP:
		m_nTopIndex--;
		if(m_nTopIndex < 0)
			m_nTopIndex = 0;
		break;

	case SB_LINEDOWN:
		m_nTopIndex++;
		if(m_nTopIndex >= GetItemCount()-m_nVisibleItemCount)
			m_nTopIndex = GetItemCount()-m_nVisibleItemCount;
		break;

	case SB_THUMBTRACK:
		m_nTopIndex = nPos;
		break;
	}

	m_ctrlVertBar.SetScrollPos(m_nTopIndex,true);

	if(nOldIndex != m_nTopIndex)
		Invalidate();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CYJListWnd::SetTopIndex(int nTopIndex, BOOL bRepaint/* = FALSE)*/)
{
	m_ctrlVertBar.SetScrollPos(nTopIndex,true);

	if (nTopIndex > GetItemCount() - m_nVisibleItemCount)
	{
		nTopIndex = GetItemCount() - m_nVisibleItemCount;
		if(nTopIndex < 0)
			nTopIndex = 0;
	}
	if(nTopIndex != m_nTopIndex)
	{
		m_nTopIndex = nTopIndex;

		if (bRepaint)
		{
			CPoint ptCursor;
			GetCursorPos(&ptCursor);
			ScreenToClient(&ptCursor);
			HitTest(ptCursor);
			Invalidate();
		}
	}
	
	
}

void CYJListWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rcWnd;
	GetWindowRect(rcWnd);
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	if (!rcWnd.PtInRect(ptCursor))
	{
		ShowWindow(SW_HIDE);
		return;
	}

	CRect rcClient,rcSize;
	GetClientRect(&rcClient);
	rcSize = rcClient;
	rcSize.left = rcSize.right - GetSystemMetrics(SM_CYVSCROLL);;
	rcSize.top = rcSize.bottom - GetSystemMetrics(SM_CYHSCROLL);

	if (rcSize.PtInRect(point))
	{
		m_bSizing = TRUE;
		m_nMouseDiffX = rcWnd.Width() - point.x;
		m_nMouseDiffY = rcWnd.Height() - point.y;

		CRect rcWnd;
		GetWindowRect( &rcWnd );
		rcWnd.right = rcWnd.left + point.x + m_nMouseDiffX;
		rcWnd.bottom = rcWnd.top + point.y + m_nMouseDiffY;
		m_rcOldFocusRect.CopyRect(rcWnd);

		m_pDesktopDC = CDC::FromHandle(::GetDC(NULL));
		m_pDesktopDC->DrawFocusRect(&m_rcOldFocusRect);

		SetCapture();
		return;
	}

	int nOldSel = m_nCurSel;
	int nSel = HitTest(point);
	if (nSel >= 0 && nOldSel != m_nCurSel)
		Invalidate();

	CWnd::OnLButtonDown(nFlags, point);
}

void CYJListWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bSizing)
	{
		m_bSizing = FALSE;

		m_pDesktopDC->DrawFocusRect(&m_rcOldFocusRect);
		MoveWindow(&m_rcOldFocusRect);
		m_rcWnd.CopyRect(&m_rcOldFocusRect);

		if (m_pDesktopDC)
			ReleaseDC(m_pDesktopDC);

		ReleaseCapture();
		return;
	}

	int nSel = HitTest(point);
	if (nSel >= 0)
	{
		m_pParent->SendMessage(LIST_SELECTED,0,nSel);
		ShowWindow(SW_HIDE);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CYJListWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_bSizing)
	{
		return;
	}

	CRect rcWnd;
	GetWindowRect(rcWnd);
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	if (!rcWnd.PtInRect(ptCursor))
	{
		ShowWindow(SW_HIDE);
		return;
	}

	CWnd::OnRButtonDown(nFlags, point);
}

LRESULT CYJListWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	if (!IsWindowVisible())
		return 0;

	CRect rcWnd;
	CPoint ptCursor;
	GetWindowRect(&rcWnd);
	GetCursorPos(&ptCursor);

	if (!rcWnd.PtInRect(ptCursor) && !m_bIsCapture)
	{
		m_bIsCapture = TRUE;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		SetCapture();
	}
	
	return 0;
}

LRESULT CYJListWnd::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	if (!IsWindowVisible())
		return 0;

	CRect rcWnd;
	CPoint ptCursor;
	GetWindowRect(&rcWnd);
	GetCursorPos(&ptCursor);

	if (rcWnd.PtInRect(ptCursor) && m_bIsCapture)
	{
		m_bIsCapture = FALSE;
		ReleaseCapture();
	}
	return 0;
}

void CYJListWnd::StartCapture()
{
	if (!m_bIsCapture)
	{
		m_bIsCapture = TRUE;
		SetCapture();
	}
}

void CYJListWnd::GetString(CString& strOut, int nItem)
{
	strOut = m_arrItemDisplayList[nItem].strItemText;
}

int CYJListWnd::CompareString(const void* p1, const void* p2)
{
	return _tcsicmp(((LIST_ITEM*)p1)->strItemText,((LIST_ITEM*)p2)->strItemText);
}

void CYJListWnd::SortString(CArray<LIST_ITEM,LIST_ITEM&>& arrList)
{
	if (arrList.GetCount() == 0)
		return;

	qsort(arrList.GetData(),arrList.GetSize(),sizeof(LIST_ITEM),CYJListWnd::CompareString);
}

int CYJListWnd::FindLikeString(const CString str)
{
	m_arrItemDisplayList.RemoveAll();
	for (int i = 0;i<m_arrItemList.GetCount();i++)
	{
		if (m_arrItemList[i].strItemText.Find(str) == 0)
			m_arrItemDisplayList.Add(m_arrItemList[i]);
	}
	if (GetItemCount() > 0)
	{
		SetScroller();
		if (!IsWindowVisible())
			ShowWindow(SW_SHOW);
		m_nCurSel = -1;
		Invalidate();
		return GetItemCount();
	}
	else if (IsWindowVisible())
		ShowWindow(SW_HIDE);
	return 0;
}

void CYJListWnd::FindAll()
{
	m_arrItemDisplayList.RemoveAll();
	m_arrItemDisplayList.Copy(m_arrItemList);

	if (GetItemCount() > 0)
	{
		SetScroller();
		if (!IsWindowVisible())
			ShowWindow(SW_SHOW);
		Invalidate();
	}
	else if (IsWindowVisible())
		ShowWindow(SW_HIDE);
}

void CYJListWnd::CalculateWindowRect()
{
	CRect rcWnd;
	m_pParent->GetWindowRect(&rcWnd);
	if (m_bIsFirstShow)
	{
		m_bIsFirstShow = FALSE;
		m_rcWnd.SetRect(rcWnd.left,rcWnd.bottom,rcWnd.right,rcWnd.bottom + m_nMinHeight);
	}
	else
	{
		m_rcWnd.SetRect(rcWnd.left,rcWnd.bottom,rcWnd.left + m_rcWnd.Width(),rcWnd.bottom + m_rcWnd.Height());
	}

	if (m_rcWnd.bottom > m_cyScreen)
	{
		m_rcWnd.SetRect(m_rcWnd.left,rcWnd.top-m_rcWnd.Height(),m_rcWnd.right,rcWnd.top);
	}
}