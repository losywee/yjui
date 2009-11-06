// YJOptionTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "YJOptionTreeCtrl.h"


// CYJOptionTreeCtrl

IMPLEMENT_DYNAMIC(CYJOptionTreeCtrl, CTreeCtrl)
CYJOptionTreeCtrl::CYJOptionTreeCtrl()
{
}

CYJOptionTreeCtrl::~CYJOptionTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CYJOptionTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
END_MESSAGE_MAP()



// CYJOptionTreeCtrl 消息处理程序


void CYJOptionTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	ShowPage(pNMTreeView->itemOld.hItem,FALSE);
	ShowPage(pNMTreeView->itemNew.hItem,TRUE);

	GetParent()->SendMessage(WM_PAGE_CHANGED,(WPARAM)pNMTreeView->itemNew.hItem,(LPARAM)0);

	*pResult = 0;
}

void CYJOptionTreeCtrl::ShowPage(HTREEITEM hItem, BOOL bShow)
{

	UINT nCmdShow = bShow?SW_SHOW:SW_HIDE;

	if(hItem)
	{
		CDialog* pPage = (CDialog* )GetItemData(hItem);
		if(pPage)
		{
			pPage->ShowWindow(nCmdShow);
		}

	}

}
