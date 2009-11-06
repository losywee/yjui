/********************************************************************
Created:	2009/02/27
Filename: 	YJSplitter.cpp
Author:		YJ.Cai
E-mail:		mailto:caiyj.84@gmail.com

Purpose:	Easy to setup Splitter.
Version History:
			1.0 Initial Release				2009-03-5
*********************************************************************/

#include "stdafx.h"
#include "YJSplitter.h"


// CYJSplitter
#define AUTO_HIDDEN_PX 10

IMPLEMENT_DYNAMIC(CYJSplitter, CWnd)
CYJSplitter::CYJSplitter()
{
	m_pFirstWndBox	= m_pSecondWndBox = NULL;
	m_nFMinSize	= m_nSMinSize = 0;
	m_nSplitterWidth		= 3;
//	m_rcPanelMargin			= CRect(3,0,3,3);
	m_fSplitterPos	= 50;
	m_bIsHorizSplitter		= TRUE;
	m_bMovingSplitter		= FALSE;
	m_bShowPanelBorder		= TRUE;
	m_nPanelBorder			= 1;
	m_clrBackground			= GetSysColor(COLOR_3DFACE);
	m_clrBorder				= GetSysColor(COLOR_3DDKSHADOW);

	m_rcFirstWnd.SetRect(0,0,0,0);
	m_rcSplitter.SetRect(0,0,0,0);
	m_rcSecondWnd.SetRect(0,0,0,0);
}

CYJSplitter::~CYJSplitter()
{
}


BEGIN_MESSAGE_MAP(CYJSplitter, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CYJSplitter 消息处理程序

void CYJSplitter::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CBrush brush(m_clrBackground);
	CRect rcClient;
	GetClientRect(rcClient);
	dc.FillRect(rcClient,&brush);

	if (m_bShowPanelBorder)
	{
		CPen pen(PS_SOLID,1,m_clrBorder);
		CPen* pOldPen = dc.SelectObject(&pen);
		dc.Rectangle(m_rcFirstWnd);
		dc.Rectangle(m_rcSecondWnd);
		dc.SelectObject(pOldPen);
	}
}

BOOL CYJSplitter::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CWnd::OnEraseBkgnd(pDC);
}

void CYJSplitter::CalculatePanelRect()
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (m_bIsHorizSplitter)
	{
		m_rcFirstWnd.SetRect(0,0,(rcClient.Width() - m_nSplitterWidth) * m_fSplitterPos / 100,rcClient.bottom);
		m_rcSplitter.SetRect(m_rcFirstWnd.right,0,m_rcFirstWnd.right + m_nSplitterWidth,rcClient.bottom);
		m_rcSecondWnd.SetRect(m_rcSplitter.right,0,rcClient.right,rcClient.bottom);

		CheckPanelSize();
	}
	else
	{
		m_rcFirstWnd.SetRect(0,0,rcClient.right,(rcClient.Height() - m_nSplitterWidth/* - m_nWndMarginSide*/) * m_fSplitterPos / 100);
		m_rcSplitter.SetRect(0,m_rcFirstWnd.bottom,rcClient.right,m_rcFirstWnd.bottom + m_nSplitterWidth);
		m_rcSecondWnd.SetRect(0,m_rcSplitter.bottom,rcClient.right,rcClient.bottom);

		CheckPanelSize();
	}
}

void CYJSplitter::CheckPanelSize()
{
	if (m_bIsHorizSplitter)
	{
		if (m_rcFirstWnd.Width() < m_nFMinSize)
		{
			if (m_rcFirstWnd.Width() < AUTO_HIDDEN_PX)
			{
				m_rcFirstWnd.right = m_rcFirstWnd.left;
			}
			else
			{
				m_rcFirstWnd.right = m_rcFirstWnd.left + m_nFMinSize;
			}
			m_rcSecondWnd.left = m_rcFirstWnd.right + m_nSplitterWidth;
			m_rcSplitter.left = m_rcFirstWnd.right;
			m_rcSplitter.right = m_rcSecondWnd.left;
		}
		else if (m_rcSecondWnd.Width() < m_nSMinSize )
		{
			if (m_rcSecondWnd.Width() < AUTO_HIDDEN_PX)
			{
				m_rcSecondWnd.left = m_rcSecondWnd.right;
			} 
			else
			{
				m_rcSecondWnd.left = m_rcSecondWnd.right - m_nSMinSize;
			}
			m_rcFirstWnd.right = m_rcSecondWnd.left - m_nSplitterWidth;
			m_rcSplitter.left = m_rcFirstWnd.right;
			m_rcSplitter.right = m_rcSecondWnd.left;
		}

	} 
	else
	{
		if (m_rcFirstWnd.Height() < m_nFMinSize)
		{
			if (m_rcFirstWnd.Height() < AUTO_HIDDEN_PX)
			{
				m_rcFirstWnd.bottom = m_rcFirstWnd.top;
			} 
			else
			{
				m_rcFirstWnd.bottom = m_rcFirstWnd.top + m_nFMinSize;
			}
			m_rcSecondWnd.top = m_rcFirstWnd.bottom + m_nSplitterWidth;
			m_rcSplitter.top = m_rcFirstWnd.bottom;
			m_rcSplitter.bottom = m_rcSecondWnd.top;
		}
		else if (m_rcSecondWnd.Height() < m_nSMinSize )
		{
			if (m_rcSecondWnd.Height() < AUTO_HIDDEN_PX)
			{
				m_rcSecondWnd.top = m_rcSecondWnd.bottom;
			} 
			else
			{
				m_rcSecondWnd.top = m_rcSecondWnd.bottom - m_nSMinSize;
			}
			m_rcFirstWnd.bottom = m_rcSecondWnd.top - m_nSplitterWidth;
			m_rcSplitter.top = m_rcFirstWnd.bottom;
			m_rcSplitter.bottom = m_rcSecondWnd.top;
		}

	}
}

void CYJSplitter::ResizePanel()
{
	if (m_pFirstWndBox)
	{
		if (m_rcFirstWnd.Width() == 0 || m_rcFirstWnd.Height() == 0)
		{
			HideBox(m_pFirstWndBox);
		}
		else
		{
			ShowBox(m_pFirstWndBox);
			CRect rcWnd(m_rcFirstWnd);
			if (m_bShowPanelBorder)
			{
				rcWnd.DeflateRect(1,1,1,1);
			}
			m_pFirstWndBox->OnSize(rcWnd);
		}
	}

	if (m_pSecondWndBox)
	{
		if (m_rcSecondWnd.Width() == 0 || m_rcSecondWnd.Height() == 0)
		{
			HideBox(m_pSecondWndBox);
		} 
		else
		{
			ShowBox(m_pSecondWndBox);
			CRect rcWnd(m_rcSecondWnd);
			if (m_bShowPanelBorder)
			{
				rcWnd.DeflateRect(1,1,1,1);
			}
			m_pSecondWndBox->OnSize(rcWnd);
		}
	}
}

void CYJSplitter::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CalculatePanelRect();
	ResizePanel();
}

void CYJSplitter::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bMovingSplitter)
	{
		DrawSplitterMoveBar();

		CRect rcClient;
		GetClientRect(rcClient);
		if (m_bIsHorizSplitter)
		{
			m_rcFirstWnd.right = point.x - (m_nSplitterWidth >> 1);
			m_rcSecondWnd.left = m_rcFirstWnd.right + m_nSplitterWidth;

			m_rcSplitter.left = m_rcFirstWnd.right;
			m_rcSplitter.right = m_rcSecondWnd.left;

			CheckPanelSize();
			
		} 
		else
		{
			m_rcFirstWnd.bottom = point.y - (m_nSplitterWidth >> 1);
			m_rcSecondWnd.top = m_rcFirstWnd.bottom + m_nSplitterWidth;

			m_rcSplitter.top = m_rcFirstWnd.bottom;
			m_rcSplitter.bottom = m_rcSecondWnd.top;

			CheckPanelSize();

		}

		DrawSplitterMoveBar();
	}
	else if (m_rcSplitter.PtInRect(point))
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor (m_bIsHorizSplitter?(LPCTSTR)IDC_SIZEWE : (LPCTSTR)IDC_SIZENS));
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CYJSplitter::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_rcSplitter.PtInRect(point))
	{
		SetCapture();
		::SetCursor( AfxGetApp()->LoadStandardCursor (m_bIsHorizSplitter?(LPCTSTR)IDC_SIZEWE : (LPCTSTR)IDC_SIZENS));
		m_bMovingSplitter = TRUE;

		DrawSplitterMoveBar();
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CYJSplitter::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bMovingSplitter)
	{
		ReleaseCapture();
		m_bMovingSplitter = FALSE;

		DrawSplitterMoveBar();
		CRect rcClient;
		GetClientRect(rcClient);
		if (m_bIsHorizSplitter)
		{
//			m_rcFirstWnd.right = m_rcSplitter.left;
//			m_rcSecondWnd.left = m_rcSplitter.right;
			m_fSplitterPos = m_rcFirstWnd.right * 100.0 / (rcClient.Width() - m_nSplitterWidth);
		}
		else
		{
//			m_rcFirstWnd.bottom = m_rcSplitter.top;
//			m_rcSecondWnd.top = m_rcSplitter.bottom;
			m_fSplitterPos = m_rcFirstWnd.bottom * 100.0 / (rcClient.Height() - m_nSplitterWidth);
		}
		Invalidate();
		ResizePanel();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CYJSplitter::DrawSplitterMoveBar()
{
	if (m_bIsHorizSplitter)
	{
		if (m_rcSplitter.left < m_rcFirstWnd.left || m_rcSplitter.right > m_rcSecondWnd.right)
			return;
	} 
	else if (m_rcSplitter.top < m_rcFirstWnd.top || m_rcSplitter.bottom > m_rcSecondWnd.bottom)
	{
		return;
	}
	CWnd* pParentWnd = GetParent();
	CDC* pDC = GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
	CBrush* pBrush = CDC::GetHalftoneBrush();
	CBrush* pOldBrush = pDC->SelectObject(pBrush);

	//CRect rc = m_rcSplitter;
	//rc -= rcWin.TopLeft();

	pDC->PatBlt(m_rcSplitter.left,m_rcSplitter.top,m_rcSplitter.Width(),m_rcSplitter.Height(),PATINVERT);

	pDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
}

BOOL CYJSplitter::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CYJWndBox* pFirstWndBox/* = NULL*/, CYJWndBox* pSecondWndBox/* = NULL*/, BOOL bIsHorizSplitter/* = TRUE*/)
{
	m_pFirstWndBox = pFirstWndBox;
	m_pSecondWndBox = pSecondWndBox;
	m_bIsHorizSplitter = bIsHorizSplitter;

	if (CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, NULL, NULL))
	{
		if (m_pFirstWndBox)
		{
			m_pFirstWndBox->SetParent(this);
			m_pFirstWndBox->SetOwner(pParentWnd);
		}
		if (m_pSecondWndBox)
		{
			m_pSecondWndBox->SetParent(this);
			m_pSecondWndBox->SetOwner(pParentWnd);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

void CYJSplitter::SetMinSize(int nFirstPanel /* = 0 */, int nSecondPanel /* = 0 */)
{
	m_nFMinSize = nFirstPanel < AUTO_HIDDEN_PX ? AUTO_HIDDEN_PX : nFirstPanel;
	m_nSMinSize = nSecondPanel < AUTO_HIDDEN_PX ? AUTO_HIDDEN_PX : nSecondPanel;
}

inline void CYJSplitter::HideBox( CYJWndBox* pWndBox )
{
	if (pWndBox)
	{
		pWndBox->ShowWindow(SW_HIDE);
	}
}

inline void CYJSplitter::ShowBox( CYJWndBox* pWndBox )
{
	if (pWndBox)
	{
		pWndBox->ShowWindow(SW_SHOW);
	}
}

void CYJSplitter::ShowPanelBorder(BOOL bShow /* = TRUE */, BOOL bRepaint /* = FALSE */)
{
	m_bShowPanelBorder = bShow;
	if (bRepaint)
		Invalidate();
}

void CYJSplitter::SetPanelBorder(int nBorder /* = 1 */, BOOL bRepaint /* = FALSE */)
{
	m_nPanelBorder = nBorder;
	if (bRepaint)
		Invalidate();
}

void CYJSplitter::SetSplitterWidth(int nWidth, BOOL bRepaint /* = FALSE */)
{
	m_nSplitterWidth = nWidth;
	if (bRepaint)
		Invalidate();
}

void CYJSplitter::SetBackgroundColor(COLORREF clrBg, BOOL bRepaint /* = FALSE */)
{
	m_clrBackground = clrBg;
	if (bRepaint)
		Invalidate();
}

void CYJSplitter::SetBorderColor(COLORREF clrBorder, BOOL bRepaint /* = FALSE */)
{
	m_clrBorder = clrBorder;
	if (bRepaint)
		Invalidate();
}


void CYJSplitter::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_rcSplitter.PtInRect(point) && m_fSplitterPos != 50)
	{
		m_fSplitterPos = 50;
		CalculatePanelRect();
		Invalidate();
		ResizePanel();
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}
