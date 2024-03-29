// Preferences.cpp: implementation of the CPreferences class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Preferences.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniSectionArray CPreferences::s_aIni;
BOOL CPreferences::s_bDirty = FALSE;
BOOL CPreferences::s_bIni = FALSE;
int CPreferences::s_nRef = 0;

CPreferences::CPreferences()
{
	// if no other object is active we need to do the setup
	if (s_nRef == 0)
	{
		// figure out whether we're using an ini file or the registry
		s_bIni = (AfxGetApp()->m_pszRegistryKey == NULL);
		
		if (s_bIni)
		{
			// read the ini file
			CStdioFile file;
			//DWORD dwTick = GetTickCount();
			
			if (file.Open(AfxGetApp()->m_pszProfileName, CFile::modeRead))
			{
				CString sLine;
				INISECTION* pCurSection = NULL;
				
				while (file.ReadString(sLine))
				{
					sLine.TrimLeft();
					sLine.TrimRight();
					
					if (sLine.IsEmpty())
						continue;
					
					// is it a section ?
					else if (sLine[0] == '[')
					{
						CString sSection = sLine.Mid(1, sLine.GetLength() - 2);
						
						pCurSection = GetSection(sSection, TRUE);
						ASSERT (pCurSection != NULL);
					}
					// else an entry
					else if (pCurSection)
					{
						int nEquals = sLine.Find('=');
						
						if (nEquals != -1)
						{
							CString sEntry = sLine.Left(nEquals);
							sEntry.TrimRight();
							
							CString sValue = sLine.Mid(nEquals + 1);
							sValue.TrimLeft();
							
							// remove quotes
							sValue.Replace(_T("\""), _T(""));
							
							if (!sEntry.IsEmpty())
								SetEntryValue(*pCurSection, sEntry, sValue);
						}
					}
				}
			}

			s_bDirty = FALSE;
			
			//TRACE ("CPreferences(Load Ini = %dms)\n", GetTickCount() - dwTick);
		}
	}
	
	s_nRef++; // increment reference count
}

CPreferences::~CPreferences()
{
	s_nRef--; // decrement reference count
	
	// save ini?
	if (s_nRef == 0 && s_bIni)
	{
		if (s_bDirty)
		{
			CStdioFile file;
			//DWORD dwTick = GetTickCount();
			
			if (file.Open(AfxGetApp()->m_pszProfileName, CFile::modeWrite | CFile::modeCreate))
			{
				for (int nSection = 0; nSection < s_aIni.GetSize(); nSection++)
				{
					// write section line
					INISECTION* pSection = s_aIni[nSection];
					
					CString sLine;
					sLine.Format(_T("[%s]\n"), pSection->sSection);
					
					file.WriteString(sLine);
					
					// write entries
					POSITION pos = pSection->aEntries.GetStartPosition();
					
					while (pos)
					{
						CString sDummy;
						INIENTRY ie;
						
						pSection->aEntries.GetNextAssoc(pos, sDummy, ie);
						
						sLine.Format(_T("%s=%s\n"), ie.sName, ie.sValue);
						file.WriteString(sLine);
					}
				}
			}
			
			//TRACE ("CPreferences(Save Ini = %dms)\n", GetTickCount() - dwTick);
		}
		
		// cleanup
		int nSection = s_aIni.GetSize();
		
		while (nSection--)
			delete s_aIni[nSection];
		
		s_aIni.RemoveAll();
	}
}

CString CPreferences::ToString(int nValue)
{
	CString sValue;
	sValue.Format(_T("%ld"), nValue);
	return sValue;
}

CString CPreferences::ToString(double dValue)
{
	CString sValue;
	sValue.Format(_T("%.6f"), dValue);
	return sValue;
}

UINT CPreferences::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault) const
{
	if (s_bIni)
	{
		CString sValue = GetProfileString(lpszSection, lpszEntry);
		
		if (sValue.IsEmpty()) 
			return nDefault;
		else
			return _tstol(sValue);
	}
	else
		return AfxGetApp()->GetProfileInt(lpszSection, lpszEntry, nDefault);
}

BOOL CPreferences::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
	if (s_bIni)
		return WriteProfileString(lpszSection, lpszEntry, ToString(nValue));
	else
		return AfxGetApp()->WriteProfileInt(lpszSection, lpszEntry, nValue);
}

CString CPreferences::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) const
{
	if (s_bIni)
	{
		INISECTION* pSection = GetSection(lpszSection, FALSE);
		
		if (pSection)
			return GetEntryValue(*pSection, lpszEntry, lpszDefault);
		else
			return lpszDefault;
	}
	else
		return AfxGetApp()->GetProfileString(lpszSection, lpszEntry, lpszDefault);
}

BOOL CPreferences::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	if (s_bIni)
	{
		INISECTION* pSection = GetSection(lpszSection, TRUE);
		ASSERT(pSection);
		
		if (pSection)
		{
			SetEntryValue(*pSection, lpszEntry, lpszValue);
			return TRUE;
		}
		
		// else
		return FALSE;
	}
	else
		return AfxGetApp()->WriteProfileString(lpszSection, lpszEntry, lpszValue);
}



double CPreferences::GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dDefault) const
{
	CString sValue = GetProfileString(lpszSection, lpszEntry, ToString(dDefault));
	
	if (sValue.IsEmpty())
		return dDefault;
	else
		return _tstof(sValue);
}

BOOL CPreferences::WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double dValue)
{
	return WriteProfileString(lpszSection, lpszEntry, ToString(dValue));
}

int CPreferences::GetArrayItems(LPCTSTR lpszSection, CStringArray& aItems) const
{
	aItems.RemoveAll();
	int nCount = GetProfileInt(lpszSection, _T("ItemCount"), 0);
	
	// items
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey, sItem;
		sItemKey.Format(_T("Item%d"), nItem);
		sItem = GetProfileString(lpszSection, sItemKey);
		
		if (!sItem.IsEmpty())
			aItems.Add(sItem);
	}
	
	return aItems.GetSize();
}

void CPreferences::WriteArrayItems(const CStringArray& aItems, LPCTSTR lpszSection)
{
	int nCount = aItems.GetSize();
	
	// items
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sItemKey;
		sItemKey.Format(_T("Item%d"), nItem);
		WriteProfileString(lpszSection, sItemKey, aItems[nItem]);
	}
	
	// item count
	WriteProfileInt(lpszSection, _T("ItemCount"), nCount);
}

CString CPreferences::GetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	CString sKey(lpszEntry);
	sKey.MakeUpper();
	
	INIENTRY ie;
	
	if (section.aEntries.Lookup(sKey, ie) && !ie.sValue.IsEmpty())
		return ie.sValue;
	
	return lpszDefault;
}

void CPreferences::SetEntryValue(INISECTION& section, LPCTSTR lpszEntry, LPCTSTR szValue)
{
	INIENTRY entry;
	CString sKey(lpszEntry);
	sKey.MakeUpper();

	if (!section.aEntries.Lookup(sKey, entry) || entry.sValue != szValue)
	{
		section.aEntries[sKey] = INIENTRY(lpszEntry, szValue);
		s_bDirty = TRUE;
	}
}

INISECTION* CPreferences::GetSection(LPCTSTR lpszSection, BOOL bCreateNotExist)
{
	int nSection = s_aIni.GetSize();
	
	while (nSection--)
	{
		if (s_aIni[nSection]->sSection.CompareNoCase(lpszSection) == 0)
			return s_aIni[nSection];
	}
	
	// add a new section
	if (bCreateNotExist)
	{
		INISECTION* pSection = new INISECTION(lpszSection);
		
		s_aIni.Add(pSection);
		return pSection;
	}
	
	// else
	return NULL;
}
