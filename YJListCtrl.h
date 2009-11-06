#pragma once


// CYJListCtrl

class AFX_EXT_CLASS CYJListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CYJListCtrl)

public:
	CYJListCtrl();
	virtual ~CYJListCtrl();

	virtual bool SortColumn(int columnIndex, bool ascending);

protected:
	void SetSortArrow(int colIndex, bool ascending);

	int			m_SortCol;
	bool		m_Ascending;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHeaderClick(NMHDR *pNMHDR, LRESULT *pResult);
};


