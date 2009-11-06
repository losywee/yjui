#pragma once


// CYJButton

class AFX_EXT_CLASS CYJButton : public CButton
{
	DECLARE_DYNAMIC(CYJButton)
//=============================================================================
// Construction
//=============================================================================
public:
	CYJButton();
	virtual ~CYJButton();

//=============================================================================
// Attributes
//=============================================================================
public:
	static enum ICON_ALIGNMENT
	{
		ICON_CENTER = 0,
		ICON_LEFT,
		ICON_RIGHT
	};

	static enum BUTTON_STATUS
	{
		STATUS_NORMAL = 0,
		STATUS_HOVER,
		STATUS_PRESSED,
		STATUS_CHECKED_NORMAL,
		STATUS_CHECKED_HOVER,
		STATUS_CHECKED_PRESSED
	};

private:
	BOOL			m_bTracking;
	BOOL			m_bMouseOver;

#pragma pack(1)
	typedef struct _STRUCT_CALLBACK
	{
		HWND		hWnd;			// Handle to window
		UINT		nMessage;		// Message identifier
		WPARAM		wParam;
		LPARAM		lParam;
	} STRUCT_CALLBACK;
#pragma pack()

#pragma pack(1)
	typedef struct _STRUCT_BITMAPS
	{
		HBITMAP		hBitmap;		// Handle to bitmap
		int			x;
		int			y;
		LONG		bmWidth;		// Width of bitmap
		LONG		bmHeight;		// Height of bitmap
		HBITMAP		hMask;			// Handle to mask bitmap
		COLORREF	crTransparent;	// Transparent color

		_STRUCT_BITMAPS()
		{
			hBitmap	= hMask				= NULL;
			x = y = bmWidth = bmHeight	= 0;
			crTransparent				= -1;
		}
	} STRUCT_BITMAPS;
#pragma pack()
//=============================================================================
// Operations
//=============================================================================
public:
	static HBITMAP CreateBitmapMask(HBITMAP hSourceBitmap, COLORREF crTransColor);

	void SetIcon(BUTTON_STATUS it, UINT nIDResource, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	void SetIcon(BUTTON_STATUS it, HICON hIcon, BOOL bRepaint = TRUE);
	void SetIconAlignment(ICON_ALIGNMENT ia, BOOL bRepaint = TRUE);
	void ShowBorder(BOOL isDrawBorder = TRUE, BOOL bRepaint = TRUE);
	void ShowFocus(BOOL isDrawFocus = TRUE, BOOL bRepaint = TRUE);
	void DrawTransparent(BOOL isDrawTransparent = TRUE, BOOL bRepaint = TRUE);
	void SetBk(CDC* pDC);
	BOOL SetBtnCursor(UINT nIDResource);
	void SetPressOffset(CPoint ptOffset);
	BOOL SetMenu(UINT nMenu, HWND hParentWnd, BOOL bRepaint);
	void SetMenuCallback(HWND hWnd, UINT nMessage, LPARAM lParam);
	BOOL SetBMP(BUTTON_STATUS it, UINT nIDResource, int x = 0, int y = 0, COLORREF crTransColor = -1, BOOL bRepaint = TRUE);
	BOOL SetBMP(BUTTON_STATUS it, HBITMAP hBitmap, int x = 0, int y = 0, COLORREF crTransColor = -1, BOOL bRepaint = TRUE);
	BOOL SetBMP(BUTTON_STATUS it, HBITMAP hBitmap, int x = 0, int y = 0, HBITMAP hMask = NULL, BOOL bRepaint = TRUE);
 	void SetTextColor(COLORREF clrText, BOOL bRepaint = TRUE);
	COLORREF GetTextColor(){return m_clrText;}
	void EnableBalloonTooltip();
	void SetTooltipText(int nText, BOOL bActivate = TRUE);
	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate = TRUE);
	void ActivateTooltip(BOOL bActivate = TRUE);
	void SetBackgroundColor(COLORREF clrBackground, BOOL bRepaint = TRUE);
	void SetFat(BOOL bIsFat, BOOL bRepaint = TRUE);
	void SetBorderColor(COLORREF clrBorderNormal, COLORREF clrBorderHover, COLORREF clrBorderPressed, BOOL bRepaint = TRUE);
	void SetCheck(BOOL bIsChecked, BOOL bRepaint = TRUE);
	BOOL IsChecked() { return m_bIsChecked; }
	void EnablePress(BOOL bEnable = TRUE, BOOL bRepaint = FALSE);
	BOOL IsPressEnabled(){ return m_bEnablePress; }

	float GetVersion(){return 1.0;}
	CString GetVersionC(){return _T("1.0");}
private:
	CSize GetIconSize(HICON hIcon);
	void PrepareImageRect(CRect& rectItem,CRect& rectText,BOOL bHasText,BOOL bIsPressed,DWORD dwWidth,DWORD dwHeight,CRect& rectImage);
	void DrawIcon(CDC *pDC,CRect& rectItem,CRect& rectText,BOOL bHasText,BOOL bIsPressed,BOOL bIsDisabled);
	void DrawBitmap(CDC* pDC, CRect& rectItem,CRect& rectText, BOOL bIsPressed, BOOL bIsDisabled);
	void DrawText(CDC *pDC,LPCTSTR lpszText,CRect& rectText, BOOL bIsPressed,BOOL bIsDisabled);
	void DrawBorder(CDC* pDC, CRect& rectItem, BOOL bIsPressed, BOOL bIsFocused, BOOL bIsDisabled);
	void DrawFocus(CDC* pDC, CRect rectItem, BOOL bIsFocused);
	void PaintBk(CDC* pDC);
	void InitToolTip();
	LRESULT OnSetStyle(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetCheck(WPARAM wParam, LPARAM lParam);

protected:
	HICON			m_hIcon;
	HICON			m_hGrayIcon;
	HICON			m_hIconHover;
	HICON			m_hIconPressed;
	STRUCT_BITMAPS	m_csBitmaps[6]; //0: Normal, 1:Hover, 2:Pressed, 3:Checked_Normal, 4:Checked_Hover, 5:Checked_Pressed
	HCURSOR			m_hCursor;
	HMENU			m_hMenu;
	HWND			m_hParentWndMenu;
	ICON_ALIGNMENT	m_eIconAlignment;
	BOOL			m_bIsCheckBox;
	BOOL			m_bIsChecked;
	BOOL			m_bIsFat;
	BOOL			m_bIsDrawBorder;
	BOOL			m_bIsDrawFocus;
	BOOL			m_bIsDrawTransparent;
	BOOL			m_bMenuDisplayed;
	CDC				m_dcMemBk;
	CBitmap			m_bmpBk;
	CBitmap*		m_pbmpOldBk;
	CPoint			m_ptPressOffset;
	STRUCT_CALLBACK	m_csMenuCallback;
	COLORREF		m_clrText;
	CToolTipCtrl	m_ctrlToolTip;
	DWORD			m_dwToolTipStyle;
	CString			m_strTipText;
	BOOL			m_bActiveTips;
	COLORREF		m_clrBackground;
	COLORREF		m_clrBorderNormal;
	COLORREF		m_clrBorderHover;
	COLORREF		m_clrBorderPressed;
	BOOL			m_bEnablePress;

	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnBnClicked();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

};


