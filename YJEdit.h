#pragma once

#include "YJListWnd.h"
// CYJEdit

class AFX_EXT_CLASS CYJEdit : public CEdit
{
	DECLARE_DYNAMIC(CYJEdit)
private:
	CBrush		m_brush;			// background brush for WM_CTLCOLOR
	CBrush		m_brushDisabled;
	CString		m_strPromptText;	// prompt text to display initially
	COLORREF	m_crPromptColor;	// color to use for prompt text
	BOOL		m_bShowPromtText;
	COLORREF	m_crBkColor;
	COLORREF	m_crTextColor;
	BOOL		m_bUseAutoComplete;
	CYJListWnd	m_wndDropList;
	int			m_nWindowTextLength;
	BOOL		m_bIsSetPromptText;


	CString ExpandShortcut(CString &inFile);
	void OnDropDown(CString strWindowText, BOOL bAutoComplete = TRUE);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYJEdit)
protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	CYJEdit();
	virtual ~CYJEdit();
	COLORREF	GetPromptColor()	{ return m_crPromptColor; }
	COLORREF	GetBkColor()		{ return m_crBkColor; }
	COLORREF	GetTextColor()		{ return m_crTextColor; }
	CString		GetPromptText()		{ return m_strPromptText; }
	int			GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;
	void		GetWindowText(CString& rString) const;
	void		SetPromptColor(COLORREF crText, BOOL bRepaint = TRUE);
	void		SetPromptText(LPCTSTR lpszPrompt);
	void		SetWindowText(LPCTSTR lpszString);
	void		ShowPromptText();
	void		SetBkColor(COLORREF crText, BOOL bRepaint = TRUE);
	void		SetTextColor(COLORREF crText, BOOL bRepaint = TRUE);
	void		SetPopupTextColor(COLORREF clrText, BOOL bRepaint = TRUE);
	void		EnableAutoComplete();
	void		AddPopupString(LPCTSTR szText, LPCTSTR szNote = NULL);
	void		AddPopupStrings(CItemArray& arrStr);
	void		RemoveAll();

protected:
	//{{AFX_MSG(CYJEdit)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnListSelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListSelectChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


