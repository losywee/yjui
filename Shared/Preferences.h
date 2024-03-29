// Preferences.h: interface for the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREFERENCES_H__DF763543_F9D5_4C94_BBD9_DF7E6E41B8C2__INCLUDED_)
#define AFX_PREFERENCES_H__DF763543_F9D5_4C94_BBD9_DF7E6E41B8C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

struct INIENTRY
{
	INIENTRY(LPCTSTR szName = NULL, LPCTSTR szValue = NULL) : sName(szName), sValue(szValue) {}
	
	CString sName;
	CString sValue;
};

typedef CMap<CString, LPCTSTR, INIENTRY, INIENTRY&> CIniEntryMap;

struct INISECTION
{
	INISECTION(LPCTSTR szName = NULL) : sSection(szName) {}
	
	CString sSection;
	CIniEntryMap aEntries;
};

typedef CArray<INISECTION*, INISECTION*> CIniSectionArray;

class AFX_EXT_CLASS CPreferences
{
public:
	CPreferences(); 
	~CPreferences();

	// directly from CWinApp
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault = 0) const;
	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL) const;
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	// note: Binary not supported by ini file

	// extras
	double GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault = 0) const;
	BOOL WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue);

	int GetArrayItems(LPCTSTR lpszSection, CStringArray& aItems) const;
	void WriteArrayItems(const CStringArray& aItems, LPCTSTR lpszSection);


protected:
	static CIniSectionArray s_aIni;
	static BOOL s_bDirty;
	static BOOL s_bIni;
	static int s_nRef;
	
	static POSITION GetEntry(INISECTION& section, LPCTSTR lpszEntry);
	static INISECTION* GetSection(LPCTSTR lpszSection, BOOL bCreateNotExist);
	
	static CString GetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
	static void SetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR szValue);

	static CString ToString(int nValue);
	static CString ToString(double dValue);
};

#endif // !defined(AFX_PREFERENCES_H__DF763543_F9D5_4C94_BBD9_DF7E6E41B8C2__INCLUDED_)
