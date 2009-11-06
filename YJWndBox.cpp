#include "StdAfx.h"
#include "YJWndBox.h"

CYJWndBox::CYJWndBox(CRect rcBox/* = CRect(0,0,0,0)*/)
{
	m_rcBox		= rcBox;
}

CYJWndBox::~CYJWndBox(void)
{
}

BOOL CYJWndBox::AddPanel(HWND hWnd, int nWidth, int nHeight, CRect rcMargin/* = CRect(0,0,0,0)*/, UINT nStyle/* = PANEL_FIX|PANEL_LEFT_TOP*/)
{
	if (m_rcBox.IsRectEmpty())
		return FALSE;

	PANEL_INFO panelInfo(hWnd, nWidth, nHeight, rcMargin, nStyle);
	CalculatePanelPosition(panelInfo);
	m_arrPanels.Add(panelInfo);

	return TRUE;
}

void CYJWndBox::CalculatePanelPosition(PANEL_INFO& panelInfo)
{
	// Left & Right
	if (panelInfo.m_nStyle&PANEL_ALIGN_LEFT)
	{
		panelInfo.m_rcPosition.left = m_rcBox.left + panelInfo.m_rcMargin.left;
		if (panelInfo.m_nStyle&PANEL_WIDTH_FIX)
		{
			panelInfo.m_rcPosition.right = panelInfo.m_rcPosition.left + panelInfo.m_nWidth;
		} 
		else
		{
			panelInfo.m_rcPosition.right = panelInfo.m_rcPosition.left + m_rcBox.Width() * panelInfo.m_nWidth / 100;
		}
	}
	else if (panelInfo.m_nStyle&PANEL_ALIGN_CENTER)
	{
		int nMid = (m_rcBox.left + m_rcBox.right) / 2;
		if (panelInfo.m_nStyle&PANEL_WIDTH_FIX)
		{
			panelInfo.m_rcPosition.right = nMid + panelInfo.m_nWidth / 2;
		} 
		else
		{
			panelInfo.m_rcPosition.right = nMid + m_rcBox.Width() * panelInfo.m_nWidth / 200;
		}
		panelInfo.m_rcPosition.left = panelInfo.m_rcPosition.right - panelInfo.m_nWidth;
	}
	else
	{
		panelInfo.m_rcPosition.right = m_rcBox.right - panelInfo.m_rcMargin.right;
		if (panelInfo.m_nStyle&PANEL_WIDTH_FIX)
		{
			panelInfo.m_rcPosition.left = panelInfo.m_rcPosition.right - panelInfo.m_nWidth;
		} 
		else
		{
			panelInfo.m_rcPosition.left = panelInfo.m_rcPosition.right - m_rcBox.Width() * panelInfo.m_nWidth / 100;
		}
	}

	// Top & Bottom
	if (panelInfo.m_nStyle&PANEL_VALIGN_TOP)
	{
		panelInfo.m_rcPosition.top = m_rcBox.top + panelInfo.m_rcMargin.top;
		if (panelInfo.m_nStyle&PANEL_HEIGHT_FIX)
		{
			panelInfo.m_rcPosition.bottom = panelInfo.m_rcPosition.top + panelInfo.m_nHeight;
		} 
		else
		{
			panelInfo.m_rcPosition.bottom = panelInfo.m_rcPosition.top + m_rcBox.Height() * panelInfo.m_nHeight / 100;
		}
	}
	else if (panelInfo.m_nStyle&PANEL_VALIGN_CENTER)
	{
		int nMid = (m_rcBox.top + m_rcBox.bottom) / 2;
		if (panelInfo.m_nStyle&PANEL_HEIGHT_FIX)
		{
			panelInfo.m_rcPosition.bottom = nMid + panelInfo.m_nHeight / 2;
		} 
		else
		{
			panelInfo.m_rcPosition.bottom = nMid + m_rcBox.Height() * panelInfo.m_nHeight / 200;
		}
		panelInfo.m_rcPosition.top = panelInfo.m_rcPosition.bottom - panelInfo.m_nHeight;
	}
	else
	{
		panelInfo.m_rcPosition.bottom = m_rcBox.bottom - panelInfo.m_rcMargin.bottom;
		if (panelInfo.m_nStyle&PANEL_HEIGHT_FIX)
		{
			panelInfo.m_rcPosition.top = panelInfo.m_rcPosition.bottom - panelInfo.m_nHeight;
		} 
		else
		{
			panelInfo.m_rcPosition.top = panelInfo.m_rcPosition.bottom - m_rcBox.Height() * panelInfo.m_nHeight / 100;
		}
	}

	CRect rcBox = m_rcBox;
	rcBox.DeflateRect(panelInfo.m_rcMargin);

	panelInfo.m_rcPosition &= rcBox;

	if (panelInfo.m_hWnd)
	{
		::MoveWindow(panelInfo.m_hWnd, 
			panelInfo.m_rcPosition.left, 
			panelInfo.m_rcPosition.top, 
			panelInfo.m_rcPosition.Width(), 
			panelInfo.m_rcPosition.Height(), 
			TRUE);
	}
}

void CYJWndBox::OnSize(CRect rcNewSize)
{
	m_rcBox = rcNewSize;
	for (int i = 0;i<m_arrPanels.GetCount();i++)
	{
		CalculatePanelPosition(m_arrPanels[i]);
	}
}

void CYJWndBox::ShowWindow(int nCmdShow/* = SW_SHOW*/)
{
	for (int i = 0;i<m_arrPanels.GetCount();i++)
	{
		PANEL_INFO panelInfo = m_arrPanels[i];
		if (panelInfo.m_hWnd)
		{
			::ShowWindow(panelInfo.m_hWnd, nCmdShow);
		}
	}
}

void CYJWndBox::SetParent(CWnd* pParentWnd)
{
	ASSERT(pParentWnd && pParentWnd->GetSafeHwnd());
	for (int i = 0;i<m_arrPanels.GetCount();i++)
	{
		PANEL_INFO panelInfo = m_arrPanels[i];
		if (panelInfo.m_hWnd)
		{
			::SetParent(panelInfo.m_hWnd, pParentWnd->GetSafeHwnd());
		}
	}
}

void CYJWndBox::SetOwner(CWnd* pOwnerWnd)
{
	ASSERT(pOwnerWnd && pOwnerWnd->GetSafeHwnd());
	for (int i = 0;i<m_arrPanels.GetCount();i++)
	{
		PANEL_INFO panelInfo = m_arrPanels[i];
		if (panelInfo.m_hWnd)
		{
			CWnd::FromHandle(panelInfo.m_hWnd)->SetOwner(pOwnerWnd);
		}
	}
}