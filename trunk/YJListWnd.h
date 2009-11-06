#pragma once

#include <afxtempl.h>

#define LIST_SELECTED			WM_USER + 1200
#define LIST_SELECT_CHANGED		WM_USER + 1201

class CYJDropScrollBar : public CScrollBar
{
	DECLARE_DYNAMIC(CYJDropScrollBar)
private:
	BOOL		m_bTracking;
	BOOL		m_bIsCapture;
public:
	CYJDropScrollBar();
	virtual ~CYJDropScrollBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
};

#pragma pack(1)
typedef struct _LIST_ITEM
{
	_LIST_ITEM(LPCTSTR szText = NULL, LPCTSTR szNote = NULL) : strItemText(szText),strNote(szNote) {}
	//_LIST_ITEM(_LIST_ITEM item){strItemText = item.strItemText;strNote = item.strNote;}

	CString strItemText;
	CString strNote;
}LIST_ITEM;
#pragma pack()

typedef CArray<LIST_ITEM,LIST_ITEM&> CItemArray;

class AFX_EXT_CLASS CYJListWnd : public CWnd
{
	DECLARE_DYNAMIC(CYJListWnd)

private:
	CWnd*				m_pParent;
	CYJDropScrollBar	m_ctrlVertBar;
	BOOL				m_bIsInit;
	COLORREF			m_clrText;
	int					m_nRowHeight;
	int					m_nCurSel;
	int					m_nVisibleItemCount;
	int					m_nTopIndex;
	BOOL				m_bIsCapture;
	BOOL				m_bTracking;
	BOOL				m_bSizing;
	int					m_nMouseDiffX;
	int					m_nMouseDiffY;
	CDC*				m_pDesktopDC;
	CRect				m_rcOldFocusRect;
	int					m_cxScreen;
	int					m_cyScreen;

	CItemArray			m_arrItemList;
	CItemArray			m_arrItemDisplayList;
	CRect				m_rcWnd;
	BOOL				m_bIsFirstShow;
	int					m_nMinHeight;


	void SetScroller();
public:
	CYJListWnd();
	virtual ~CYJListWnd();

	void Init(CWnd* pParent);
	void AddString(LPCTSTR szText, LPCTSTR szNote = NULL);
	void AddStrings(CItemArray& arrItem);
	void RemoveAll(){m_arrItemList.RemoveAll();m_arrItemDisplayList.RemoveAll();}
	void SetTextColor(COLORREF clrText, BOOL bRepaint = FALSE);
	void SetRowHeight(int nHeight, BOOL bRepaint = FALSE);
	void SetTopIndex(int nTopIndex, BOOL bRepaint = FALSE);
	void StartCapture();
	int FindLikeString(const CString str);
	void FindAll();
	void CalculateWindowRect();

	COLORREF	GetTextColor(){return m_clrText;}
	int			GetRowHeight(){return m_nRowHeight;}
	int			GetTopIndex(){return m_nTopIndex;}
	int			GetItemCount(){return (int)m_arrItemDisplayList.GetCount();}
	void		GetString(CString& strOut, int nItem);

	static int CompareString(const void* p1, const void* p2);

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(CDC* pDC, int nWidth);
	virtual void DrawSize(CDC* pDC, CRect& rect);
	virtual void SortString(CArray<LIST_ITEM,LIST_ITEM&>& arrList);
	int HitTest(CPoint ptPoint);
	long GetScrollBarWidth();
public:
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


