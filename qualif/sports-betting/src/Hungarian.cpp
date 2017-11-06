///////////////////////////////////////////////////////////////////////////////
// Hungarian.cpp: Implementation file for Class HungarianAlgorithm.
// 
// This is a C++ wrapper with slight modification of a hungarian algorithm implementation by Markus Buehren.
// The original implementation is a few mex-functions for use in MATLAB, found here:
// http://www.mathworks.com/matlabcentral/fileexchange/6543-functions-for-the-rectangular-ass-problem
// 
// Both this code and the orignal code are published under the BSD license.
// by Cong Ma, 2016
// 

#include <stdlib.h>
#include <cfloat> // for DBL_MAX
#include <cmath>  // for fabs()
#include <limits>
#include "Hungarian.h"


HungarianAlgorithm::HungarianAlgorithm(){}
HungarianAlgorithm::~HungarianAlgorithm(){}

using namespace std;

//********************************************************//
// A single function wrapper for solving ass problem.
//********************************************************//
Number HungarianAlgorithm::Solve(vector <vector<Number> >& DistMatrix, vector<int>& Assignment)
{
	unsigned int nRows = DistMatrix.size();
	unsigned int nCols = DistMatrix[0].size();

	Number *dMIn = new Number[nRows * nCols];
	int *ass = new int[nRows];
	Number cost = Number{0};

	// Fill in the dMIn. Mind the index is "i + nRows * j".
	// Here the cost matrix of size MxN is defined as a double precision array of N*M elements. 
	// In the solving functions matrices are seen to be saved MATLAB-internally in row-order.
	// (i.e. the matrix [1 2; 3 4] will be stored as a vector [1 3 2 4], NOT [1 2 3 4]).
	for (unsigned int i = 0; i < nRows; i++)
		for (unsigned int j = 0; j < nCols; j++)
			dMIn[i + nRows * j] = DistMatrix[i][j];
	
	// call solving function
	assoptimal(ass, &cost, dMIn, nRows, nCols);

	Assignment.clear();
	for (unsigned int r = 0; r < nRows; r++)
		Assignment.push_back(ass[r]);

	delete[] dMIn;
	delete[] ass;
	return cost;
}


//********************************************************//
// Solve optimal solution for ass problem using Munkres algorithm, also known as Hungarian Algorithm.
//********************************************************//
void HungarianAlgorithm::assoptimal(int *ass, Number *cost, Number *dMIn, int nRs, int nCs)
{
	Number *dM, *dMTemp, *dMEnd, *columnEnd, value, minValue;
	bool *cCs, *cRs, *sM, *nSM, *pM;
	int nOfElements, mD, row, col;

	/* initialization */
	*cost = Number{0};
	for (row = 0; row<nRs; row++)
		ass[row] = -1;

	/* generate working copy of distance Matrix */
	/* check if all matrix elements are positive */
	nOfElements = nRs * nCs;
	dM = new Number[nOfElements];
	dMEnd = dM + nOfElements;

	for (row = 0; row<nOfElements; row++)
	{
		value = dMIn[row];
		if (value < Number{0})
			cerr << "All matrix elements have to be non-negative." << endl;
		dM[row] = value;
	}


	/* memory allocation */
	cCs = (bool *)calloc(nCs, sizeof(bool));
	cRs = (bool *)calloc(nRs, sizeof(bool));
	sM = (bool *)calloc(nOfElements, sizeof(bool));
	pM = (bool *)calloc(nOfElements, sizeof(bool));
	nSM = (bool *)calloc(nOfElements, sizeof(bool)); /* used in step4 */

	/* preliminary steps */
	if (nRs <= nCs)
	{
		mD = nRs;

		for (row = 0; row<nRs; row++)
		{
			/* find the smallest element in the row */
			dMTemp = dM + row;
			minValue = *dMTemp;
			dMTemp += nRs;
			while (dMTemp < dMEnd)
			{
				value = *dMTemp;
				if (value < minValue)
					minValue = value;
				dMTemp += nRs;
			}

			/* subtract the smallest element from each element of the row */
			dMTemp = dM + row;
			while (dMTemp < dMEnd)
			{
				*dMTemp -= minValue;
				dMTemp += nRs;
			}
		}

		/* Steps 1 and 2a */
		for (row = 0; row<nRs; row++)
			for (col = 0; col<nCs; col++)
				if (fabs(static_cast<double>(dM[row + nRs*col])) < DBL_EPSILON)
					if (!cCs[col])
					{
						sM[row + nRs*col] = true;
						cCs[col] = true;
						break;
					}
	}
	else /* if(nRs > nCs) */
	{
		mD = nCs;

		for (col = 0; col<nCs; col++)
		{
			/* find the smallest element in the column */
			dMTemp = dM + nRs*col;
			columnEnd = dMTemp + nRs;

			minValue = *dMTemp++;
			while (dMTemp < columnEnd)
			{
				value = *dMTemp++;
				if (value < minValue)
					minValue = value;
			}

			/* subtract the smallest element from each element of the column */
			dMTemp = dM + nRs*col;
			while (dMTemp < columnEnd)
				*dMTemp++ -= minValue;
		}

		/* Steps 1 and 2a */
		for (col = 0; col<nCs; col++)
			for (row = 0; row<nRs; row++)
				if (fabs(static_cast<double>(dM[row + nRs*col])) < DBL_EPSILON)
					if (!cRs[row])
					{
						sM[row + nRs*col] = true;
						cCs[col] = true;
						cRs[row] = true;
						break;
					}
		for (row = 0; row<nRs; row++)
			cRs[row] = false;

	}

	/* move to step 2b */
	step2b(ass, dM, sM, nSM, pM, cCs, cRs, nRs, nCs, mD);

	/* compute cost and remove invalid asss */
	computeasscost(ass, cost, dMIn, nRs);

	/* free allocated memory */
	free(dM);
	free(cCs);
	free(cRs);
	free(sM);
	free(pM);
	free(nSM);

	return;
}

/********************************************************/
void HungarianAlgorithm::buildassvector(int *ass, bool *sM, int nRs, int nCs)
{
	int row, col;

	for (row = 0; row<nRs; row++)
		for (col = 0; col<nCs; col++)
			if (sM[row + nRs*col])
			{
				ass[row] = col;
				break;
			}
}

/********************************************************/
void HungarianAlgorithm::computeasscost(int *ass, Number *cost, Number *dM, int nRs)
{
	int row, col;

	for (row = 0; row<nRs; row++)
	{
		col = ass[row];
		if (col >= 0)
			*cost += dM[row + nRs*col];
	}
}

/********************************************************/
void HungarianAlgorithm::step2a(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD)
{
	bool *sMTemp, *columnEnd;
	int col;

	/* cover every column containing a starred zero */
	for (col = 0; col<nCs; col++)
	{
		sMTemp = sM + nRs*col;
		columnEnd = sMTemp + nRs;
		while (sMTemp < columnEnd){
			if (*sMTemp++)
			{
				cCs[col] = true;
				break;
			}
		}
	}

	/* move to step 3 */
	step2b(ass, dM, sM, nSM, pM, cCs, cRs, nRs, nCs, mD);
}

/********************************************************/
void HungarianAlgorithm::step2b(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD)
{
	int col, nCC;

	/* count covered columns */
	nCC = 0;
	for (col = 0; col<nCs; col++)
		if (cCs[col])
			nCC++;

	if (nCC == mD)
	{
		/* algorithm finished */
		buildassvector(ass, sM, nRs, nCs);
	}
	else
	{
		/* move to step 3 */
		step3(ass, dM, sM, nSM, pM, cCs, cRs, nRs, nCs, mD);
	}

}

/********************************************************/
void HungarianAlgorithm::step3(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD)
{
	bool zerosFound;
	int row, col, starCol;

	zerosFound = true;
	while (zerosFound)
	{
		zerosFound = false;
		for (col = 0; col<nCs; col++)
			if (!cCs[col])
				for (row = 0; row<nRs; row++)
					if ((!cRs[row]) && (fabs(static_cast<double>(dM[row + nRs*col])) < DBL_EPSILON))
					{
						/* prime zero */
						pM[row + nRs*col] = true;

						/* find starred zero in current row */
						for (starCol = 0; starCol<nCs; starCol++)
							if (sM[row + nRs*starCol])
								break;

						if (starCol == nCs) /* no starred zero found */
						{
							/* move to step 4 */
							step4(ass, dM, sM, nSM, pM, cCs, cRs, nRs, nCs, mD, row, col);
							return;
						}
						else
						{
							cRs[row] = true;
							cCs[starCol] = false;
							zerosFound = true;
							break;
						}
					}
	}

	/* move to step 5 */
	step5(ass, dM, sM, nSM, pM, cCs, cRs, nRs, nCs, mD);
}

/********************************************************/
void HungarianAlgorithm::step4(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD, int row, int col)
{
	int n, starRow, starCol, primeRow, primeCol;
	int nOfElements = nRs*nCs;

	/* generate temporary copy of sM */
	for (n = 0; n<nOfElements; n++)
		nSM[n] = sM[n];

	/* star current zero */
	nSM[row + nRs*col] = true;

	/* find starred zero in current column */
	starCol = col;
	for (starRow = 0; starRow<nRs; starRow++)
		if (sM[starRow + nRs*starCol])
			break;

	while (starRow<nRs)
	{
		/* unstar the starred zero */
		nSM[starRow + nRs*starCol] = false;

		/* find primed zero in current row */
		primeRow = starRow;
		for (primeCol = 0; primeCol<nCs; primeCol++)
			if (pM[primeRow + nRs*primeCol])
				break;

		/* star the primed zero */
		nSM[primeRow + nRs*primeCol] = true;

		/* find starred zero in current column */
		starCol = primeCol;
		for (starRow = 0; starRow<nRs; starRow++)
			if (sM[starRow + nRs*starCol])
				break;
	}

	/* use temporary copy as new sM */
	/* delete all primes, uncover all rows */
	for (n = 0; n<nOfElements; n++)
	{
		pM[n] = false;
		sM[n] = nSM[n];
	}
	for (n = 0; n<nRs; n++)
		cRs[n] = false;

	/* move to step 2a */
	step2a(ass, dM, sM, nSM, pM, cCs, cRs, nRs, nCs, mD);
}

/********************************************************/
void HungarianAlgorithm::step5(int *ass, Number *dM, bool *sM, bool *nSM, bool *pM, bool *cCs, bool *cRs, int nRs, int nCs, int mD)
{
	Number h, value;
	int row, col;

	/* find smallest uncovered element h */
	h = Number{std::numeric_limits<int>::max()};
	for (row = 0; row<nRs; row++)
		if (!cRs[row])
			for (col = 0; col<nCs; col++)
				if (!cCs[col])
				{
					value = dM[row + nRs*col];
					if (value < h)
						h = value;
				}

	/* add h to each covered row */
	for (row = 0; row<nRs; row++)
		if (cRs[row])
			for (col = 0; col<nCs; col++)
				dM[row + nRs*col] += h;

	/* subtract h from each uncovered column */
	for (col = 0; col<nCs; col++)
		if (!cCs[col])
			for (row = 0; row<nRs; row++)
				dM[row + nRs*col] -= h;

	/* move to step 3 */
	step3(ass, dM, sM, nSM, pM, cCs, cRs, nRs, nCs, mD);
}
