// YJReBar.cpp : 实现文件
//

#include "stdafx.h"
#include "YJReBar.h"


// CYJReBar

IMPLEMENT_DYNAMIC(CYJReBar, CReBar)
CYJReBar::CYJReBar()
{
}

CYJReBar::~CYJReBar()
{
}


BEGIN_MESSAGE_MAP(CYJReBar, CReBar)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


void CYJReBar::Lock( bool bLock /*=true*/ )
{

	REBARBANDINFO rbbi;
	rbbi.cbSize = sizeof( rbbi );
	rbbi.fMask  = RBBIM_STYLE | RBBIM_ID;

	for ( UINT nBand = 0; nBand < GetReBarCtrl().GetBandCount(); nBand++ )
	{
		VERIFY( GetReBarCtrl().GetBandInfo( nBand, &rbbi ) );

		if ( bLock )
		{
			rbbi.fStyle &= ~RBBS_GRIPPERALWAYS;
			rbbi.fStyle |= RBBS_NOGRIPPER;
		}
		else
		{
			rbbi.fStyle |= RBBS_GRIPPERALWAYS;
			rbbi.fStyle &= ~RBBS_NOGRIPPER;
		}

		VERIFY( GetReBarCtrl().SetBandInfo( nBand, &rbbi ) );
	}

	GetParentFrame()->DelayRecalcLayout();

}


void CYJReBar::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	ClientToScreen(&point);
	GetParent()->SendMessage(WM_REBAR_CONTEXTMENU,(WPARAM)0, (LPARAM)&point);
}
