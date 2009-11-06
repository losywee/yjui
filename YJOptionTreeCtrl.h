#pragma once


// CYJOptionTreeCtrl
#define WM_PAGE_CHANGED		WM_USER + 100

class CYJOptionTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CYJOptionTreeCtrl)

private:
	void ShowPage(HTREEITEM hItem, BOOL bShow);
public:
	CYJOptionTreeCtrl();
	virtual ~CYJOptionTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


