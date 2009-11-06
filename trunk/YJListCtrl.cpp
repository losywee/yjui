// YJListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "YJListCtrl.h"
#include "ThemeHelper.h"


namespace ListSort{
	struct PARAMSORT
	{
		PARAMSORT(HWND hWnd, int columnIndex, bool ascending)
			:m_hWnd(hWnd)
			,m_ColumnIndex(columnIndex)
			,m_Ascending(ascending)
		{}

		HWND m_hWnd;
		int  m_ColumnIndex;
		bool m_Ascending;
	};

	// Comparison extracts values from the List-Control
	int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		PARAMSORT& ps = *(PARAMSORT*)lParamSort;

		TCHAR left[256] = _T(""), right[256] = _T("");
		ListView_GetItemText(ps.m_hWnd, lParam1, ps.m_ColumnIndex, left, sizeof(left));
		ListView_GetItemText(ps.m_hWnd, lParam2, ps.m_ColumnIndex, right, sizeof(right));

		if (ps.m_Ascending)
			return _tcscmp( left, right );
		else
			return _tcscmp( right, left );
	}

	HBITMAP CreateSortBitmap(bool bAscending)
	{
		CDC* pDC = CDC::FromHandle(::GetDC(::GetDesktopWindow()));

		CBrush brush;

		CRect iconRect(0, 0, 16, 16);

		//create a brush
		brush.CreateSolidBrush(RGB(0, 0, 0));

		//create a memory dc
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);

		//Create a memory bitmap
		CBitmap newbmp;
		newbmp.CreateCompatibleBitmap(pDC, iconRect.Height(), iconRect.Width());

		//release the Display DC
		::ReleaseDC(::GetDesktopWindow(), pDC->Detach());

		//select the bitmap in the memory dc
		CBitmap *pOldBitmap = memDC.SelectObject(&newbmp);

		//make the bitmap white to begin with
		memDC.FillSolidRect(iconRect.top,iconRect.left,iconRect.bottom,iconRect.right,::GetSysColor(COLOR_3DFACE));

		//draw a rectangle and an ellipse on the Display using the brush
		CBrush *pOldBrush = memDC.SelectObject(&brush);
		if (!bAscending)
		{
			// Arrow pointing down
			CPoint Pt[3];
			Pt[0] = CPoint(10, 6);	// Right
			Pt[1] = CPoint(4, 6);	// Left
			Pt[2] = CPoint(7, 9);	// Bottom
			memDC.Polygon(Pt, 3);
		}
		else
		{
			// Arrow pointing up
			CPoint Pt[3];
			Pt[0] = CPoint(7,  5);	// Top
			Pt[1] = CPoint(4,  8);	// Left
			Pt[2] = CPoint(10,  8);	// Right
			memDC.Polygon(Pt, 3);
		}
		memDC.SelectObject(pOldBrush);

		//select old bitmap back into the memory dc
		memDC.SelectObject(pOldBitmap);

		return (HBITMAP)newbmp.Detach();
	}
}

// CYJListCtrl

IMPLEMENT_DYNAMIC(CYJListCtrl, CListCtrl)
CYJListCtrl::CYJListCtrl()
	:m_SortCol(-1)
	,m_Ascending(false)
{
}

CYJListCtrl::~CYJListCtrl()
{
}


BEGIN_MESSAGE_MAP(CYJListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnHeaderClick)
END_MESSAGE_MAP()



// CYJListCtrl 消息处理程序
bool CYJListCtrl::SortColumn(int columnIndex, bool ascending)
{
	// virtual lists cannot be sorted with this method
	if (GetStyle() & LVS_OWNERDATA)
		return false;

	if (GetItemCount()<=0)
		return true;

	// Uses SortItemsEx because it requires no knowledge of datamodel
	//	- CListCtrl::SortItems() is faster with direct access to the datamodel
	ListSort::PARAMSORT paramsort(m_hWnd, columnIndex, ascending);
	ListView_SortItemsEx(m_hWnd, ListSort::SortFunc, &paramsort);

	SetSortArrow(columnIndex, ascending);
	m_SortCol = columnIndex;
	m_Ascending = ascending;

	return true;
}

void CYJListCtrl::OnHeaderClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	BeginWaitCursor();
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	int nCol = pNMLV->iSubItem;

	if (m_SortCol==nCol)
	{
		m_Ascending = !m_Ascending;
	}
	else
	{
		m_SortCol = nCol;
		m_Ascending = true;
	}

	SortColumn(m_SortCol, m_Ascending);
	EndWaitCursor();

	*pResult = 0;
}

void CYJListCtrl::SetSortArrow(int colIndex, bool ascending)
{
	if (CThemeHelper().AreControlsThemed())
	{
#if (_WIN32_WINNT >= 0x501)
		for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); ++i)
		{
			HDITEM hditem = {0};
			hditem.mask = HDI_FORMAT;
			VERIFY( GetHeaderCtrl()->GetItem( i, &hditem ) );
			hditem.fmt &= ~(HDF_SORTDOWN|HDF_SORTUP);
			if (i == colIndex)
			{
				hditem.fmt |= ascending ? HDF_SORTUP : HDF_SORTDOWN ;
			}
			VERIFY( CListCtrl::GetHeaderCtrl()->SetItem( i, &hditem ) );
		}
#endif
	}
	else
	{
		for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); ++i)
		{
			HDITEM hditem = {0};
			hditem.mask = HDI_BITMAP | HDI_FORMAT;
			VERIFY( GetHeaderCtrl()->GetItem( i, &hditem ) );
			if (hditem.fmt & HDF_BITMAP && hditem.fmt & HDF_BITMAP_ON_RIGHT)
			{
				if (hditem.hbm)
				{
					DeleteObject(hditem.hbm);
					hditem.hbm = NULL;
				}
				hditem.fmt &= ~(HDF_BITMAP|HDF_BITMAP_ON_RIGHT);
				VERIFY( CListCtrl::GetHeaderCtrl()->SetItem( i, &hditem ) );
			}
			if (i == colIndex)
			{
				hditem.fmt |= HDF_BITMAP|HDF_BITMAP_ON_RIGHT;
				//UINT bitmapID = m_Ascending ? IDB_DOWNARROW : IDB_UPARROW; 
				//hditem.hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(bitmapID), IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS);
				hditem.hbm = ListSort::CreateSortBitmap(ascending);
				VERIFY( hditem.hbm!=NULL );
				VERIFY( CListCtrl::GetHeaderCtrl()->SetItem( i, &hditem ) );
			}
		}
	}
}