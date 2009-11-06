#pragma once

#include "YJOptionTreeCtrl.h"
#include "YJOptionPageBase.h"
#include <afxtempl.h>
// CYJOptionDlg 对话框

typedef CArray<CYJOptionPageBase*,CYJOptionPageBase*&> CPageArray;

class AFX_EXT_CLASS CYJOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CYJOptionDlg)

private:
	CYJOptionTreeCtrl		m_ctrlLeftTree;
	CStatic					m_ctrlGroupBox;
	CRect					m_rtPageRect;
	CRect					m_rtTreeRect;
	CRect					m_rtGroupBoxRect;
	CPageArray				m_aPages;

	HTREEITEM FindItem(CString strPath, BOOL bCreate = TRUE);
public:
	CYJOptionDlg(UINT nIDResource, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CYJOptionDlg();

	BOOL AddPage(CYJOptionPageBase* pPage, UINT nIDResource, CString strPagePath);
	void SetTreeCtrl(UINT nIDResource);
	void SetGroupBox(UINT nIDResourse);
	BOOL SelectPage(CString strPagePath);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	afx_msg LRESULT OnPageChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	void LoadPreferences();
	void SavePreferences();

};
