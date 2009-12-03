#include "StdAfx.h"
#include ".\yjdate.h"

CYJDate::CYJDate(void)
{
	CTime time		= CTime::GetCurrentTime();
	m_nYear			= time.GetYear();
	m_nMonth		= time.GetMonth();
	m_nDay			= time.GetDay();
	m_nDayOfWeek	= time.GetDayOfWeek() + 1;
}

CYJDate::CYJDate(int nDay, int nMonth, int nYear):
	m_nDay(nDay),
	m_nMonth(nMonth),
	m_nYear(nYear)
{
	m_nDayOfWeek = CalculateDayOfWeek(nDay, nMonth, nYear);
}

CYJDate::~CYJDate(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////////
void CYJDate::AddMonth(int nMonth)
{
	if ( nMonth > 0)
	{
		m_nMonth += nMonth;
		m_nMonth = m_nMonth % 12;
	}
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////
void CYJDate::SetDate(int nDay, int nMonth, int nYear)
{
	m_nDay		= nDay;
	m_nMonth	= nMonth;
	m_nYear		= nYear;
	m_nDayOfWeek= CalculateDayOfWeek(nDay, nMonth, nYear);
}
bool CYJDate::IsLeapYear()
{
	return IsLeapYear(m_nYear);
}

int CYJDate::GetNumberOfDays()
{
	return GetNumberOfDays(m_nMonth, m_nYear);
}

CYJDate& CYJDate::AddDate(int nDay, int nMonth /* = 0 */, int nYear /* = 0 */)
{
	m_nYear += nYear;
	AddMonth(nMonth);
}

//////////////////////////////////////////////////////////////////////////
// Static Methods
//////////////////////////////////////////////////////////////////////////
int CYJDate::CalculateDayOfWeek(int day, int month, int year)
{
	// Calculate params
	int a = (int)((14-month) / 12);
	int y = year - a;
	int m = month + 12*a - 2;

	// Calculate day of the week
	return (day + y + y/4 - y/100 + y/400 + (31*m)/12) % 7;
}

bool CYJDate::IsLeapYear(int nYear)
{
	if ( ( nYear % 400 == 0 ) || ( ( nYear % 4 == 0 ) && ( nYear % 100 != 0 ) ) )
		return true;
	return false;
}

int CYJDate::GetNumberOfDays(int nMonth, int nYear)
{
	int result;

	// Calculate number of days in the month
	if ( nMonth != 2 )
	{
		// In the first half of the year
		if ( nMonth <= 7 )
		{
			if ( nMonth % 2 == 1 )
				result = 31;
			else
				result = 30;
		}
		// In the second half of the year
		else
		{
			if ( nMonth % 2 == 0 )
				result = 31;
			else
				result = 30;
		}
	}
	else
	{
		// If year is leap year
		if ( IsLeapYear(nYear) )
			result = 29;
		else
			result = 28;
	}

	return result;
}