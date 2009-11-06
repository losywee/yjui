#pragma once

#include <afxtempl.h>

#define PANEL_WIDTH_FIX			1
#define PANEL_WIDTH_AUTO		2
#define PANEL_HEIGHT_FIX		4
#define PANEL_HEIGHT_AUTO		8
#define PANEL_ALIGN_LEFT		16
#define PANEL_ALIGN_CENTER		32
#define PANEL_ALIGN_RIGHT		64
#define PANEL_VALIGN_TOP		128
#define PANEL_VALIGN_CENTER		256
#define PANEL_VALIGN_BOTTOM		512
#define PANEL_FIX				PANEL_WIDTH_FIX|PANEL_HEIGHT_FIX
#define PANEL_AUTO				PANEL_WIDTH_AUTO|PANEL_HEIGHT_AUTO
#define PANEL_LEFT_TOP			PANEL_ALIGN_LEFT|PANEL_VALIGN_TOP
#define PANEL_LEFT_BOTTOM		PANEL_ALIGN_LEFT|PANEL_VALIGN_BOTTOM
#define PANEL_CENTER			PANEL_ALIGN_CENTER|PANEL_VALIGN_CENTER
#define PANEL_RIGHT_TOP			PANEL_ALIGN_RIGHT|PANEL_VALIGN_TOP
#define PANEL_RIGHT_BOTTOM		PANEL_ALIGN_RIGHT|PANEL_VALIGN_BOTTOM

typedef struct tagPanelInfo{
	HWND	m_hWnd;
	int		m_nWidth;
	int		m_nHeight;
	CRect	m_rcMargin;
	UINT	m_nStyle;
	CRect	m_rcPosition;

	tagPanelInfo(HWND hwnd = NULL, int nWidth = 0, int nHeight = 0, CRect rcMargin = CRect(0,0,0,0), UINT nStyle = PANEL_WIDTH_FIX|PANEL_HEIGHT_FIX|PANEL_ALIGN_LEFT|PANEL_VALIGN_TOP ):
		m_hWnd(hwnd),
		m_nWidth(nWidth),
		m_nHeight(nHeight),
		m_rcMargin(rcMargin),
		m_nStyle(nStyle)
	{};
}PANEL_INFO;

typedef CArray<PANEL_INFO,PANEL_INFO&> CPanelArray;

class AFX_EXT_CLASS CYJWndBox
{
private:
	CPanelArray			m_arrPanels;
	CRect				m_rcBox;

	void CalculatePanelPosition(PANEL_INFO& panelInfo);

public:
	CYJWndBox(CRect rcBox = CRect(0,0,0,0));
	~CYJWndBox(void);

	BOOL AddPanel(HWND hWnd, int nWidth, int nHeight, CRect rcMargin = CRect(0,0,0,0), UINT nStyle = PANEL_FIX|PANEL_LEFT_TOP);
	void OnSize(CRect rcNewSize);
	void ShowWindow(int nCmdShow = SW_SHOW);
	void SetParent(CWnd* pParentWnd);
	void SetOwner(CWnd* pOwnerWnd);
};
