//created by Wang Jianwei on 1 Dec 2012

#include "LogLinearInterpolator.h"
#include <math.h>
#define NaN log(-1.0) 

using namespace utilities;

typedef tuple<date, double> point;

LogLinearInterpolator::LogLinearInterpolator(point startPoint, point endPoint):
AbstractInterpolator(startPoint, endPoint){
	_slope = NaN;
}

point LogLinearInterpolator::interpolate(date date0){
	dateInRangeCheck(date0);
	if (_slope != NaN){
		double yDiff = log(std::get<1>(_endPoint)) - log(std::get<1>(_startPoint));
		double xDiff = std::get<0>(_endPoint).getJudianDayNumber() - std::get<0>(_startPoint).getJudianDayNumber();
		_slope = yDiff / xDiff;
	}
	double yVal = _slope*(date0.getJudianDayNumber() - std::get<0>(_startPoint).getJudianDayNumber())+ log(std::get<1>(_startPoint));
	return point(date0, exp(yVal));
}