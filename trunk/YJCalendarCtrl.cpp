// CalendarCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "YJCalendarCtrl.h"

#define MYWNDCLASS _T("YJCalendarCtrl")
#define MARGIN_TOP 5
#define MARGIN_BOTTOM 1
#define MARGIN_LEFT 1
#define MARGIN_RIGHT 1
#define TITLE_HEIGHT 15
// CYJCalendarCtrl

IMPLEMENT_DYNAMIC(CYJCalendarCtrl, CWnd)
CYJCalendarCtrl::CYJCalendarCtrl()
{
	RegisterWndClass();
	CTime time = CTime::GetCurrentTime();//当前时间
	m_nSelectedYear = m_nYear	= time.GetYear();
	m_nSelectedMonth = m_nMonth	= time.GetMonth();
	m_nSelectedDay = m_nDay		= time.GetDay();
	
	m_arrWeekNames[0]	= _T("日");
	m_arrWeekNames[1]	= _T("一");
	m_arrWeekNames[2]	= _T("二");
	m_arrWeekNames[3]	= _T("三");
	m_arrWeekNames[4]	= _T("四");
	m_arrWeekNames[5]	= _T("五");
	m_arrWeekNames[6]	= _T("六");

	m_arrDaysOfMonth[0]       = 0;
	m_arrDaysOfMonth[1]       = 31;
	m_arrDaysOfMonth[2]       = 28;
	m_arrDaysOfMonth[3]       = 31;
	m_arrDaysOfMonth[4]       = 30;
	m_arrDaysOfMonth[5]       = 31;
	m_arrDaysOfMonth[6]       = 30;
	m_arrDaysOfMonth[7]       = 31;
	m_arrDaysOfMonth[8]       = 31;
	m_arrDaysOfMonth[9]       = 30;
	m_arrDaysOfMonth[10]      = 31;
	m_arrDaysOfMonth[11]      = 30;
	m_arrDaysOfMonth[12]      = 31;
	
//	m_clrBgToday		= RGB(255,255,255);
//	m_clrBgSelected		= RGB(255,255,255);
//	m_clrSelectedBorderColor=RGB(255,0,0);
	
	m_ptSelectedPrev = m_ptSelected = CPoint(-1, -1);
	m_ptToday			= CPoint(-1,-1);
	m_rcDate			= CRect(0,0,0,0);
	m_bIsCapture		= FALSE;

	GenerateCalendar();
}

//int CYJCalendarCtrl::m_arrDaysOfMonth[13] = {0, 31, 28, 31,30,31,30,31,31,30,31,30,31};

CYJCalendarCtrl::~CYJCalendarCtrl()
{
	if (m_dcMem.GetSafeHdc())
		m_dcMem.DeleteDC();
	if (m_bmpMem.GetSafeHandle())
		m_bmpMem.DeleteObject();
}


BEGIN_MESSAGE_MAP(CYJCalendarCtrl, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CYJCalendarCtrl::RegisterWndClass()
{
	WNDCLASS windowclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	//Check weather the class is registerd already

	if (!(::GetClassInfo(hInst, MYWNDCLASS, &windowclass)))
	{
		//If not then we have to register the new class

		windowclass.style = CS_DBLCLKS;// | CS_HREDRAW | CS_VREDRAW;

		windowclass.lpfnWndProc = ::DefWindowProc;
		windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
		windowclass.hInstance = hInst;
		windowclass.hIcon = NULL;
		windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
		windowclass.lpszMenuName = NULL;
		windowclass.lpszClassName = MYWNDCLASS;

		if (!AfxRegisterClass(&windowclass))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

//************************************
// Function name:
//     CalculateWeek
// Purpose:
//     计算星期及读取对应日期的数据
// Parameters:
//     NULL  
//
// Return value:
//     void
//************************************
void CYJCalendarCtrl::GenerateCalendar()
{
	m_ptToday.SetPoint(-1, -1);

	int days=0;
	if ((m_nSelectedYear%4==0 && m_nSelectedYear%100!=0) || m_nSelectedYear%400==0)
	{
		m_arrDaysOfMonth[2]=29;//闰年二月29天
	}
	else
	{
		m_arrDaysOfMonth[2]=28;//平年二月28天
	}
	for (int i=1;i<m_nSelectedMonth;i++)
	{
		days+=m_arrDaysOfMonth[i];
	}
	days++;//计算m_CurrentYear-m_CurrentMonth-1在m_CurrentYear这一年的累积天数
	int year=m_nSelectedYear-1;
	int week=year+year/4-year/100+year/400+days;
	//W = [Y-1] + [(Y-1)/4] - [(Y-1)/100] + [(Y-1)/400] + D
	//Y是年份数，D是这一天在这一年中的累积天数，也就是这一天在这一年中是第几天。
	week=week%7;//余数为几就表示为星期几，余数为0表示星期日
	int month=m_nSelectedMonth-1;
	year=m_nSelectedYear;
	if (month==0)
	{
		month=12;
		year--;
	}
	if (week==0)
	{
		week=7;
	}
	int day=m_arrDaysOfMonth[month]-week+1;
	for (int row=0;row<6;row++)
		for (week=0;week<7;week++)
		{
			m_CalendarArray[row][week].m_nYear=year;
			m_CalendarArray[row][week].m_nMonth=month;
			m_CalendarArray[row][week].m_nDay=day;
			m_CalendarArray[row][week].m_bIsToday=false;
			m_CalendarArray[row][week].m_bHasInfo=false;
			m_CalendarArray[row][week].m_bIsSelectedMonth=false;
			
			if (year == m_nYear && month == m_nMonth && day == m_nDay)
			{
				m_CalendarArray[row][week].m_bIsToday=true;
				m_ptToday.SetPoint(week, row);
			}

			if (m_nSelectedMonth == month)
			{
				m_CalendarArray[row][week].m_bIsSelectedMonth=true;

				if (m_nSelectedYear == year && m_nSelectedDay == day)
				{
					m_ptSelected.SetPoint(week,row);
				}
			}

			IncreaseDate(year, month, day);
		}
}

inline void CYJCalendarCtrl::IncreaseDate(int& nYear, int& nMonth, int& nDay)
{
	nDay++;
	if (nDay>m_arrDaysOfMonth[nMonth])
	{
		nDay=1;
		nMonth++;
		if (nMonth==13)
		{
			nMonth=1;
			nYear++;
		}
	}
}

void CYJCalendarCtrl::InitCalendar()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	m_CalendarInfo.m_nGridWidth = (rcClient.Width() - m_CalendarInfo.m_nBorderWidth) / 7;
	m_CalendarInfo.m_nGridHeight = (rcClient.Height() - m_CalendarInfo.m_nWeekHeight - m_CalendarInfo.m_nBorderWidth) / 6;

	int nWidth = m_CalendarInfo.m_nGridWidth * 7 + m_CalendarInfo.m_nBorderWidth;
	int nHeight = (m_CalendarInfo.m_nGridHeight << 1) + (m_CalendarInfo.m_nGridHeight << 2) + m_CalendarInfo.m_nBorderWidth;

	m_rcWeek = CRect(0, 0, nWidth, m_CalendarInfo.m_nWeekHeight);
	m_rcDate = CRect(0, m_rcWeek.bottom, nWidth, m_rcWeek.bottom + nHeight);
//	m_CalendarInfo.m_nGridWidth -= m_CalendarInfo.m_nBorderWidth;
//	m_CalendarInfo.m_nGridHeight -= m_CalendarInfo.m_nBorderWidth;


	CClientDC dc(this);

	if (m_dcMem.GetSafeHdc() != NULL)
	{
		m_dcMem.DeleteDC();
		m_bmpMem.DeleteObject();
	}
	if(!m_dcMem.CreateCompatibleDC(&dc))
	{
		::PostQuitMessage(0);
	}

	m_bmpMem.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	m_dcMem.SelectObject(&m_bmpMem);

	DrawCalendar();
}

void CYJCalendarCtrl::DrawBackground(CRect* pRect, COLORREF color, BOOL bRepaint /* = FALSE */)
{
	ASSERT(pRect);
	if (pRect->IsRectEmpty() || color == -1)
		return;

	m_dcMem.FillSolidRect(pRect, color);

	if (bRepaint)
	{
		CClientDC dc(this);
		dc.BitBlt(pRect->left, pRect->top, pRect->Width(), pRect->Height(), &m_dcMem, pRect->left, pRect->top, SRCCOPY);
	}
}

void CYJCalendarCtrl::DrawHead()
{
	
}

void CYJCalendarCtrl::DrawWeek()
{
	if (m_rcWeek.IsRectEmpty())
		return;

	CFont* pFont;
	if (m_CalendarInfo.m_hFontWeek)
		pFont = CFont::FromHandle(m_CalendarInfo.m_hFontWeek);
	else
		pFont = GetParent()->GetFont();

	CFont* pOldFont = m_dcMem.SelectObject(pFont);
	m_dcMem.SetTextColor(m_CalendarInfo.m_clrTextWeek);//color of the weekend
	int nPrevMode = m_dcMem.SetBkMode(TRANSPARENT);
	DrawBackground(&m_rcWeek, m_CalendarInfo.m_clrBgWeek);

	CRect rcText = m_rcWeek;
	rcText.right = rcText.left + m_CalendarInfo.m_nGridWidth;

	for (int i=0;i<7;i++)
	{//draw weekend
		m_dcMem.DrawText(m_arrWeekNames[i], &rcText, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		rcText.OffsetRect(m_CalendarInfo.m_nGridWidth, 0);
	}

	m_dcMem.SetBkMode(nPrevMode);
	m_dcMem.SelectObject(pOldFont);
}

void CYJCalendarCtrl::DrawGrid()
{
	if (!HasGrid())
		return;

	LOGBRUSH logBrushBottom;

	logBrushBottom.lbStyle = BS_SOLID;
	logBrushBottom.lbColor = m_CalendarInfo.m_clrGrid;
	logBrushBottom.lbHatch = 0;
	CPen pen;
	pen.CreatePen( PS_GEOMETRIC|PS_ENDCAP_SQUARE, m_CalendarInfo.m_nBorderWidth, &logBrushBottom );

//	CPen pen(PS_SOLID, m_CalendarInfo.m_nBorderWidth, m_CalendarInfo.m_clrGrid);
	CPen *pOldPen=m_dcMem.SelectObject(&pen);

	int nOffset = m_CalendarInfo.m_nBorderWidth >> 1;
	CPoint ptFrom = CPoint(m_rcDate.left + nOffset , m_rcDate.top + nOffset);
	CPoint ptTo = CPoint(m_rcDate.left + nOffset, m_rcDate.bottom - m_CalendarInfo.m_nBorderWidth + nOffset);

	for (int i=0;i<8;i++)
	{
		m_dcMem.MoveTo(ptFrom);
		m_dcMem.LineTo(ptTo);
		ptFrom.Offset(m_CalendarInfo.m_nGridWidth, 0);
		ptTo.Offset(m_CalendarInfo.m_nGridWidth, 0);
	}

	ptFrom.SetPoint(m_rcDate.left + nOffset, m_rcDate.top + nOffset);
	ptTo.SetPoint(m_rcDate.right - m_CalendarInfo.m_nBorderWidth + nOffset, m_rcDate.top + nOffset);
	for (int i=0;i<7;i++)
	{
		m_dcMem.MoveTo(ptFrom);
		m_dcMem.LineTo(ptTo);
		ptFrom.Offset(0, m_CalendarInfo.m_nGridHeight);
		ptTo.Offset(0, m_CalendarInfo.m_nGridHeight);
	}
	m_dcMem.SelectObject(pOldPen);
	pen.DeleteObject();
}

void CYJCalendarCtrl::DrawDate()
{
	CFont* pFont;
	if (m_CalendarInfo.m_hFontDate)
		pFont = CFont::FromHandle(m_CalendarInfo.m_hFontDate);
	else
		pFont = GetParent()->GetFont();

	CFont* pOldFont = m_dcMem.SelectObject(pFont);
	int nPrevMode = m_dcMem.SetBkMode(TRANSPARENT);
	COLORREF clrOld = m_dcMem.GetTextColor();

	int nRight = m_rcDate.left + m_CalendarInfo.m_nGridWidth;
	CRect rcDate;//(m_rcDate.left, m_rcDate.top, nRight, m_rcDate.top + m_CalendarInfo.m_nGridHeight);
	GetDayRect(&rcDate, 0, 0);
	bool bChangeColor = !m_CalendarArray[0][0].m_bIsSelectedMonth;
	for (int i=0;i<6;i++)
	{
		for (int j=0;j<7;j++)
		{
			if (bChangeColor != m_CalendarArray[i][j].m_bIsSelectedMonth)
			{
				m_dcMem.SetTextColor(m_CalendarArray[i][j].m_bIsSelectedMonth ? m_CalendarInfo.m_clrTextLightDay : m_CalendarInfo.m_clrTextDarkDay);
				bChangeColor = m_CalendarArray[i][j].m_bIsSelectedMonth;
			}

			COLORREF clrOld;
			if (m_CalendarArray[i][j].m_bIsToday)
			{
				clrOld = m_dcMem.GetTextColor();
				m_dcMem.SetTextColor(m_CalendarInfo.m_clrTextToday);
				DrawBackground(&rcDate, m_CalendarInfo.m_clrBgToday);
			}

			if (m_ptSelected.x == j && m_ptSelected.y == i)
				DrawBackground(&rcDate, m_CalendarInfo.m_clrBgSelected);

			CString strNum;
			strNum.Format(_T("%d"), m_CalendarArray[i][j].m_nDay);

			m_dcMem.DrawText(strNum, &rcDate, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

			if (m_CalendarArray[i][j].m_bIsToday)
				m_dcMem.SetTextColor(clrOld);

			rcDate.OffsetRect(m_CalendarInfo.m_nGridWidth, 0);
		}
		rcDate.OffsetRect(0, m_CalendarInfo.m_nGridHeight);
		rcDate.left = m_rcDate.left + m_CalendarInfo.m_nBorderWidth;
		rcDate.right = nRight;
	}

	m_dcMem.SetTextColor(clrOld);
	m_dcMem.SetBkMode(nPrevMode);
	m_dcMem.SelectObject(pOldFont);
}

void CYJCalendarCtrl::DrawDay(CRect* pRect, int nRow, int nColumn)
{
	if (pRect->IsRectEmpty() || !(nRow >= 0 && nRow < 6) || !(nColumn >= 0 && nColumn < 7))
		return;

	CFont* pFont;
	if (m_CalendarInfo.m_hFontDate)
		pFont = CFont::FromHandle(m_CalendarInfo.m_hFontDate);
	else
		pFont = GetParent()->GetFont();

	CFont* pOldFont = m_dcMem.SelectObject(pFont);
	int nPrevMode = m_dcMem.SetBkMode(TRANSPARENT);

	if (m_CalendarArray[nRow][nColumn].m_bIsToday)
		m_dcMem.SetTextColor(m_CalendarInfo.m_clrTextToday);
	else if (m_CalendarArray[nRow][nColumn].m_bIsSelectedMonth)
		m_dcMem.SetTextColor(m_CalendarInfo.m_clrTextLightDay);
	else
		m_dcMem.SetTextColor(m_CalendarInfo.m_clrTextDarkDay);

	if (m_ptSelected.x == nColumn && m_ptSelected.y == nRow)
		DrawBackground(pRect, m_CalendarInfo.m_clrBgSelected);
	else if (m_CalendarArray[nRow][nColumn].m_bIsToday)
		DrawBackground(pRect, m_CalendarInfo.m_clrBgToday);
	else
		DrawBackground(pRect, m_CalendarInfo.m_clrBg);


	CString strNum;
	strNum.Format(_T("%d"), m_CalendarArray[nRow][nColumn].m_nDay);

	m_dcMem.DrawText(strNum, pRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	m_dcMem.SetBkMode(nPrevMode);
	m_dcMem.SelectObject(pOldFont);
}

void CYJCalendarCtrl::DrawDay(CRect* pRect, CPoint ptPoint)
{
	DrawDay(pRect, ptPoint.y, ptPoint.x);
}

void CYJCalendarCtrl::DrawFocus(int nRow, int nColumn, BOOL bRepaint/* = FALSE*/)
{
	if (nRow < 0 || nColumn < 0)
		return;

	CRect focusRect;
	GetDayRect(&focusRect, nRow, nColumn);
// 	if (HasGrid())
// 		focusRect.DeflateRect(1, 1, 0, 0);
	DrawFocus(&focusRect, bRepaint);
}

void CYJCalendarCtrl::DrawFocus(CPoint ptPoint, BOOL bRepaint /* = FALSE */)
{
	DrawFocus(ptPoint.y, ptPoint.x, bRepaint);
}

void CYJCalendarCtrl::DrawFocus(CRect* pRect, BOOL bRepaint/* = FALSE*/)
{
	ASSERT(pRect);
	if (pRect->IsRectEmpty())
		return;

	m_dcMem.SetTextColor(RGB(255,255,255));
	m_dcMem.SetBkColor(RGB(0,0,0));
	m_dcMem.DrawFocusRect(pRect);

	if (bRepaint)
	{
		CClientDC dc(this);
		dc.BitBlt(pRect->left, pRect->top, pRect->Width(), pRect->Height(), &m_dcMem, pRect->left, pRect->top, SRCCOPY);
	}
}

//************************************
// Function name:
//     DrawCalendar
// Purpose:
//     绘制日历
// Parameters:
//     NULL
//
// Return value:
//     void
//************************************
void CYJCalendarCtrl::DrawCalendar()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	DrawBackground(&rcClient, m_CalendarInfo.m_clrBg);

 	DrawHead();
 	DrawWeek();
 	DrawDate();
 	DrawGrid();
	DrawFocus(m_ptSelected.y, m_ptSelected.x);
}

void CYJCalendarCtrl::GetDayRect(CRect* pRect, int nRow, int nColumn)
{
	pRect->SetRect(m_rcDate.left + m_CalendarInfo.m_nBorderWidth, m_rcDate.top + m_CalendarInfo.m_nBorderWidth, m_rcDate.left + m_CalendarInfo.m_nGridWidth, m_rcDate.top + m_CalendarInfo.m_nGridHeight);

	if (nRow > 0 || nColumn > 0)
		pRect->OffsetRect(m_CalendarInfo.m_nGridWidth*nColumn, m_CalendarInfo.m_nGridHeight*nRow);
}

void CYJCalendarCtrl::GetDayRect(CRect* pRect, CPoint ptPoint)
{
	GetDayRect(pRect, ptPoint.y, ptPoint.x);
}

void CYJCalendarCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CYJCalendarCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bIsCapture)
	{
		CPoint pt = GetMousetPointInArray(point);
		if (pt != CPoint(-1, -1) && pt != m_ptSelected)
		{
			DrawFocus(m_ptSelected, TRUE);
			DrawFocus(pt, TRUE);
			m_ptSelected = pt;
//			Invalidate();
		}
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CYJCalendarCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	InitCalendar();
}

CPoint CYJCalendarCtrl::GetMousetPointInArray(CPoint point)
{
	if (m_rcDate.PtInRect(point))
	{
		for (int i=0;i<6;i++)
		{
			for (int j=0;j<7;j++)
			{
				CRect gridRect;
				GetDayRect(&gridRect, i, j);
				if (gridRect.PtInRect(point))
					return CPoint(j,i);
			}
		}
	}
	return CPoint(-1,-1);
}

void CYJCalendarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_dcMem.GetSafeHdc())
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	CPoint pt = this->GetMousetPointInArray(point);

	if (pt == CPoint(-1,-1))
	{
		CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	m_ptSelectedPrev = m_ptSelected;
	if (pt != m_ptSelected)
	{
		DrawFocus(m_ptSelected, TRUE);
		DrawFocus(pt, TRUE);
		m_ptSelected = pt;

		//Invalidate();
	}

	m_bIsCapture = TRUE;
	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void CYJCalendarCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
// 	CPoint pt = this->GetMousetPointInArray(point);
// 	if (pt != CPoint(-1, -1) && pt != m_ptSelected)
// 		m_ptSelected = pt;

	if (m_bIsCapture)
	{
		ReleaseCapture();
		m_bIsCapture = FALSE;
	}

	if (!m_CalendarArray[m_ptSelected.y][m_ptSelected.x].m_bIsSelectedMonth)
	{
		ChangeMonth(m_CalendarArray[m_ptSelected.y][m_ptSelected.x].m_nYear, m_CalendarArray[m_ptSelected.y][m_ptSelected.x].m_nMonth, m_CalendarArray[m_ptSelected.y][m_ptSelected.x].m_nDay);
	}
	else if (m_ptSelected != m_ptSelectedPrev)
	{
		CClientDC dc(this);
		CRect rcDay;
		GetDayRect(&rcDay, m_ptSelectedPrev);
		DrawDay(&rcDay, m_ptSelectedPrev);
		dc.BitBlt(rcDay.left, rcDay.top, rcDay.Width(), rcDay.Height(), &m_dcMem, rcDay.left, rcDay.top, SRCCOPY);


		GetDayRect(&rcDay, m_ptSelected);
		DrawDay(&rcDay, m_ptSelected);
		DrawFocus(m_ptSelected);
		dc.BitBlt(rcDay.left, rcDay.top, rcDay.Width(), rcDay.Height(), &m_dcMem, rcDay.left, rcDay.top, SRCCOPY);

		m_nSelectedDay = m_CalendarArray[m_ptSelected.y][m_ptSelected.x].m_nDay;
		CallBackDateChange();
//		Invalidate();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CYJCalendarCtrl::ChangeMonth(int nYear,int nMonth,int nDay)
{
	m_nSelectedYear = nYear;
	m_nSelectedMonth = nMonth;
	m_nSelectedDay = nDay;

	this->GenerateCalendar();

	this->DrawCalendar();

	Invalidate();

	for (int i=0;i<m_arrNotify.GetSize();i++)
	{
		::SendMessage(m_arrNotify[i], WM_YJCALENDAR_MONTH_CHANGED, 0, 0);
		::SendMessage(m_arrNotify[i], WM_YJCALENDAR_DATE_CHANGED, 0, 0);
	}
}

void CYJCalendarCtrl::AddNotify(HWND hWnd)
{
	if (hWnd)
		m_arrNotify.Add(hWnd);
}

void CYJCalendarCtrl::CallBackDateChange()
{
	for (int i=0;i<m_arrNotify.GetSize();i++)
	{
		::SendMessage(m_arrNotify[i], WM_YJCALENDAR_DATE_CHANGED, 0, 0);
	}
}

void CYJCalendarCtrl::CallBackDBClick()
{
	for (int i=0;i<m_arrNotify.GetSize();i++)
	{
		::SendMessage(m_arrNotify[i], WM_YJCALENDAR_DBLCLICK, 0, 0);
	}
}

int CYJCalendarCtrl::GetSelectedYear()
{
	return m_nSelectedYear;
}

int CYJCalendarCtrl::GetSelectedMonth()
{
	return m_nSelectedMonth;
}

int CYJCalendarCtrl::GetSelectedDay()
{
	return m_nSelectedDay;
}

int CYJCalendarCtrl::GetSelectedWeekOfYear()
{
	return 0;
}
void CYJCalendarCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint pt = this->GetMousetPointInArray(point);
	if (pt.x != -1)
	{
		this->CallBackDBClick();
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

int CYJCalendarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitCalendar();

	return 0;
}

void CYJCalendarCtrl::NextMonth()
{
	m_nSelectedDay = 1;
	if ( ++m_nSelectedMonth > 12)
	{
		m_nSelectedMonth = 1;
		m_nSelectedYear++;
	}
	 
	ChangeMonth(m_nSelectedYear, m_nSelectedMonth, m_nSelectedDay);
}

void CYJCalendarCtrl::PrevMonth()
{
	m_nSelectedDay = 1;
	if ( --m_nSelectedMonth < 1 )
	{
		m_nSelectedMonth = 1;
		m_nSelectedYear--;
	}

	ChangeMonth(m_nSelectedYear, m_nSelectedMonth, m_nSelectedDay);
}

void CYJCalendarCtrl::NextYear()
{
	m_nSelectedDay = 1;
//	m_nSelectedMonth = 1;
	m_nSelectedYear++;
	ChangeMonth(m_nSelectedYear, m_nSelectedMonth, m_nSelectedDay);
}

void CYJCalendarCtrl::PrevYear()
{
	m_nSelectedDay = 1;
//	m_nSelectedMonth = 1;
	m_nSelectedYear--;
	ChangeMonth(m_nSelectedYear, m_nSelectedMonth, m_nSelectedDay);
}

void CYJCalendarCtrl::GoToday()
{
	ChangeMonth(m_nYear, m_nMonth, m_nDay);
}

BOOL CYJCalendarCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&m_dcMem,0,0,SRCCOPY);

	return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}

void CYJCalendarCtrl::GetSelectedWeekRegion(int& nYearFrom,int& nMonthFrom,int& nDayFrom, int& nYearTo,int& nMonthTo,int& nDayTo)
{
	nYearFrom	= m_CalendarArray[m_ptSelected.y][0].m_nYear;
	nMonthFrom	= m_CalendarArray[m_ptSelected.y][0].m_nMonth;
	nDayFrom	= m_CalendarArray[m_ptSelected.y][0].m_nDay;

	nYearTo		= m_CalendarArray[m_ptSelected.y][6].m_nYear;
	nMonthTo	= m_CalendarArray[m_ptSelected.y][6].m_nMonth;
	nDayTo		= m_CalendarArray[m_ptSelected.y][6].m_nDay;
}

byte CYJCalendarCtrl::GetDaysOfSelectedMonth()
{
	return m_arrDaysOfMonth[m_nSelectedMonth];
}