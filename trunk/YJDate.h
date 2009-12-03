#pragma once

class AFX_EXT_CLASS CYJDate
{
public:
	CYJDate(void);
	CYJDate(int nYear, int nMonth, int nDay);
	~CYJDate(void);

	//Public Methods
	void		SetDate(int nDay, int nMonth, int nYear);
	bool		IsLeapYear();
	int			GetNumberOfDays();
	CYJDate&	AddDate(int nDay, int nMonth = 0, int nYear = 0);

	//Public static Methods
	static int	CalculateDayOfWeek(int day, int month, int year);
	static bool	IsLeapYear(int nYear);
	static int	GetNumberOfDays(int nMonth, int nYear);

private:
	//Attributes
	int m_nYear;
	int m_nMonth;
	int m_nDay;
	int m_nDayOfWeek;

	//Private Methods
	void	AddMonth(int nMonth);
};
