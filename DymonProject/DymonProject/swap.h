//created by Hu Kun on 30 Nov 2012
//class for standard IRS with fix to floating legs in one currency.
//rates for fixed and floating in private data members are all in terms of annual rates
//rewrote again with cashflow constructs by Kun 16 Dec 2012

#ifndef SWAP_H
#define SWAP_H
#include "currency.h"
#include "date.h"
#include "AbstractInstrument.h"
#include <vector>
#include "zero.h"
#include <tuple>
#include <iterator>
#include "cashflow.h"
#include "cashflowLeg.h"
#include "BuilderCashFlowLeg.h"
#include "RecordHelper.h"
#include "SwapPricer.h"
#include "AbstractPricer.h"

using namespace utilities;
using namespace std;
using namespace enums;
using namespace instruments;


namespace instruments {
	class swap:  public SwapPricer, public AbstractInstrument{
	public:
		swap() {};
		swap(date tradeDate, date maturityDate, double notional, double couponRate, vector<double> FLiborRate, currency fixLegCurr, currency floatingLegCurr, int paymentFreqFixLeg, int paymentFreqFloatingLeg, bool rollAccuralDates, RecordHelper::HolidayMap holidayMap);

		~swap();

		cashflowLeg getCashflowLegFix();
		cashflowLeg getCashflowLegFloat();
		void printCashflowLegFix();
		void printCashflowLegFloat();

	protected:


	private:

		cashflowLeg _fixCashflowLeg;
		cashflowLeg _floatingCashflowLeg;
		date _tradeDate;
		date _maturityDate;
	
	};

}
#endif