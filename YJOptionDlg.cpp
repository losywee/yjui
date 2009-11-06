// YJOptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "YJOptionDlg.h"
#include "Shared/Preferences.h"

#define TREE_MARGIN_TOP			10
#define TREE_MARGIN_BOTTOM		45
#define TREE_MARGIN_LEFT		10
#define TREE_WIDTH				120

#define GROUP_BOX_MARGIN_TOP	TREE_MARGIN_TOP
#define GROUP_BOX_MARGIN_BOTTOM	TREE_MARGIN_BOTTOM
#define GROUP_BOX_MARGIN_LEFT	(TREE_MARGIN_LEFT + TREE_WIDTH + 10)
#define GROUP_BOX_MARGIN_RIGHT	10

#define PAGE_MARGIN_TOP			(GROUP_BOX_MARGIN_TOP + 15)
#define PAGE_MARGIN_BOTTOM		(GROUP_BOX_MARGIN_BOTTOM + 10)
#define PAGE_MARGIN_LEFT		(GROUP_BOX_MARGIN_LEFT + 10)
#define PAGE_MARGIN_RIGHT		(GROUP_BOX_MARGIN_RIGHT + 10)

// CYJOptionDlg 对话框

IMPLEMENT_DYNAMIC(CYJOptionDlg, CDialog)
CYJOptionDlg::CYJOptionDlg(UINT nIDResource,CWnd* pParent /*=NULL*/)
	: CDialog(nIDResource, pParent)
{
	m_aPages.RemoveAll();
}

CYJOptionDlg::~CYJOptionDlg()
{
}

void CYJOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CYJOptionDlg, CDialog)
	//{{AFX_MSG_MAP
	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PAGE_CHANGED,OnPageChanged)
END_MESSAGE_MAP()


HTREEITEM CYJOptionDlg::FindItem(CString strPath, BOOL bCreate/* = TRUE*/)
{
	ASSERT(m_ctrlLeftTree.GetSafeHwnd());

	HTREEITEM hCurrent = m_ctrlLeftTree.GetRootItem();
	int index;
	bool bIsFound = true;
	while (true)
	{
		CString strTitle;
		if ((index = strPath.Find('\\')) != -1)
		{
			strTitle = strPath.Left(index + 1);
			strPath = strPath.Right(strPath.GetLength() - index -1);
		}
		else if (!strPath.IsEmpty())
		{
			strTitle = strPath;
			strPath = "";
		}
		else
		{
			break;
		}

		if (bIsFound)
		{
			bool found = false;
			do 
			{
				if (m_ctrlLeftTree.GetItemText(hCurrent).CompareNoCase(strTitle) == 0)
				{
					found = true;
					break;
				}
			} while ((hCurrent = m_ctrlLeftTree.GetNextSiblingItem(hCurrent)) != NULL);

			if (found)
				continue;

			if (!bCreate)
				return NULL;
			else
				bIsFound = false;
		}

		if (!bIsFound)
		{
			hCurrent = m_ctrlLeftTree.InsertItem(strTitle,hCurrent);
		}
	}

	return hCurrent;
}

BOOL CYJOptionDlg::AddPage(CYJOptionPageBase* pPage, UINT nIDResource, CString strPagePath)
{
	if (!m_ctrlLeftTree.GetSafeHwnd())
		return FALSE;

	pPage->Create(nIDResource,this);

	m_aPages.Add(pPage);
	m_ctrlLeftTree.SetItemData(FindItem(strPagePath),(LPARAM)pPage);
	
	pPage->ShowWindow(SW_HIDE);
	pPage->MoveWindow(&m_rtPageRect);

	return TRUE;
}

void CYJOptionDlg::SetTreeCtrl(UINT nIDResource)
{
	m_ctrlLeftTree.SubclassDlgItem(nIDResource,this);
	m_ctrlLeftTree.MoveWindow(&m_rtTreeRect);
}

void CYJOptionDlg::SetGroupBox(UINT nIDResourse)
{
	m_ctrlGroupBox.SubclassDlgItem(nIDResourse,this);
	m_ctrlGroupBox.MoveWindow(&m_rtGroupBoxRect);
}

BOOL CYJOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rtClientRect;
	GetClientRect(&rtClientRect);
	m_rtTreeRect.SetRect(TREE_MARGIN_LEFT,TREE_MARGIN_TOP,TREE_MARGIN_LEFT + TREE_WIDTH,rtClientRect.bottom - TREE_MARGIN_BOTTOM);
	m_rtGroupBoxRect.SetRect(GROUP_BOX_MARGIN_LEFT,GROUP_BOX_MARGIN_TOP,rtClientRect.right - GROUP_BOX_MARGIN_RIGHT,rtClientRect.bottom - GROUP_BOX_MARGIN_BOTTOM);

	m_rtPageRect.top = PAGE_MARGIN_TOP;
	m_rtPageRect.bottom = rtClientRect.bottom - PAGE_MARGIN_BOTTOM;
	m_rtPageRect.left = PAGE_MARGIN_LEFT;
	m_rtPageRect.right = rtClientRect.right - PAGE_MARGIN_RIGHT;


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CYJOptionDlg::SelectPage(CString strPagePath)
{
	HTREEITEM hItem;
	if ((hItem = FindItem(strPagePath,FALSE)) != NULL)
	{
		m_ctrlLeftTree.Select(hItem,TVGN_CARET);
		return TRUE;
	}
	return FALSE;
}

LRESULT CYJOptionDlg::OnPageChanged(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hItem = (HTREEITEM)wParam;
	if (!hItem)
		return 0;

	m_ctrlGroupBox.SetWindowText(m_ctrlLeftTree.GetItemText(hItem));

	return 0;
}

void CYJOptionDlg::OnOK()
{
	SavePreferences();
	CDialog::OnOK();
}

void CYJOptionDlg::LoadPreferences()
{
	if (m_aPages.GetSize() > 0)
	{
		CPreferences prefs;

		for (int i = 0;i<m_aPages.GetSize();i++)
		{
			CYJOptionPageBase* pPage = m_aPages[i];
			if (pPage && pPage->GetSafeHwnd())
			{
				pPage->LoadPreferences(prefs);
				pPage->UpdateData(FALSE);
			}
		}
	}
}


void CYJOptionDlg::SavePreferences()
{
	if (m_aPages.GetSize() > 0)
	{
		CPreferences prefs;

		for (int i = 0;i<m_aPages.GetSize();i++)
		{
			CYJOptionPageBase* pPage = m_aPages[i];
			if (pPage && pPage->GetSafeHwnd())
			{
				pPage->UpdateData();
				pPage->SavePreferences(prefs);
			}
		}
	}
}


