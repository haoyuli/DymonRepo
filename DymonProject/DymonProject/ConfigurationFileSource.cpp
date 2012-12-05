//created by Wang Jianwei on 1 Dec 2012

#include "ConfigurationFileSource.h"
#include "AbstractFileSource.h"
#include "fileUtil.h"
#include "dateUtil.h"
#include "date.h"
#include "DymonRecordHelper.h"
#include "Configuration.h"

using namespace DAO;
using namespace std;
using namespace Session;
using namespace utilities;

ConfigurationFileSource::ConfigurationFileSource():
	AbstractFileSource(){}

ConfigurationFileSource::ConfigurationFileSource(std::string persistDir, std::string fileName):
	AbstractFileSource(persistDir, fileName){}

ConfigurationFileSource::~ConfigurationFileSource(){}

void ConfigurationFileSource::init(Configuration cfg){
	_fileName = "instance.properties";
	_persistDir = "";
	AbstractFileSource::init(cfg);
}

void ConfigurationFileSource::retrieveRecord(){
	AbstractFileSource::retrieveRecord();
	
	string keyEqualsValue;
	string key;
	string value;
	while (_inFile.good()){
		_inFile>>keyEqualsValue;
		if (keyEqualsValue.find('=')==0)
		{
			cout<<"Property ignored - Equal sign not found in: "<<keyEqualsValue<<endl;
			continue;
		}
		key = keyEqualsValue.substr(0,keyEqualsValue.find('='));
		value = keyEqualsValue.substr(keyEqualsValue.find('=')+1);
		if (value==""||key=="")
		{
			cout<<"Property ignored - Key/value pair not complete: "<<keyEqualsValue<<endl;
			continue;
		}
		Configuration::_config[key]=value;
	}

	for(map<string,string>::iterator i=Configuration::_config.begin();i!=Configuration::_config.end();i++) {
		cout << "Item \"" << i->first << "\" has value \"" << i->second << '\"' << endl ;
	}
	
	_inFile.close();
}
