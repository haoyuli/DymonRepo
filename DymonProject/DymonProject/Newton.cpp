//created by Wang Jianwei on 1 Dec 2012
//implemented by Hu Kun on 16 Dec 2012

#include "Newton.h"
#include <iostream>

using namespace std;
using namespace utilities;

void nrerror(char error_text[]) {
		cout <<error_text<<endl;
}

float Newton::findroot(void (*funcd)(float,float *, float *), float x1, float x2, float xacc, int iterateCount){

	int j;
	float df,dx,dxold,f,fh,fl;
	float temp,xh,xl,rts;

	(*funcd) (x1,&fl,&df);
	(*funcd) (x2,&fh,&df);

	if((fl>0.0 && fh>0.0) || (fl<0.0&& fh<0.0)) cout<<"Root must be bracketed in rtsafe"<<endl;
	if (fl==0.0) return x1;
	if(fh==0.0) return x2;
	if (fl<0.0) {
		xl=x1;
		xh=x2;
	} else {
		xh=x1;
		xl=x2;
	}

	rts=0.5*(x1+x2);
	dxold=fabs(x2-x1);

	dx=dxold;
	(*funcd)(rts,&f,&df);

	for (j=1;j<=iterateCount;j++) {
		if ((((rts-xh)*df-f)*((rts-xl)*df-f)>0.0||fabs(2.0*f)>fabs(dxold*df))) {
			dxold=dx;
			dx=0.5*(xh-xl);
			rts=xl+dx;
			if (xl==rts) return rts;
		} else {
			dxold=dx;
			dx=f/df;
			temp=rts;
			rts -=dx;
			if(temp==rts) return rts;
		}
		if(fabs(dx)<xacc) return rts;
		(*funcd)(rts,&f,&df);

		if (f<0.0)
			xl=rts;
		else
			xh=rts;

	}

	cout<<"Maximum number of iterations exceeded in Newton-Raphson"<<endl;
	return 0.0; 
}