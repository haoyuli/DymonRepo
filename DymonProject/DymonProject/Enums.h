//created by Wang Jianwei on 30 Nov 2012

#ifndef _Market_H
#define _Market_H

namespace enums {
		
	enum CurrencyEnum{EUR,USD,THB,KRW,SGD,JPY};
	enum DayRollEnum {Null,Following,Preceding,Mfollowing,Mfollowingbi,EOM};
	enum DayCountEnum {DayCountNull, thirty_360US, thirthE_360,ACT_360,ACT_365,ACT_ACT,BUS_252};
	enum NumericAlgo {BISECTION, RIDDER, SECANT, FALSEPOSITION};
	enum interpolAlgo{LINEAR,LOGLINEAR,CUBIC,QUADRATIC};
	enum VolType{Call, Put, RR, STR, ATM};
	enum PayReceive {Payer, Receiver};
	enum Instrument{SWAP,OPTION,BOND,FXOPTION};
}

#endif