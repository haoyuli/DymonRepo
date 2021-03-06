//created by Wang Jianwei on 1 Dec 2012

#include "DepositFileSource.h"
#include "AbstractFileSource.h"
#include "fileUtil.h"
#include "dateUtil.h"
#include "date.h"
#include "RecordHelper.h"
#include "Market.h"
#include "EnumHelper.h"

using namespace DAO;
using namespace std;
using namespace utilities;
using namespace Session;
using namespace instruments;

DepositFileSource::DepositFileSource():
AbstractFileSource(){}

DepositFileSource::DepositFileSource(std::string persistDir, std::string fileName):
AbstractFileSource(persistDir, fileName){}

DepositFileSource::~DepositFileSource(){}

void DepositFileSource::init(Configuration* cfg){
	_fileName = cfg->getProperty("depositRate.file",true,"");
	_persistDir = cfg->getProperty("depositRate.path",false,"");
	_enabled = cfg->getProperty("depositRate.enabled",true,"")=="true"?true:false;
	AbstractFileSource::init(cfg);
}

void DepositFileSource::retrieveRecord(){
	if (!_enabled) return;	
	AbstractFileSource::retrieveRecord();

	CSVDatabase db;
	readCSV(_inFile, db);

	int numOfRows=db.size();
	int numOfCols=db.at(0).size();

	RecordHelper::DepositRateMap* depositRateMap = RecordHelper::getInstance()->getDepositRateMap();

	for (int i=1;i<numOfRows;i++) {
		Deposit* tempDeposit = createDepositObject(db, i);
		insertDepositIntoCache(tempDeposit, depositRateMap);
	}

	_inFile.close();
}

void DepositFileSource::insertDepositIntoCache(Deposit* deposit, RecordHelper::DepositRateMap* depositRateMap){	
	enums::CurrencyEnum market = deposit->getMarket().getCurrencyEnum();
	long accrualEndJDN = deposit->getExpiryDate().getJudianDayNumber();
	if (depositRateMap->find(market) == depositRateMap->end()){
		auto tempMap = map<long, Deposit>();
		tempMap.insert(std::make_pair(accrualEndJDN, *deposit));
		depositRateMap->insert(std::make_pair(market, tempMap));
	}else{
		auto tempMap = &(depositRateMap->find(market)->second);
		tempMap->insert(std::make_pair(accrualEndJDN, *deposit));
	}
	cout<<deposit->toString()<<endl;
}

Deposit* DepositFileSource::createDepositObject(CSVDatabase db, int row){
	int numOfCols=db.at(0).size();
	Deposit* tempDeposit = new Deposit();

	for (int i=0;i<numOfCols;i++) {
		String fieldName = db.at(0).at(i);
		String fieldVal = db.at(row).at(i);
		updateDepositObjectField(fieldName, fieldVal, tempDeposit);
	}		
	return tempDeposit;
}

void DepositFileSource::updateDepositObjectField(std::string fieldName, std::string fieldVal, Deposit* deposit){
	if(fieldName=="ID"){
		deposit->setID(fieldVal);
	}else if (fieldName=="NAME"){
		deposit->setName(fieldVal);
	}else if (fieldName=="SECURITY_TENOR_TWO"){
		deposit->setTenorStr(fieldVal);
	}else if (fieldName=="PX_MID"){
		deposit->setDepositRate(stod(fieldVal));
	}else if (fieldName=="DAY_CNT_DES"){
		enum::DayCountEnum dayCount = EnumHelper::getDayCountEnum(fieldVal);
		deposit->setDayCount(dayCount);
	}else if (fieldName=="DAYS_TO_MTY"){
		deposit->setDaysToMty(stoi(fieldVal));
	}else if (fieldName=="TRADING_DT_REALTIME"){
		date tradeDate(fieldVal,false);
		deposit->setTradeDate(tradeDate);
	}else if (fieldName=="SETTLE_DT"){
		date accrualStartDate(fieldVal,false);
		deposit->setSpotDate(accrualStartDate);
	} else if (fieldName=="COUNTRY"){
		Market market = Market(EnumHelper::getCcyEnum(fieldVal));
		deposit->setMarket(market);
	}
}