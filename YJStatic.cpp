// YJStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "YJStatic.h"


// CYJStatic

IMPLEMENT_DYNAMIC(CYJStatic, CStatic)
CYJStatic::CYJStatic()
{
	m_crTextColor		= RGB(0,0,0);
	m_crBkColor			= GetSysColor(COLOR_BTNFACE);
}

CYJStatic::~CYJStatic()
{
}


BEGIN_MESSAGE_MAP(CYJStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


void CYJStatic::SetTextColor(COLORREF crTextColor, BOOL bRepaint/* = FALSE*/)
{
	m_crTextColor = crTextColor;
	if (bRepaint)
		Invalidate();
}

HBRUSH CYJStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_crTextColor);

	return m_brush;
}

void CYJStatic::PreSubclassWindow()
{
	m_brush.CreateSolidBrush(m_crBkColor);

	CStatic::PreSubclassWindow();
}

void CYJStatic::SetBkColor(COLORREF crText, BOOL bRepaint/* = FALSE*/)
{
	m_crBkColor = crText;

	if (m_crBkColor)
	{
		m_brush.DeleteObject();
		m_brush.CreateSolidBrush(m_crBkColor);
	}

	if (bRepaint)
		Invalidate();
}
