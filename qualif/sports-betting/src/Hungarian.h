///////////////////////////////////////////////////////////////////////////////
// Hungarian.h: Header file for Class HungarianAlgorithm.
// 
// This is a C++ wrapper with slight modification of a hungarian algorithm implementation by Markus Buehren.
// The original implementation is a few mex-functions for use in MATLAB, found here:
// http://www.mathworks.com/matlabcentral/fileexchange/6543-functions-for-the-rectangular-ass-problem
// 
// Both this code and the orignal code are published under the BSD license.
// by Cong Ma, 2016
// 

#ifndef HUNGARIAN_H
#define HUNGARIAN_H

#include <iostream>
#include <vector>
#include "Number.hpp"


class HungarianAlgorithm
{
public:
	HungarianAlgorithm();
	~HungarianAlgorithm();
	Number Solve(std::vector<std::vector<Number> >& DistMatrix, std::vector<int>& Assignment);

private:
	void assoptimal(int *ass, Number *cost, Number *dM, int nRs, int nCs);
	void buildassvector(int *ass, bool *sM, int nRs, int nCs);
	void computeasscost(int *ass, Number *cost, Number *dM, int nRs);
	void step2a(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD);
	void step2b(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD);
	void step3(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD);
	void step4(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD, int row, int col);
	void step5(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD);
};


#endif
