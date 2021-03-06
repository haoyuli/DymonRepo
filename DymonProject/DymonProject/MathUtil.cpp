//created by Jianwei 04 Dec 2012
#include "MathUtil.h"
#include <math.h>
#include "Matrix.h"
#include "Constants.h"

using namespace utilities;

double MathUtil::CNF(double x){
	double L, K, w ;

	double const a1 = 0.31938153, a2 = -0.356563782, a3 = 1.781477937, a4 = -1.821255978, a5 = 1.330274429;

	L = fabs(x);
	K = 1.0 / (1.0 + 0.2316419 * L);
	w = 1.0 - 1.0 / sqrt(2 * Pi) * exp(-L *L / 2) * (a1 * K + a2 * pow(K,2) + a3 * pow(K,3) + a4 * pow(K,4) + a5 * pow(K,5));

	if (x < 0 )	w= 1.0 - w;
	return w;
}
		
double MathUtil::invCNF(double p){
	double a1 = -39.69683028665376;
	double a2 = 220.9460984245205;
	double a3 = -275.9285104469687;
	double a4 = 138.3577518672690;
	double a5 =-30.66479806614716;
	double a6 = 2.506628277459239;

	double b1 = -54.47609879822406;
	double b2 = 161.5858368580409;
	double b3 = -155.6989798598866;
	double b4 = 66.80131188771972;
	double b5 = -13.28068155288572;

	double c1 = -0.007784894002430293;
	double c2 = -0.3223964580411365;
	double c3 = -2.400758277161838;
	double c4 = -2.549732539343734;
	double c5 = 4.374664141464968;
	double c6 = 2.938163982698783;

	double d1 = 0.007784695709041462;
	double d2 = 0.3224671290700398;
	double d3 = 2.445134137142996;
	double d4 = 3.754408661907416;

	//Define break-points.

	double p_low =  0.02425;
	double p_high = 1 - p_low;
	double q, x,y, r;
	double sigma = 1.318;
	double my = 9.357;

	if (p<=0 || p>=1)
		throw "Probability input is invalid.";

	//Rational approximation for lower region.

	if (0 < p && p < p_low) {
		q = sqrt(-2*log(p));
		x = (((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) / ((((d1*q+d2)*q+d3)*q+d4)*q+1);
	}

	//Rational approximation for central region.

	if (p_low <= p && p <= p_high) {
		q = p - 0.5;
		r = q*q;
		x = (((((a1*r+a2)*r+a3)*r+a4)*r+a5)*r+a6)*q / (((((b1*r+b2)*r+b3)*r+b4)*r+b5)*r+1);
	}

	//Rational approximation for upper region.

	if (p_high < p && p < 1) {
		q = sqrt(-2*log(1-p));
		x = -(((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) / ((((d1*q+d2)*q+d3)*q+d4)*q+1);
	}
	y = exp(sigma*x+my);
	return y;
}

Matrix MathUtil::leastSquare(Matrix X, Matrix Y){
	Matrix tmp1=X.transpose()*Y;
	Matrix tmp=(X.transpose()*X).inverse();
	Matrix betas=tmp*tmp1;
	return betas;
}

Matrix MathUtil::restrictedLeastSquare(Matrix X, Matrix Y, Matrix R){
	Matrix beta = leastSquare(X, Y);
	Matrix xTxInv = (X.transpose()*X).inverse();
	Matrix r(1,R.getNumOfRows());
	r.setMatrixElement(1,1,R.getNumOfRows());
	return beta + ((xTxInv*R.inverse())*((R*xTxInv*R.transpose()).transpose()))*(r-R*beta);
}