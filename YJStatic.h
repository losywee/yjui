#pragma once


// CYJStatic

class AFX_EXT_CLASS CYJStatic : public CStatic
{
	DECLARE_DYNAMIC(CYJStatic)

private:
	CBrush			m_brush;
	COLORREF		m_crTextColor;
	COLORREF		m_crBkColor;
public:
	CYJStatic();
	virtual ~CYJStatic();

	void SetTextColor(COLORREF crTextColor, BOOL bRepaint = FALSE);
	COLORREF GetTextColor() { return m_crTextColor; }
	void SetBkColor(COLORREF crText, BOOL bRepaint = FALSE);
	COLORREF GetBkColor() { return m_crBkColor; }

protected:
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

};


