#pragma once

#define WM_REBAR_CONTEXTMENU (WM_USER+1)

class AFX_EXT_CLASS CYJReBar : public CReBar
{
	DECLARE_DYNAMIC(CYJReBar)

public:
	CYJReBar();
	virtual ~CYJReBar();

	void Lock( bool bLock =true );
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
};


