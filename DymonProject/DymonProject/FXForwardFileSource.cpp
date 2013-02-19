#include "FXForwardFileSource.h"
#include "AbstractFileSource.h"
#include "fileUtil.h"
#include "dateUtil.h"
#include "date.h"
#include "RecordHelper.h"
#include "Enums.h"
#include "EnumHelper.h"
#include "Market.h"
#include <tuple>
#include <regex>
#include "Constants.h"

using namespace DAO;
using namespace std;
using namespace utilities;
using namespace Session;
using namespace instruments;

void FXForwardFileSource::init(Configuration* cfg){
	_fileName = cfg->getProperty("FXForward.file",true,"");
	_persistDir = cfg->getProperty("FXForward.path",false,"");
	_enabled = cfg->getProperty("FXForward.enabled",true,"")=="true"?true:false;
	AbstractFileSource::init(cfg);
}

void FXForwardFileSource::retrieveRecord(){
	if (!_enabled) return;
	
	AbstractFileSource::retrieveRecord();
	CSVDatabase db;
	readCSV(_inFile, db);

	int numOfRows=db.size();
	int numOfCols=db.at(0).size();

	RecordHelper::FXForwardMap* FXForwardMap = RecordHelper::getInstance()->getFXForwardMap();

	for (int i=1;i<numOfRows;i++) {
		FXForward* tempForward = createForwardObject(db, i);
		insertForwardIntoCache(tempForward, FXForwardMap);
	}

	_inFile.close();
}

void FXForwardFileSource::insertForwardIntoCache(FXForward* forward, RecordHelper::FXForwardMap* FXForwardMap){
	string ccyPairStr = forward->getCcyPair().getCcyPairStr();
	string tenorStr = forward->getTenorStr();
	if (FXForwardMap->find(ccyPairStr) == FXForwardMap->end()){
		auto tempMap = std::map<string, FXForward>();
		tempMap.insert(std::make_pair(tenorStr, *forward));
		FXForwardMap->insert(std::make_pair(ccyPairStr, tempMap));
	}else{
		auto tempMap = &(FXForwardMap->find(ccyPairStr)->second);
		tempMap->insert(std::make_pair(tenorStr, *forward));
	}
	cout<<forward->toString()<<endl;
}

FXForward* FXForwardFileSource::createForwardObject(CSVDatabase db, int row){
	int numOfCols=db.at(0).size();
	FXForward* tempForward = new FXForward();
	tempForward->setCcyPair(CcyPair());

	for (int i=0;i<numOfCols;i++) {
		String fieldName = db.at(0).at(i);
		String fieldVal = db.at(row).at(i);
		updateObjectField(fieldName, fieldVal, tempForward);
	}		
	return tempForward;
}

void FXForwardFileSource::updateObjectField(std::string fieldName, std::string fieldVal, FXForward* forward){
	if(fieldName=="ID"){
		forward->setID(fieldVal);
	}else if (fieldName=="SECURITY_TENOR_ONE"){
		forward->setTenorStr(fieldVal);
	}else if (fieldName=="PX_MID"){
		std::regex IDRegex ("[A-Za-z]{3} BGN Curncy");
		if (std::regex_match (fieldVal,IDRegex)){
			forward->setSpot(std::stod(fieldVal));
			forward->setPoint(0);
			forward->setIsSpot(true);
		} else{
			forward->setPoint(std::stod(fieldVal));
			forward->setIsSpot(false);
		}
	}else if (fieldName=="DAY_CNT_DES"){
		enum::DayCountEnum dayCount = EnumHelper::getDayCountEnum(fieldVal);
		forward->setDayCount(dayCount);
	}else if (fieldName=="DAYS_TO_MTY"){
		forward->setDaysToMty(stoi(fieldVal));
	}else if (fieldName=="TRADING_DT_REALTIME"){
		date tradeDate(fieldVal,true);
		forward->setTradeDate(tradeDate);
	}else if (fieldName=="SETTLE_DT"){
		date settleDate(fieldVal,true);
		forward->setDeliveryDate(settleDate);
	}else if (fieldName=="CRNCY"){
		forward->getCcyPair().setCCY1(fieldVal);
	}else if (fieldName=="BASE_CRNCY"){
		forward->getCcyPair().setCCY2(fieldVal);
	}
}
