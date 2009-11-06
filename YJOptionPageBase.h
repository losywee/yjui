#pragma once

#include "Shared/Preferences.h"
// CYJOptionPageBase 对话框

class AFX_EXT_CLASS CYJOptionPageBase : public CDialog
{
	DECLARE_DYNAMIC(CYJOptionPageBase)

public:
	CYJOptionPageBase(UINT nIDResourse, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CYJOptionPageBase();

	virtual void LoadPreferences(const CPreferences& /*prefs*/) {}
	virtual void SavePreferences(CPreferences& /*prefs*/) {}
};
