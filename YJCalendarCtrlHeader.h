#pragma once

#include "YJCalendarCtrl.h"
#include "YJButton.h"

class AFX_EXT_CLASS CYJCalendarCtrlHeader : public CWnd
{
	DECLARE_DYNAMIC(CYJCalendarCtrlHeader)

private:
	CYJCalendarCtrl*			m_pCalendar;
	COLORREF					m_clrDate;
	COLORREF					m_clrBackground;
	HFONT						m_hFontDate;

	CYJButton					m_btnNextMonth;
	CYJButton					m_btnNextYear;
	CYJButton					m_btnPrevMonth;
	CYJButton					m_btnPrevYear;

protected:
	DECLARE_MESSAGE_MAP()

public:
	CYJCalendarCtrlHeader();
	virtual ~CYJCalendarCtrlHeader();

	BOOL SetCalendarCtrl(CYJCalendarCtrl* pWnd);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnMonthChanged(WPARAM wParam,LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNextMonth();
	afx_msg void OnPrevMonth();
	afx_msg void OnNextYear();
	afx_msg void OnPrevYear();
};