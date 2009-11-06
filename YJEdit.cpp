// YJEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "YJEdit.h"
#include <sys/types.h>
#include <sys/stat.h>

// CYJEdit

IMPLEMENT_DYNAMIC(CYJEdit, CEdit)
CYJEdit::CYJEdit()
{
	m_bShowPromtText	= FALSE;
	m_strPromptText		= _T("");
	m_crPromptColor		= RGB(119,121,118);
	m_crBkColor			= GetSysColor(COLOR_WINDOW);
	m_crTextColor		= RGB(0,0,0);
	m_bUseAutoComplete	= FALSE;
	m_nWindowTextLength = 0;
	m_bIsSetPromptText	= FALSE;
}

CYJEdit::~CYJEdit()
{
	m_brush.DeleteObject();
}


BEGIN_MESSAGE_MAP(CYJEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KILLFOCUS()
	//ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(LIST_SELECTED,OnListSelected)
	ON_MESSAGE(LIST_SELECT_CHANGED,OnListSelectChanged)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYUP()
	ON_WM_DROPFILES()
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CYJEdit::PreSubclassWindow()
{
	m_brush.CreateSolidBrush(m_crBkColor);
	m_brushDisabled.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	ShowPromptText();
	CEdit::PreSubclassWindow();
}

LRESULT CYJEdit::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_SETTEXT)
	{
		TCHAR *cp = (TCHAR *) lParam;

		if (!cp)
			return TRUE;

		// the control is being initialized, just ignore
		if (m_bShowPromtText && (cp[0] == _T('\0')))
			return TRUE;

		// if this is not prompt string, then no need to prompt
		if (m_strPromptText.Compare(cp) != 0)
			m_bShowPromtText = FALSE;
	}
	else if (message == WM_GETTEXT)
	{
		if (m_bShowPromtText)
		{
			TCHAR *cp = (TCHAR *) lParam;

			if (cp && (wParam != 0))
				*cp = _T('\0');

			return 0;
		}
	}

	return CEdit::DefWindowProc(message, wParam, lParam);
}

void CYJEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);

	if (m_bShowPromtText)
	{
		SetSel(m_strPromptText.GetLength(), m_strPromptText.GetLength());
	}
}

HBRUSH CYJEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	if (IsWindowEnabled())
	{
		pDC->SetBkColor(m_crBkColor);
		if (m_bShowPromtText)
			pDC->SetTextColor(m_crPromptColor);
		else
			pDC->SetTextColor(m_crTextColor);

		return m_brush;		// setting text color will have no effect unless 
	}
	else
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_brushDisabled;
	}
	// we return a valid brush
}

void CYJEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_bShowPromtText)
	{
		// key down includes keys like shift and ctrl
		m_bShowPromtText = FALSE;
		SetWindowText(_T(""));
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CYJEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString str;
	GetWindowText(str);

	if ((nChar == VK_DOWN || nChar == VK_UP) && m_wndDropList.GetSafeHwnd() && !m_wndDropList.IsWindowVisible())
	{
		m_wndDropList.FindLikeString(str);
		return;
	}

//	if (m_nWindowTextLength != str.GetLength())
//	{
//		m_nWindowTextLength = str.GetLength();
//
//		OnDropDown(str, (nChar != VK_BACK && nChar != VK_DELETE));
//
//	}

	if (nChar == VK_BACK || nChar == VK_DELETE)
	{
		OnDropDown(str,FALSE);
	}

	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CYJEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bShowPromtText)
	{
		m_bShowPromtText = FALSE;
		SetWindowText(_T(""));
	}

	CEdit::OnLButtonDown(nFlags, point);
}

void CYJEdit::OnMButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bShowPromtText)
	{
		m_bShowPromtText = FALSE;
		SetWindowText(_T(""));
	}

	CEdit::OnMButtonDown(nFlags, point);
}

void CYJEdit::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bShowPromtText)
	{
		m_bShowPromtText = FALSE;
		SetWindowText(_T(""));
	}

	CEdit::OnRButtonDown(nFlags, point);
}

void CYJEdit::SetPromptColor(COLORREF crText, BOOL bRepaint/* = TRUE*/)
{
	m_crPromptColor = crText; 
	if (m_bShowPromtText && bRepaint)
		Invalidate(FALSE);
}

void CYJEdit::SetPromptText(LPCTSTR lpszPrompt)
{ 
	m_strPromptText = lpszPrompt;
	CString strWindowText;
	GetWindowText(strWindowText);

	if (m_strPromptText.IsEmpty() || (!strWindowText.IsEmpty() && strWindowText.CompareNoCase(m_strPromptText) != 0))
		m_bShowPromtText = FALSE;
	else
		m_bShowPromtText = TRUE;
	if (m_bShowPromtText)
	{
		ShowPromptText();
	}
}

int CYJEdit::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	if (m_bShowPromtText)
	{
		if (lpszStringBuf && (nMaxCount > 0))
			lpszStringBuf[0] = _T('\0');
		return 0;
	}

	return CEdit::GetWindowText(lpszStringBuf, nMaxCount);
}

void CYJEdit::GetWindowText(CString& rString) const
{
	if (m_bShowPromtText)
		rString = _T("");
	else
		CEdit::GetWindowText(rString);
}

void CYJEdit::SetWindowText(LPCTSTR lpszString)
{
	if (m_bIsSetPromptText)
	{
		m_bIsSetPromptText = FALSE;
		if (m_strPromptText.IsEmpty())
		{
			m_bShowPromtText = FALSE;
			return;
		}
		else
		{
			m_nWindowTextLength = 0;
			m_bShowPromtText = TRUE;
			CEdit::SetWindowText(lpszString);
			return;
		}
	}

// 
// 	if (m_bShowPromtText && (lpszString[0] == _T('\0')))
// 		return;
// 
// 	if (lpszString && (m_strPromptText.Compare(lpszString) != 0))
// 	{
// 		m_bShowPromtText = FALSE;
// 		m_nWindowTextLength = CString(lpszString).GetLength();
// 	}

	m_bShowPromtText = FALSE;
	m_nWindowTextLength = CString(lpszString).GetLength();
	CEdit::SetWindowText(lpszString);
}
void CYJEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	if (m_bUseAutoComplete && m_wndDropList.IsWindowVisible())
		m_wndDropList.ShowWindow(SW_HIDE);

	CString str;
	GetWindowText(str);
	if (!m_bShowPromtText && str.IsEmpty())
	{
		ShowPromptText();
	}
	
}

void CYJEdit::SetBkColor(COLORREF crText, BOOL bRepaint/* = TRUE*/)
{
	m_crBkColor = crText;
	
	if (m_crBkColor)
	{
		m_brush.DeleteObject();
		m_brush.CreateSolidBrush(m_crBkColor);
	}

	if (bRepaint)
		Invalidate();
}

void CYJEdit::SetTextColor(COLORREF crText, BOOL bRepaint /* = TRUE */)
{
	m_crTextColor = crText;
	if (bRepaint)
		Invalidate();
}

void CYJEdit::EnableAutoComplete()
{
	m_bUseAutoComplete = TRUE;
	if (!m_wndDropList.GetSafeHwnd())
	{
		CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS|CS_HREDRAW|CS_VREDRAW,
			0,(HBRUSH) (COLOR_WINDOW), 0);
		CRect rcWnd;
		GetWindowRect(&rcWnd);

		VERIFY(m_wndDropList.CreateEx(WS_EX_TOOLWINDOW,
			szClassName,NULL,
			/*WS_THICKFRAME |*/ WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS /*| WS_OVERLAPPED*/,
			CRect(rcWnd.left, rcWnd.top +20, rcWnd.left+ 200, rcWnd.top+200),
			GetDesktopWindow(),
			NULL, NULL));
	}

	m_wndDropList.Init(this);
}
BOOL CYJEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
		case VK_UP:
		case VK_DOWN:
			if (m_wndDropList.GetSafeHwnd() && m_wndDropList.IsWindowVisible())
			{
				m_wndDropList.PostMessage(WM_KEYDOWN,pMsg->wParam,pMsg->lParam);
				return TRUE;
			}
			break;
		case VK_TAB:
			CString str;
			GetWindowText(str);
			OnDropDown(str);
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CYJEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_wndDropList.GetSafeHwnd())
	{
		int nTopIndex = m_wndDropList.GetTopIndex();
		if (zDelta >= WHEEL_DELTA)
		{
			nTopIndex -= 3;
			nTopIndex = nTopIndex < 0 ? 0:nTopIndex;
			m_wndDropList.SetTopIndex(nTopIndex, TRUE);
		} 
		else
		{
			nTopIndex += 3;
			nTopIndex = nTopIndex > m_wndDropList.GetItemCount() ? m_wndDropList.GetItemCount():nTopIndex;
			m_wndDropList.SetTopIndex(nTopIndex, TRUE);
		}
	}
	
	return CEdit::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT CYJEdit::OnListSelected(WPARAM wParam, LPARAM lParam)
{
	CString str;
	m_wndDropList.GetString(str,(int)lParam);
	SetWindowText(str);
	SetSel(str.GetLength(),-1);

	GetParent()->SendMessage(LIST_SELECTED,wParam,lParam);

	return 1;
}

LRESULT CYJEdit::OnListSelectChanged(WPARAM wParam, LPARAM lParam)
{
	CString str;
	m_wndDropList.GetString(str,(int)lParam);
	SetWindowText(str);
	SetSel(0,-1);

	GetParent()->SendMessage(LIST_SELECT_CHANGED,wParam,lParam);

	return 1;
}

void CYJEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetSel(0,-1);

//	CEdit::OnLButtonDblClk(nFlags, point);
}

void CYJEdit::AddPopupString(LPCTSTR szText, LPCTSTR szNote/* = NULL*/)
{
	if (!m_bUseAutoComplete) return;
	m_wndDropList.AddString(szText,szNote);
}

void CYJEdit::AddPopupStrings(CItemArray& arrStr)
{
	if (!m_bUseAutoComplete) return;
	m_wndDropList.AddStrings(arrStr);
}

void CYJEdit::RemoveAll()
{
	if (!m_bUseAutoComplete) return;
	m_wndDropList.RemoveAll();
}

void CYJEdit::SetPopupTextColor(COLORREF clrText, BOOL bRepaint /* = TRUE */)
{
	if (!m_bUseAutoComplete) return;
	m_wndDropList.SetTextColor(clrText,bRepaint);
}

//////////////////////////////////////////////////////////////////
//	original code was part of CDropEdit by Chris Losinger
//  http://www.codeproject.com/KB/edit/dropedit.aspx
/////////////////////////////////////////////////////////////////
void CYJEdit::OnDropFiles(HDROP hDropInfo)
{
	// Get the number of pathnames that have been dropped
	WORD wNumFilesDropped = DragQueryFile(hDropInfo, -1, NULL, 0);

	CString csFirstFile = _T("");

	// there may be many, but we'll only use the first
	if (wNumFilesDropped > 0)
	{
		// Get the number of bytes required by the file's full pathname
		WORD wPathnameSize = DragQueryFile(hDropInfo, 0, NULL, 0);

		// Allocate memory to contain full pathname & zero byte
		wPathnameSize+=1;

		TCHAR * pFile = new TCHAR[wPathnameSize];
		if (pFile == NULL)
		{
			ASSERT(0);
			DragFinish(hDropInfo);
			return;
		}

		// Copy the pathname into the buffer
		DragQueryFile(hDropInfo, 0, pFile, wPathnameSize);

		csFirstFile = pFile;

		// clean up
		delete [] pFile; 
	}

	// Free the memory block containing the dropped-file information
	DragFinish(hDropInfo);

	// if this was a shortcut, we need to expand it to the target path
	CString csExpandedFile = ExpandShortcut(csFirstFile);

	// if that worked, we should have a real file name
	if (!csExpandedFile.IsEmpty()) 
	{
		csFirstFile = csExpandedFile;
	}

	if (!csFirstFile.IsEmpty())
	{
		struct _stat buf;

		// get some info about that file
		int result = _tstat( csFirstFile, &buf );
		if( result == 0 ) 
		{
			// verify that we have a dir (if we want dirs)
			if ((buf.st_mode & _S_IFDIR) == _S_IFDIR || (buf.st_mode & _S_IFREG) == _S_IFREG) 
			{
				SetWindowText(csFirstFile);
			}
		}
	}

	CEdit::OnDropFiles(hDropInfo);
}


//////////////////////////////////////////////////////////////////
//	use IShellLink to expand the shortcut
//	returns the expanded file, or "" on error
//
//	original code was part of CShortcut 
//	1996 by Rob Warner
//	rhwarner@southeast.net
//	http://users.southeast.net/~rhwarner

CString CYJEdit::ExpandShortcut(CString &inFile)
{
	CString outFile;

	// Make sure we have a path
	ASSERT(inFile != _T(""));

	IShellLink* psl;
	HRESULT hres;
//	LPTSTR lpsz = inFile.GetBuffer(MAX_PATH);

	// Create instance for shell link
	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &psl);

	if (SUCCEEDED(hres))
	{
		// Get a pointer to the persist file interface
		IPersistFile* ppf;

		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
		if (SUCCEEDED(hres))
		{
			// Make sure it's ANSI
			WCHAR wsz[MAX_PATH];
#ifdef _UNICODE
			_tcscpy(wsz, (LPCTSTR)inFile.GetBuffer(inFile.GetLength()));
			
			inFile.ReleaseBuffer();
#else
			::MultiByteToWideChar(CP_ACP, 0, inFile, -1, wsz, MAX_PATH);
#endif

			// Load shortcut
			hres = ppf->Load(wsz, STGM_READ);

			if (SUCCEEDED(hres)) 
			{
				WIN32_FIND_DATA wfd;

				// find the path from that
				HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH), 
					MAX_PATH,
					&wfd, 
					SLGP_UNCPRIORITY);

				outFile.ReleaseBuffer();
			}

			ppf->Release();
		}

		psl->Release();
	}

	inFile.ReleaseBuffer();

	// if this fails, outFile == ""
	return outFile;
}

void CYJEdit::ShowPromptText()
{
	m_bIsSetPromptText = TRUE;
	SetWindowText(m_strPromptText);
	SetSel(m_strPromptText.GetLength(),m_strPromptText.GetLength());
}

void CYJEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nStartChar,nEndChar;
	GetSel(nStartChar,nEndChar);
	CString str;
	GetWindowText(str);
	if (nStartChar != nEndChar && nChar == str.GetAt(nStartChar) && m_wndDropList.IsWindowVisible())
	{
		nStartChar++;
		SetSel(nStartChar,nEndChar);
//		m_nWindowTextLength = nStartChar;
		str = str.Left(nStartChar);
		OnDropDown(str,FALSE);
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	if (nChar != VK_BACK)
	{
		GetWindowText(str);
		OnDropDown(str);
	}
}

void CYJEdit::OnDropDown(CString strWindowText, BOOL bAutoComplete/* = TRUE*/)
{
	if (m_wndDropList.GetSafeHwnd() && m_wndDropList.FindLikeString(strWindowText) > 0 && bAutoComplete)
	{
		CString strFound;
		int i = strWindowText.GetLength();
		m_wndDropList.GetString(strFound,0);
		SetWindowText(strFound);
		SetSel(i,-1);
//		m_nWindowTextLength = i;
	}
}

