#pragma once

#include "YJWndBox.h"
// CYJSplitter

class AFX_EXT_CLASS CYJSplitter : public CWnd
{
	DECLARE_DYNAMIC(CYJSplitter)

private:
	int			m_nFMinSize;
	int			m_nSMinSize;
	int			m_nSplitterWidth;
//	CRect		m_rcPanelMargin;
	BOOL		m_bIsHorizSplitter;
	CYJWndBox*	m_pFirstWndBox;
	CYJWndBox*	m_pSecondWndBox;
	CRect		m_rcFirstWnd;
	CRect		m_rcSecondWnd;
	CRect		m_rcSplitter;
	float		m_fSplitterPos;
	BOOL		m_bMovingSplitter;
	BOOL		m_bShowPanelBorder;
	int			m_nPanelBorder;

	COLORREF	m_clrBackground;
	COLORREF	m_clrBorder;

public:
	CYJSplitter();
	virtual ~CYJSplitter();

	void SetSplitterWidth(int nWidth, BOOL bRepaint = FALSE);
	void SetMinSize(int nFirstPanel = 0, int nSecondPanel = 0);
	void ShowPanelBorder(BOOL bShow = TRUE, BOOL bRepaint = FALSE);
	void SetPanelBorder(int nBorder = 1, BOOL bRepaint = FALSE);
	int GetPanelBorder(){ return m_nPanelBorder; }
	void SetBackgroundColor(COLORREF clrBg, BOOL bRepaint = FALSE);
	void SetBorderColor(COLORREF clrBorder, BOOL bRepaint = FALSE);
	void SetSplitterPos(float fPos = 50.0) { m_fSplitterPos = fPos; CalculatePanelRect(); ResizePanel(); Invalidate(); }
	float GetSplitterPos() { return m_fSplitterPos; }

private:
	inline void HideBox(CYJWndBox* pWndBox);
	inline void ShowBox(CYJWndBox* pWndBox);

protected:
	void DrawSplitterMoveBar();
	void CalculatePanelRect();
	void ResizePanel();
	void CheckPanelSize();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CYJWndBox* pFirstWndBox = NULL, CYJWndBox* pSecondWndBox = NULL, BOOL bIsHorizSplitter = TRUE);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	
};


