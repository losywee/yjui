#pragma once

#include "Shared/Preferences.h"
// CYJOptionPageBase �Ի���

class AFX_EXT_CLASS CYJOptionPageBase : public CDialog
{
	DECLARE_DYNAMIC(CYJOptionPageBase)

public:
	CYJOptionPageBase(UINT nIDResourse, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CYJOptionPageBase();

	virtual void LoadPreferences(const CPreferences& /*prefs*/) {}
	virtual void SavePreferences(CPreferences& /*prefs*/) {}
};
