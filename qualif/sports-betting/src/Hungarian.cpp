///////////////////////////////////////////////////////////////////////////////
// Hungarian.cpp: Implementation file for Class HungarianAlgorithm.
//
// This is a C++ wrapper with slight modification of a hungarian algorithm implementation by Markus Buehren.
// The original implementation is a few mex-functions for use in MATLAB, found here:
// http://www.mathworks.com/matlabcentral/fileexchange/6543-functions-for-the-rectangular-assignment-problem
//
// Both this code and the orignal code are published under the BSD license.
// by Cong Ma, 2016
//

#include <stdlib.h>
#include <cfloat> // for DBL_MAX
#include <cmath>  // for fabs()
#include <limits>
#include <iostream>
#include "Hungarian.h"


using namespace std;

void f_step2a(int*, Number*, bool*, bool*, bool*, bool*, bool*, int, int, int);
void f_step2b(int*, Number*, bool*, bool*, bool*, bool*, bool*, int, int, int);
void f_step3(int*, Number*, bool*, bool*, bool*, bool*, bool*, int, int, int);
void f_step4(int*, Number*, bool*, bool*, bool*, bool*, bool*, int, int, int, int, int);
void f_step5(int*, Number*, bool*, bool*, bool*, bool*, bool*, int, int, int);
void f_computeassignmentcost(int*, Number*, Number*, int);

//********************************************************//
// Solve optimal solution for assignment problem using Munkres algorithm, also known as Hungarian Algorithm.
//********************************************************//
void f_assignmentoptimal(int *v_assignment, Number *v_cost, Number *v_distMatrixIn, int v_nOfRows, int v_nOfColumns)
{
	Number *v_distMatrix, *v_distMatrixTemp, *v_distMatrixEnd, *v_columnEnd, v_value, v_minValue;
	bool *v_coveredColumns, *v_coveredRows, *v_starMatrix, *v_newStarMatrix, *v_primeMatrix;
	int v_nOfElements, v_minDim, v_row, v_col;

	/* initialization */
	*v_cost = Number{0};
	for (v_row = 0; v_row<v_nOfRows; v_row++)
		v_assignment[v_row] = -1;

	/* generate working copy of distance Matrix */
	/* check if all matrix elements are positive */
	v_nOfElements = v_nOfRows * v_nOfColumns;
	v_distMatrix = new Number[v_nOfElements];
	v_distMatrixEnd = v_distMatrix + v_nOfElements;

	for (v_row = 0; v_row<v_nOfElements; v_row++)
	{
		v_value = v_distMatrixIn[v_row];
		if (v_value < Number{0})
			cerr << "ERROR" << endl;
		v_distMatrix[v_row] = v_value;
	}


	/* memory allocation */
	v_coveredColumns = (bool *)calloc(v_nOfColumns, sizeof(bool));
	v_coveredRows = (bool *)calloc(v_nOfRows, sizeof(bool));
	v_starMatrix = (bool *)calloc(v_nOfElements, sizeof(bool));
	v_primeMatrix = (bool *)calloc(v_nOfElements, sizeof(bool));
	v_newStarMatrix = (bool *)calloc(v_nOfElements, sizeof(bool)); /* used in step4 */

	/* preliminary steps */
	if (v_nOfRows <= v_nOfColumns)
	{
		v_minDim = v_nOfRows;

		for (v_row = 0; v_row<v_nOfRows; v_row++)
		{
			/* find the smallest element in the v_row */
			v_distMatrixTemp = v_distMatrix + v_row;
			v_minValue = *v_distMatrixTemp;
			v_distMatrixTemp += v_nOfRows;
			while (v_distMatrixTemp < v_distMatrixEnd)
			{
				v_value = *v_distMatrixTemp;
				if (v_value < v_minValue)
					v_minValue = v_value;
				v_distMatrixTemp += v_nOfRows;
			}

			/* subtract the smallest element from each element of the v_row */
			v_distMatrixTemp = v_distMatrix + v_row;
			while (v_distMatrixTemp < v_distMatrixEnd)
			{
				*v_distMatrixTemp -= v_minValue;
				v_distMatrixTemp += v_nOfRows;
			}
		}

		/* Steps 1 and 2a */
		for (v_row = 0; v_row<v_nOfRows; v_row++)
			for (v_col = 0; v_col<v_nOfColumns; v_col++)
				if (fabs(boost::rational_cast<double>(v_distMatrix[v_row + v_nOfRows*v_col])) < DBL_EPSILON)
					if (!v_coveredColumns[v_col])
					{
						v_starMatrix[v_row + v_nOfRows*v_col] = true;
						v_coveredColumns[v_col] = true;
						break;
					}
	}
	else /* if(v_nOfRows > v_nOfColumns) */
	{
		v_minDim = v_nOfColumns;

		for (v_col = 0; v_col<v_nOfColumns; v_col++)
		{
			/* find the smallest element in the column */
			v_distMatrixTemp = v_distMatrix + v_nOfRows*v_col;
			v_columnEnd = v_distMatrixTemp + v_nOfRows;

			v_minValue = *v_distMatrixTemp++;
			while (v_distMatrixTemp < v_columnEnd)
			{
				v_value = *v_distMatrixTemp++;
				if (v_value < v_minValue)
					v_minValue = v_value;
			}

			/* subtract the smallest element from each element of the column */
			v_distMatrixTemp = v_distMatrix + v_nOfRows*v_col;
			while (v_distMatrixTemp < v_columnEnd)
				*v_distMatrixTemp++ -= v_minValue;
		}

		/* Steps 1 and 2a */
		for (v_col = 0; v_col<v_nOfColumns; v_col++)
			for (v_row = 0; v_row<v_nOfRows; v_row++)
				if (fabs(boost::rational_cast<double>(v_distMatrix[v_row + v_nOfRows*v_col])) < DBL_EPSILON)
					if (!v_coveredRows[v_row])
					{
						v_starMatrix[v_row + v_nOfRows*v_col] = true;
						v_coveredColumns[v_col] = true;
						v_coveredRows[v_row] = true;
						break;
					}
		for (v_row = 0; v_row<v_nOfRows; v_row++)
			v_coveredRows[v_row] = false;

	}

	/* move to step 2b */
	f_step2b(v_assignment, v_distMatrix, v_starMatrix, v_newStarMatrix, v_primeMatrix, v_coveredColumns, v_coveredRows, v_nOfRows, v_nOfColumns, v_minDim);

	/* compute v_cost and remove invalid assignments */
	f_computeassignmentcost(v_assignment, v_cost, v_distMatrixIn, v_nOfRows);

	/* free allocated memory */
	delete[] v_distMatrix;
	free(v_coveredColumns);
	free(v_coveredRows);
	free(v_starMatrix);
	free(v_primeMatrix);
	free(v_newStarMatrix);

	return;
}

/********************************************************/
void f_buildassignmentvector(int *v_assignment, bool *v_starMatrix, int v_nOfRows, int v_nOfColumns)
{
	int v_row, v_col;

	for (v_row = 0; v_row<v_nOfRows; v_row++)
		for (v_col = 0; v_col<v_nOfColumns; v_col++)
			if (v_starMatrix[v_row + v_nOfRows*v_col])
			{
#ifdef ONE_INDEXING
				v_assignment[v_row] = v_col + 1; /* MATLAB-Indexing */
#else
				v_assignment[v_row] = v_col;
#endif
				break;
			}
}

/********************************************************/
void f_computeassignmentcost(int *v_assignment, Number *v_cost, Number *v_distMatrix, int v_nOfRows)
{
	int v_row, v_col;

	for (v_row = 0; v_row<v_nOfRows; v_row++)
	{
		v_col = v_assignment[v_row];
		if (v_col >= 0)
			*v_cost += v_distMatrix[v_row + v_nOfRows*v_col];
	}
}

/********************************************************/
void f_step2a(int *v_assignment, Number *v_distMatrix, bool *v_starMatrix, bool *v_newStarMatrix, bool *v_primeMatrix, bool *v_coveredColumns, bool *v_coveredRows, int v_nOfRows, int v_nOfColumns, int v_minDim)
{
	bool *v_starMatrixTemp, *v_columnEnd;
	int v_col;

	/* cover every column containing a starred zero */
	for (v_col = 0; v_col<v_nOfColumns; v_col++)
	{
		v_starMatrixTemp = v_starMatrix + v_nOfRows*v_col;
		v_columnEnd = v_starMatrixTemp + v_nOfRows;
		while (v_starMatrixTemp < v_columnEnd){
			if (*v_starMatrixTemp++)
			{
				v_coveredColumns[v_col] = true;
				break;
			}
		}
	}

	/* move to step 3 */
	f_step2b(v_assignment, v_distMatrix, v_starMatrix, v_newStarMatrix, v_primeMatrix, v_coveredColumns, v_coveredRows, v_nOfRows, v_nOfColumns, v_minDim);
}

/********************************************************/
void f_step2b(int *v_assignment, Number *v_distMatrix, bool *v_starMatrix, bool *v_newStarMatrix, bool *v_primeMatrix, bool *v_coveredColumns, bool *v_coveredRows, int v_nOfRows, int v_nOfColumns, int v_minDim)
{
	int v_col, v_nOfCoveredColumns;

	/* count covered columns */
	v_nOfCoveredColumns = 0;
	for (v_col = 0; v_col<v_nOfColumns; v_col++)
		if (v_coveredColumns[v_col])
			v_nOfCoveredColumns++;

	if (v_nOfCoveredColumns == v_minDim)
	{
		/* algorithm finished */
		f_buildassignmentvector(v_assignment, v_starMatrix, v_nOfRows, v_nOfColumns);
	}
	else
	{
		/* move to step 3 */
		f_step3(v_assignment, v_distMatrix, v_starMatrix, v_newStarMatrix, v_primeMatrix, v_coveredColumns, v_coveredRows, v_nOfRows, v_nOfColumns, v_minDim);
	}

}

/********************************************************/
void f_step3(int *v_assignment, Number *v_distMatrix, bool *v_starMatrix, bool *v_newStarMatrix, bool *v_primeMatrix, bool *v_coveredColumns, bool *v_coveredRows, int v_nOfRows, int v_nOfColumns, int v_minDim)
{
	bool v_zerosFound;
	int v_row, v_col, v_starCol;

	v_zerosFound = true;
	while (v_zerosFound)
	{
		v_zerosFound = false;
		for (v_col = 0; v_col<v_nOfColumns; v_col++)
			if (!v_coveredColumns[v_col])
				for (v_row = 0; v_row<v_nOfRows; v_row++)
					if ((!v_coveredRows[v_row]) && (fabs(boost::rational_cast<double>(v_distMatrix[v_row + v_nOfRows*v_col])) < DBL_EPSILON))
					{
						/* prime zero */
						v_primeMatrix[v_row + v_nOfRows*v_col] = true;

						/* find starred zero in current v_row */
						for (v_starCol = 0; v_starCol<v_nOfColumns; v_starCol++)
							if (v_starMatrix[v_row + v_nOfRows*v_starCol])
								break;

						if (v_starCol == v_nOfColumns) /* no starred zero found */
						{
							/* move to step 4 */
							f_step4(v_assignment, v_distMatrix, v_starMatrix, v_newStarMatrix, v_primeMatrix, v_coveredColumns, v_coveredRows, v_nOfRows, v_nOfColumns, v_minDim, v_row, v_col);
							return;
						}
						else
						{
							v_coveredRows[v_row] = true;
							v_coveredColumns[v_starCol] = false;
							v_zerosFound = true;
							break;
						}
					}
	}

	/* move to step 5 */
	f_step5(v_assignment, v_distMatrix, v_starMatrix, v_newStarMatrix, v_primeMatrix, v_coveredColumns, v_coveredRows, v_nOfRows, v_nOfColumns, v_minDim);
}

/********************************************************/
void f_step4(int *v_assignment, Number *v_distMatrix, bool *v_starMatrix, bool *v_newStarMatrix, bool *v_primeMatrix, bool *v_coveredColumns, bool *v_coveredRows, int v_nOfRows, int v_nOfColumns, int v_minDim, int v_row, int v_col)
{
	int n, v_starRow, v_starCol, v_primeRow, v_primeCol;
	int v_nOfElements = v_nOfRows*v_nOfColumns;

	/* generate temporary copy of v_starMatrix */
	for (n = 0; n<v_nOfElements; n++)
		v_newStarMatrix[n] = v_starMatrix[n];

	/* star current zero */
	v_newStarMatrix[v_row + v_nOfRows*v_col] = true;

	/* find starred zero in current column */
	v_starCol = v_col;
	for (v_starRow = 0; v_starRow<v_nOfRows; v_starRow++)
		if (v_starMatrix[v_starRow + v_nOfRows*v_starCol])
			break;

	while (v_starRow<v_nOfRows)
	{
		/* unstar the starred zero */
		v_newStarMatrix[v_starRow + v_nOfRows*v_starCol] = false;

		/* find primed zero in current v_row */
		v_primeRow = v_starRow;
		for (v_primeCol = 0; v_primeCol<v_nOfColumns; v_primeCol++)
			if (v_primeMatrix[v_primeRow + v_nOfRows*v_primeCol])
				break;

		/* star the primed zero */
		v_newStarMatrix[v_primeRow + v_nOfRows*v_primeCol] = true;

		/* find starred zero in current column */
		v_starCol = v_primeCol;
		for (v_starRow = 0; v_starRow<v_nOfRows; v_starRow++)
			if (v_starMatrix[v_starRow + v_nOfRows*v_starCol])
				break;
	}

	/* use temporary copy as new v_starMatrix */
	/* delete all primes, uncover all rows */
	for (n = 0; n<v_nOfElements; n++)
	{
		v_primeMatrix[n] = false;
		v_starMatrix[n] = v_newStarMatrix[n];
	}
	for (n = 0; n<v_nOfRows; n++)
		v_coveredRows[n] = false;

	/* move to step 2a */
	f_step2a(v_assignment, v_distMatrix, v_starMatrix, v_newStarMatrix, v_primeMatrix, v_coveredColumns, v_coveredRows, v_nOfRows, v_nOfColumns, v_minDim);
}

/********************************************************/
void f_step5(int *v_assignment, Number *v_distMatrix, bool *v_starMatrix, bool *v_newStarMatrix, bool *v_primeMatrix, bool *v_coveredColumns, bool *v_coveredRows, int v_nOfRows, int v_nOfColumns, int v_minDim)
{
	Number h, v_value;
	int v_row, v_col;

	/* find smallest uncovered element h */
	h = Number{std::numeric_limits<int>::max()};
	for (v_row = 0; v_row<v_nOfRows; v_row++)
		if (!v_coveredRows[v_row])
			for (v_col = 0; v_col<v_nOfColumns; v_col++)
				if (!v_coveredColumns[v_col])
				{
					v_value = v_distMatrix[v_row + v_nOfRows*v_col];
					if (v_value < h)
						h = v_value;
				}

	/* add h to each covered v_row */
	for (v_row = 0; v_row<v_nOfRows; v_row++)
		if (v_coveredRows[v_row])
			for (v_col = 0; v_col<v_nOfColumns; v_col++)
				v_distMatrix[v_row + v_nOfRows*v_col] += h;

	/* subtract h from each uncovered column */
	for (v_col = 0; v_col<v_nOfColumns; v_col++)
		if (!v_coveredColumns[v_col])
			for (v_row = 0; v_row<v_nOfRows; v_row++)
				v_distMatrix[v_row + v_nOfRows*v_col] -= h;

	/* move to step 3 */
	f_step3(v_assignment, v_distMatrix, v_starMatrix, v_newStarMatrix, v_primeMatrix, v_coveredColumns, v_coveredRows, v_nOfRows, v_nOfColumns, v_minDim);
}

//********************************************************//
// A single function wrapper for solving assignment problem.
//********************************************************//
Number solveHungarian(vector<vector<Number>> &v_DistMatrix, vector<int> &v_Assignment)
{
	unsigned int v_nRows = v_DistMatrix.size();
	unsigned int v_nCols = v_DistMatrix[0].size();

	Number *v_distMatrixIn = new Number[v_nRows * v_nCols];
	int *v_assignment = new int[v_nRows];
	Number v_cost = Number{0};

	// Fill in the v_distMatrixIn. Mind the index is "i + v_nRows * j".
	// Here the v_cost matrix of size MxN is defined as a double precision array of N*M elements.
	// In the solving functions matrices are seen to be saved MATLAB-internally in row-order.
	// (i.e. the matrix [1 2; 3 4] will be stored as a vector [1 3 2 4], NOT [1 2 3 4]).
	for (unsigned int i = 0; i < v_nRows; i++)
		for (unsigned int j = 0; j < v_nCols; j++)
			v_distMatrixIn[i + v_nRows * j] = v_DistMatrix[i][j];

	// call solving function
	f_assignmentoptimal(v_assignment, &v_cost, v_distMatrixIn, v_nRows, v_nCols);

	v_Assignment.clear();
	for (unsigned int r = 0; r < v_nRows; r++)
		v_Assignment.push_back(v_assignment[r]);

	delete[] v_distMatrixIn;
	delete[] v_assignment;
	return v_cost;
}

