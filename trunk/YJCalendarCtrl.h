#pragma once

#include <afxtempl.h>

#define WM_YJCALENDAR_DATE_CHANGED			(WM_USER+1)
#define WM_YJCALENDAR_MONTH_CHANGED			(WM_USER+2)
#define WM_YJCALENDAR_DBLCLICK				(WM_USER+3)

typedef struct  _STRUCT_CALENDER
{
	int m_nYear;
	int m_nMonth;
	int m_nDay;
	bool m_bIsToday;
	bool m_bIsSelectedMonth;
	bool m_bHasInfo;
}STRUCT_CALENDER;

typedef struct tagCalendarInfo
{
	COLORREF m_clrGrid;
	COLORREF m_clrTextLightDay;
	COLORREF m_clrTextDarkDay;
	COLORREF m_clrTextToday;
	COLORREF m_clrTextWeek;
	COLORREF m_clrBg;
	COLORREF m_clrBgWeek;
	COLORREF m_clrBgToday;
	COLORREF m_clrBgSelected;
	COLORREF m_clrMouseOverDay;

	int m_nBorderWidth;
	int m_nGridWidth;
	int m_nGridHeight;
	int m_nWeekHeight;
//	BOOL m_bShowGrid;

	HFONT m_hFontDate;
	HFONT m_hFontWeek;

	tagCalendarInfo()
	{
		m_clrGrid			= RGB(200,	200,	200);
		m_clrTextLightDay	= RGB(255,	0,		0);
		m_clrTextDarkDay	= RGB(128,	128,	128);
		m_clrTextToday		= RGB(255,	255,	255);
		m_clrTextWeek		= RGB(255,	255,	255);

		m_clrBg				= RGB(253,	249,	239);
		m_clrBgWeek			= RGB(0,	0,		255);
		m_clrBgToday		= RGB(0,	0,		255);
		m_clrBgSelected		= RGB(0,	0,		128);
		m_clrMouseOverDay	= RGB(0,	0,		0);

		m_nBorderWidth			= 1;
		m_nGridWidth			= 0;
		m_nGridHeight			= 0;
		m_nWeekHeight			= 20;

		m_hFontDate				= NULL;
		m_hFontWeek				= NULL;
	}

	~tagCalendarInfo()
	{
		if (m_hFontDate)
			DeleteObject(m_hFontDate);

		if (m_hFontWeek)
			DeleteObject(m_hFontWeek);
	}
}CALENDAR_INFO;


class AFX_EXT_CLASS CYJCalendarCtrl : public CWnd
{
	DECLARE_DYNAMIC(CYJCalendarCtrl)

private:
	byte m_arrDaysOfMonth[13];//days of every month, starts from 1

	STRUCT_CALENDER m_CalendarArray[6][7];
	CALENDAR_INFO	m_CalendarInfo;
	int				m_nYear;
	int				m_nMonth;
	int				m_nDay;
	int				m_nSelectedYear;
	int				m_nSelectedMonth;
	int				m_nSelectedDay;
	//int m_nSelectedWeekOfMonth;
	
	//COLORREF m_clrSelectedBorderColor;
	
	CString			m_arrWeekNames[7];
	CPoint			m_ptSelected;
	CPoint			m_ptSelectedPrev;
	CPoint			m_ptToday;
	CRect			m_rcDate;
	CRect			m_rcWeek;
	BOOL			m_bIsCapture;

	CArray<HWND,HWND&> m_arrNotify;

	CDC				m_dcMem;
	CBitmap			m_bmpMem;


	BOOL RegisterWndClass();
	void GenerateCalendar();
	CPoint GetMousetPointInArray(CPoint point);
//	void DrawDayRegion(CRect* pRect,CString strText,COLORREF clrText,COLORREF clrBorder=NULL,COLORREF clrBg=NULL,BOOL bHasGrid=TRUE);
	void CallBackDateChange();
	void CallBackDBClick();
	inline BOOL HasGrid(){ return (m_CalendarInfo.m_nBorderWidth > 0); }
	inline void IncreaseDate(int& nYear, int& nMonth, int& nDay);
	void GetDayRect(CRect* pRect, int nRow, int nColumn);
	void GetDayRect(CRect* pRect, CPoint ptPoint);

protected:
	void DrawBackground(CRect* pRect, COLORREF color, BOOL bRepaint = FALSE);
	void DrawHead();
	void DrawWeek();
	void DrawGrid();
	void DrawDate();
	void DrawDay(CRect* pRect, int nRow, int nColumn);
	void DrawDay(CRect* pRect, CPoint ptPoint);
	void DrawFocus(int nRow, int nColumn, BOOL bRepaint = FALSE);
	void DrawFocus(CPoint ptPoint, BOOL bRepaint = FALSE);
	void DrawFocus(CRect* pRect, BOOL bRepaint = FALSE);
	void DrawCalendar();

	DECLARE_MESSAGE_MAP()

public:
	CYJCalendarCtrl();
	virtual ~CYJCalendarCtrl();
	void InitCalendar();
	void ChangeMonth(int nYear,int nMonth,int nDay);
// 	void SetCallBackDateChange(HWND hWnd, UINT nMessage,WPARAM wParam=0,LPARAM lParam= 0);
// 	void SetCallBackDBClick(HWND hWnd, UINT nMessage,WPARAM wParam=0,LPARAM lParam= 0);
// 	void SetCallBackHeader(HWND hWnd, UINT nMessage,WPARAM wParam=0,LPARAM lParam= 0);
	void AddNotify(HWND hWnd);
	int GetSelectedYear();
	int GetSelectedMonth();
	int GetSelectedDay();
	int GetSelectedWeekOfYear();
	void NextMonth();
	void PrevMonth();
	void NextYear();
	void PrevYear();
	void GoToday();
	void GetSelectedWeekRegion(int& nYearFrom,int& nMonthFrom,int& nDayFrom, int& nYearTo,int& nMonthTo,int& nDayTo);
	byte GetDaysOfSelectedMonth();


	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


