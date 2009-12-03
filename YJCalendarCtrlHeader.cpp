#include "stdafx.h"
#include "YJCalendarCtrlHeader.h"

#define ID_BTN_NEXT_MONTH			(WM_USER+1)
#define ID_BTN_PREV_MONTH			(WM_USER+2)
#define ID_BTN_NEXT_YEAR			(WM_USER+3)
#define ID_BTN_PREV_YEAR			(WM_USER+4)


// CYJCalendarCtrlHeader

IMPLEMENT_DYNAMIC(CYJCalendarCtrlHeader, CWnd)
CYJCalendarCtrlHeader::CYJCalendarCtrlHeader()
{
	m_clrBackground			= RGB(253,249,239);//GetSysColor(COLOR_3DFACE);
	m_clrDate				= RGB(128, 128, 128);
	m_hFontDate				= NULL;
}

CYJCalendarCtrlHeader::~CYJCalendarCtrlHeader()
{
}


BEGIN_MESSAGE_MAP(CYJCalendarCtrlHeader, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_YJCALENDAR_MONTH_CHANGED,OnMonthChanged)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_BTN_NEXT_MONTH, OnNextMonth)
	ON_BN_CLICKED(ID_BTN_PREV_MONTH, OnPrevMonth)
	ON_BN_CLICKED(ID_BTN_NEXT_YEAR, OnNextYear)
	ON_BN_CLICKED(ID_BTN_PREV_YEAR, OnPrevYear)
END_MESSAGE_MAP()


BOOL CYJCalendarCtrlHeader::SetCalendarCtrl(CYJCalendarCtrl* pWnd)
{
	ASSERT(pWnd);

	if (pWnd->GetSafeHwnd())
	{
		m_pCalendar = pWnd;
		m_pCalendar->AddNotify(m_hWnd);
		return TRUE;
	}
	return FALSE;
}
BOOL CYJCalendarCtrlHeader::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}

void CYJCalendarCtrlHeader::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcClient;
	GetClientRect(&rcClient);

	if (m_clrBackground != -1)
		dc.FillSolidRect(&rcClient, m_clrBackground);

	dc.SetTextColor(m_clrDate);
	dc.SetBkMode(TRANSPARENT);

	CFont* pFont;
	if (m_hFontDate)
		pFont = CFont::FromHandle(m_hFontDate);
	else
		pFont = GetParent()->GetFont();

	CFont* pOldFont = dc.SelectObject(pFont);

	CString strDate;
	strDate.Format(_T("%dÄê%dÔÂ"), m_pCalendar->GetSelectedYear(), m_pCalendar->GetSelectedMonth());
	dc.DrawText(strDate, &rcClient, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	dc.SelectObject(pOldFont);
}

LRESULT CYJCalendarCtrlHeader::OnMonthChanged(WPARAM wParam,LPARAM lParam)
{
	Invalidate();
	return 1;
}

int CYJCalendarCtrlHeader::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect(&rcClient);

	if (!m_btnNextMonth.Create(_T(">"), BS_BOTTOM|BS_OWNERDRAW|WS_VISIBLE, CRect(rcClient.right - 40, 0, rcClient.right - 20, rcClient.bottom), this, ID_BTN_NEXT_MONTH))
		return -1;

	if (!m_btnNextYear.Create(_T(">>"), BS_BOTTOM|BS_OWNERDRAW|WS_VISIBLE, CRect(rcClient.right - 20, 0, rcClient.right, rcClient.bottom), this, ID_BTN_NEXT_YEAR))
		return -1;

	if (!m_btnPrevMonth.Create(_T("<"), BS_BOTTOM|BS_OWNERDRAW|WS_VISIBLE, CRect(rcClient.left + 20, 0, rcClient.left + 40, rcClient.bottom), this, ID_BTN_PREV_MONTH))
		return -1;

	if (!m_btnPrevYear.Create(_T("<<"), BS_BOTTOM|BS_OWNERDRAW|WS_VISIBLE, CRect(rcClient.left, 0, rcClient.left + 20, rcClient.bottom), this, ID_BTN_PREV_YEAR))
		return -1;

	m_btnNextMonth.SetFat(TRUE, FALSE);
	m_btnPrevMonth.SetFat(TRUE, FALSE);
	m_btnNextYear.SetFat(TRUE, FALSE);
	m_btnPrevYear.SetFat(TRUE, FALSE);

	m_btnNextMonth.ShowBorder(FALSE, FALSE);
	m_btnPrevMonth.ShowBorder(FALSE, FALSE);
	m_btnNextYear.ShowBorder(FALSE, FALSE);
	m_btnPrevYear.ShowBorder(FALSE, FALSE);

	m_btnNextMonth.ShowFocus(FALSE, FALSE);
	m_btnPrevMonth.ShowFocus(FALSE, FALSE);
	m_btnNextYear.ShowFocus(FALSE, FALSE);
	m_btnPrevYear.ShowFocus(FALSE, FALSE);

	m_btnNextMonth.SetTextColor(m_clrDate, FALSE);
	m_btnPrevMonth.SetTextColor(m_clrDate, FALSE);
	m_btnNextYear.SetTextColor(m_clrDate, FALSE);
	m_btnPrevYear.SetTextColor(m_clrDate, FALSE);

	m_btnNextMonth.SetBackgroundColor(RGB(253,249,239), FALSE);
	m_btnPrevMonth.SetBackgroundColor(RGB(253,249,239), FALSE);
	m_btnNextYear.SetBackgroundColor(RGB(253,249,239), FALSE);
	m_btnPrevYear.SetBackgroundColor(RGB(253,249,239), FALSE);


	return 0;
}

void CYJCalendarCtrlHeader::OnNextMonth()
{
	ASSERT(m_pCalendar);

	m_pCalendar->NextMonth();
}

void CYJCalendarCtrlHeader::OnPrevMonth()
{
	ASSERT(m_pCalendar);

	m_pCalendar->PrevMonth();
}

void CYJCalendarCtrlHeader::OnNextYear()
{
	ASSERT(m_pCalendar);

	m_pCalendar->NextYear();
}

void CYJCalendarCtrlHeader::OnPrevYear()
{
	ASSERT(m_pCalendar);

	m_pCalendar->PrevYear();
}