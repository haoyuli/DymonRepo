//created by Hu Kun 30 Nov 2012
#include "dateUtil.h"
#include "date.h"
#include <math.h>
#include <iostream>
#include "RecordHelper.h"
#include <ctime>

using namespace utilities;
using namespace Session;
using namespace std;

long dateUtil::getJudianDayNumber(unsigned short year, unsigned short month, unsigned short day){
	int	_a=(14-month)/12;
	int _y=year+4800-_a;
	int _m=month+12*_a-3;
	//if (_calendarType==Gregorian){
	return day+(int)((153*_m+2)/5)+365*_y+(int)(_y/4)-(int)(_y/100)+(int)(_y/400)-32045;
	/*}else if (_calendarType==Judian){
		_judianDayNumber=_day+(int)((153*_m+2)/5)+365*_y+(int)(_y/4)-32083;
	}*/
}

long dateUtil::getDaysBetween(date startDate, date endDate){
	return getDaysBetween
		(endDate.getJudianDayNumber(),startDate.getJudianDayNumber());
}

long dateUtil::getDaysBetween(long startJDN, long endJDN){
	return startJDN-endJDN;
}

signed long dateUtil::getBizDaysBetween(date startDate, date endDate){
	return getBizDaysBetween
		(startDate.getJudianDayNumber(),endDate.getJudianDayNumber());
}

signed long dateUtil::getBizDaysBetween(long startJDN, long endJDN){
	
	long numBizDay=0;
	for (long i = 0; i<abs(endJDN-startJDN); i++){
		if (isBizDay(i)) numBizDay++;
	}
	if (startJDN>endJDN) {
		return -numBizDay;
	}
	else {
		return numBizDay;
	}
}

bool dateUtil::isBizDay(date date0){
	return isBizDay(date0.getJudianDayNumber());
}

bool dateUtil::isBizDay(long JDN){
	int dayOfWeek = 1 + (int)fmod(JDN+1,7.0);
	if (dayOfWeek==1||dayOfWeek==7){
		return false;
	}
	return true;
}

bool dateUtil::isHoliday(long JDN, enums::CurrencyEnum market){
	RecordHelper::HolidayMap holidayMap= RecordHelper::getInstance()->getHolidayMap();
	if (holidayMap.find(market)== holidayMap.end()){
		cout<< "Market not found in Holiday Map: "+market<<endl;
		return false;
	}

	set<long> holidaySet = holidayMap[market];
	if (holidaySet.find(JDN) != holidaySet.end())
		return true;
	return false;
}

bool dateUtil::isHoliday(date aDate, enums::CurrencyEnum market){
	return isHoliday(aDate.getJudianDayNumber(),market);
}

date dateUtil::getToday(){
	date today(getTodayYear(),getTodayMonth(),getTodayDay());
	return today;
}

int dateUtil::getTodayDay() {
	time_t     rawtime;
	struct tm* timeinfo;

	time( &rawtime );
	timeinfo = localtime( &rawtime );
	return timeinfo->tm_mday;		
}

int dateUtil::getTodayMonth() {
	time_t     rawtime;
	struct tm* timeinfo;

	time( &rawtime );
	timeinfo = localtime( &rawtime );
	return 1+timeinfo->tm_mon;	
}
	
int dateUtil::getTodayYear() {
	time_t     rawtime;
	struct tm* timeinfo;

	time( &rawtime );
	timeinfo = localtime( &rawtime );
	return 1900+timeinfo->tm_year;
}

unsigned short* dateUtil::getYearMonthDay(long JDN){
	long _year, _month, _day;
	JDN = JDN - 1721119 ;
	_year = (4 * JDN - 1) / 146097 ; 
	JDN = 4 * JDN - 1 - 146097 * _year ; 
	_day = JDN / 4 ;
	JDN = (4 * _day + 3) / 1461 ; 
	_day = 4 * _day + 3 - 1461 * JDN ; 
	_day = (_day + 4) / 4 ;
	_month = (5 * _day - 3) / 153 ; 
	_day = 5 * _day - 3 - 153 * _month ; 
	_day = (_day + 5) / 5 ;
	_year = 100 * _year + JDN ;
	if (_month < 10){
		_month = _month + 3;
	}
	else{
		_month = _month - 9 ; 
		_year = _year + 1;
	}
	unsigned short* returnArray = new unsigned short[3]();
	returnArray[0]=(unsigned short) _year;
	returnArray[1]=(unsigned short) _month;
	returnArray[2]=(unsigned short) _day;
	return returnArray;
}

double dateUtil::getAccrualFactor(date startDate,date endDate, enums::DayCountEnum dayCount){
	double accrualFactor;
	int numBizDay=0;
	switch(dayCount){
	case enums::thirty_360US:
		//This day count convention is also called Bond basis.
		if (endDate.getDay()==31 && (startDate.getDay()==30||31))
			endDate.setDay(30);
		if (startDate.getDay()==31)
			startDate.setDay(30);
		accrualFactor = thirty_360(startDate, endDate);
		break;
	case enums::thirthE_360:
		if (startDate.getDay()==31)
			startDate.setDay(30);
		if (endDate.getDay()==31)
			endDate.setDay(30);
		accrualFactor = thirty_360(startDate, endDate);
		break;
	case enums::ACT_360:
		//This day count is also called Money Market basis or Actual 360
		//This is the most used day count convention for money market instruments (maturity below one year).
		accrualFactor = (endDate.getJudianDayNumber()-startDate.getJudianDayNumber())/360.0;
		//cout<<"inside ACT_360"<<endl;
		break;
	case enums::ACT_365:
		//Also called English Money Market basis.
		//The number 365 is used even in a leap year.
		accrualFactor = (endDate.getJudianDayNumber()-startDate.getJudianDayNumber())/365.0;
		//cout<<"inside ACT_365"<<endl;
		break;
	case enums::ACT_ACT:
		//To compute the number of days, the period first day is included and the last day is excluded.
		if (startDate.getYear()==endDate.getYear()){
			int numDays = endDate.getJudianDayNumber()-startDate.getJudianDayNumber();
			accrualFactor = numDays/(isleapyear(startDate.getYear())?366.0:365.0);
		}else{
			int numStartYearDays = date(startDate.getYear(),12,31).getJudianDayNumber() - startDate.getJudianDayNumber() + 1;
			double startYearFactor = numStartYearDays/(isleapyear(startDate.getYear())?366.0:365.0);
			int numEndYearDays = endDate.getJudianDayNumber() - date(endDate.getYear(),1,1).getJudianDayNumber();
			double endYearFactor = numEndYearDays/(isleapyear(endDate.getYear())?366.0:365.0);
			accrualFactor = startYearFactor + endYearFactor +(endDate.getYear()-startDate.getYear()-1);
		}
		break;
	case enums::BUS_252:
		//Numerator is the number of business days (in a given calendar) from and including the start date up to and excluding the end date.
		numBizDay=0;
		for(long i = startDate.getJudianDayNumber();i<endDate.getJudianDayNumber();i++)
			if (isBizDay(i))
				numBizDay++;
		accrualFactor = numBizDay/252.0;
		break;
	default:
		accrualFactor = (endDate.getJudianDayNumber()-startDate.getJudianDayNumber())/360.0;
	
	}
	return accrualFactor;
}

date dateUtil::dayRollAdjust(date aDate,DayRollEnum aDayRollConvention, enums::CurrencyEnum market) {
	long adjustedJDN;
	switch(aDayRollConvention){
	case enums::Following:
		adjustedJDN = getFolloingJDN(aDate, market);
		break;
	case enums::Preceding:
		adjustedJDN = getPrecedingJDN(aDate, market);
		break;
	case enums::Mfollowing:
		adjustedJDN = getFolloingJDN(aDate, market);
		if (getYearMonthDay(adjustedJDN)[1]!=aDate.getMonth())
			adjustedJDN = getPrecedingJDN(aDate, market);
		break;
	case enums::Mfollowingbi:	
		adjustedJDN = getFolloingJDN(aDate, market);
		if (getYearMonthDay(adjustedJDN)[1]!=aDate.getMonth()||
			getYearMonthDay(adjustedJDN)[2]>=15)
			adjustedJDN = getPrecedingJDN(aDate, market);
		break;
	case enums::EOM:
		break;
	case enums::Null:
		adjustedJDN = aDate.getJudianDayNumber();
		break;
	}
	date adjustedDate(adjustedJDN);
	return adjustedDate;
}

date dateUtil::getEndDateMonthIncrement(date startDate, int numMonth, enums::CurrencyEnum market){
	short startMonth = startDate.getMonth();
	short endMonth;

	//modified to cater for negative month offset cases -Kun 15 Dec

	short endYear;
	if ((startMonth+numMonth)<0) {
		int interim=(int) ceil(-(startMonth + numMonth)/12.0)*12;
		endMonth= (startMonth + numMonth+interim)%12;
		endYear= (short)( startDate.getYear()-ceil((double)-(startMonth + numMonth)/12));
	} else {
		endMonth = (startMonth + numMonth)%12;
		endYear= startDate.getYear()+(startMonth + numMonth)/12 - (endMonth==0?1:0);
	}
	date endDate(endYear, endMonth==0?12:endMonth, startDate.getDay());		
	return endDate;
}

date dateUtil::getEndDate(date startDate, int increment, enums::DayRollEnum dayRoll, enums::CurrencyEnum market, DateUnit dateUnit){
	date endDate;
	switch(dateUnit){
	case YEAR:
		endDate = date(startDate.getYear()+increment,startDate.getMonth(), startDate.getDay());
		break;
	case MONTH:
		endDate = getEndDateMonthIncrement(startDate, increment, market);
		break;
	case WEEK:
		endDate = date(startDate.getJudianDayNumber()+increment*7);
		break;
	case DAY:
		endDate = date(startDate.getJudianDayNumber()+increment);
		break;
	case BIZDAY:
		endDate = getBizDateOffSet(startDate,increment, market);
		break;
	}
	endDate = dayRollAdjust(endDate,dayRoll, market);
	return endDate;
}

date dateUtil::getBizDateOffSet(date refDate, long offset, enums::CurrencyEnum market) {
	long JDN = refDate.getJudianDayNumber();
	bool forward = offset>=0?true:false;
	for (long i=0; i<abs(offset); i++){
		forward?JDN++:JDN--;
		while(!isBizDay(JDN) || isHoliday(JDN,market)){
			forward?JDN++:JDN--;
		}
	}
	date offsetDate(JDN);
	return offsetDate; 
}

date dateUtil::adjustInvalidateDate(date aDate, bool forwardAdjust){
	unsigned short monthlen[]={31,28,31,30,31,30,31,31,30,31,30,31};
	if (isleapyear(aDate.getYear()) && aDate.getMonth()==2)
		monthlen[1]++;
	if (aDate.getDay()>monthlen[aDate.getMonth()-1]){
		if (forwardAdjust){
			aDate.setMonth(aDate.getMonth()%12+1);
			aDate.setDay(1);
		}
		else
			aDate.setDay(monthlen[aDate.getMonth()-1]);
	}
	return aDate;
}

bool dateUtil::isleapyear(unsigned short year){
	return (!(year%4) && (year%100) || !(year%400));
}

long dateUtil::getPrecedingJDN(date refDate, enums::CurrencyEnum market){
	date adjustedDate = adjustInvalidateDate(refDate,false);
	long JDN = adjustedDate.getJudianDayNumber();
	while(!isBizDay(JDN) || isHoliday(JDN,market)){
		JDN--;
	}
	return JDN;
}

long dateUtil::getFolloingJDN(date refDate, enums::CurrencyEnum market){
	date adjustedDate = adjustInvalidateDate(refDate,true);
	long JDN = adjustedDate.getJudianDayNumber();
	while(!isBizDay(JDN) || isHoliday(JDN,market)){
		JDN++;
	}
	return JDN;
}

double dateUtil::thirty_360(date startDate, date endDate){
	int yearFactor = 360*(endDate.getYear()-startDate.getYear());
	int monthFactor = 30*(endDate.getMonth()-startDate.getMonth());
	int dayFactor = endDate.getDay()-startDate.getDay();
	return (yearFactor+monthFactor+dayFactor)/360.0;
}

dateUtil::DateUnit dateUtil::getDateUnit(char letterDateUnit){
	switch(letterDateUnit){
	case 'D':
		return DAY;
	case 'M':
		return MONTH;
	case 'W':
		return WEEK;
	case 'Y':
		return YEAR;
	}
	throw "DateUnit not found: "+letterDateUnit;
}
