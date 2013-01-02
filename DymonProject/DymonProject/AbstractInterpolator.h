//created by Wang Jianwei on 01 Dec 2012
#ifndef ABSTRACTINTERPOLATOR_H
#define ABSTRACTINTERPOLATOR_H
#include <vector>
#include <tuple>
#include "date.h"
#include "Enums.h"
#include <sstream>

using namespace std;

namespace utilities{
	template<typename T> class AbstractInterpolator{

	public:

		typedef tuple<T, double> point;

		AbstractInterpolator(point startPoint, point endPoint){
			_startPoint = startPoint;
			_endPoint = endPoint;
		}

		AbstractInterpolator(point startPoint, point endPoint, enums::interpolAlgo algo){
			_startPoint = startPoint;
			_endPoint = endPoint;
			_algo=algo;
		}

		virtual point interpolate(T xVal){return point(NULL,0);}

		point getStartPoint(){return _startPoint;}

		point getEndPoint(){return _endPoint;}

		T getStartingX(){
			return std::get<0>(_startPoint);
		}

		T getEndingX(){
			return std::get<0>(_endPoint);
		}

		virtual std::string toString();

	protected:

		void xValInRangeCheck(T xVal);

		point _startPoint;

		point _endPoint;

		enums::interpolAlgo _algo;
	};

	template <class T> 
	string AbstractInterpolator<T>::toString(){
		std::stringstream ss (stringstream::in | stringstream::out);
		ss << "line section from {";
		ss << std::get<0>(_startPoint) << "," <<std::get<1>(_startPoint)<<"} to {";
		ss << std::get<0>(_endPoint) << "," <<std::get<1>(_endPoint)<<"}";
		return ss.str();
	}

	template <class T> 
	void AbstractInterpolator<T>::xValInRangeCheck(T xVal){
		if (xVal<AbstractInterpolator<T>::getStartingX()){
			std::stringstream ss (stringstream::in | stringstream::out);
			ss<<"X Value is smaller than the line section starting value";
			throw ss.str();
		}else if (xVal>AbstractInterpolator<T>::getEndingX()){
			std::stringstream ss (stringstream::in | stringstream::out);
			ss<<"X Value is larger than the line section ending value";
			throw ss.str();
		}
	}

	template <> 
	inline string AbstractInterpolator<date>::toString(){
		std::stringstream ss (stringstream::in | stringstream::out);
		ss << "line section from {";
		ss << std::get<0>(_startPoint).toString() << "," <<std::get<1>(_startPoint)<<"} to {";
		ss << std::get<0>(_endPoint).toString() << "," <<std::get<1>(_endPoint)<<"}";
		return ss.str();
	}
}
#endif