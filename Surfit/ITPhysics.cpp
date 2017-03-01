// Dom's includes.
#include "ITPhysics.h"
#include "global.h"
#include "ITProject.h"



ITPhysics::ITPhysics(void)
{
}


ITPhysics::~ITPhysics(void)
{
}


int ITPhysics::multiplyArbitraryMatrices(int rA, int cA, char storageModeA, float*A, int rB, int cB, char storageModeB, float*B, float*C)
{

	// This function takes input matrices as either ROW MAJOR or COL MAJOR and computes the output matrix as ROW MAJOR.
	// Note that only ROW MAJOR is currently implemented.
	// The output matrix R has dimensions rA x cB.

	// Return codes: 0 - Success.
	//				 1 - cA != rB

	//  First check that cA == rB
	if (cA == rB)
	{
		for (int i=0; i<rA; i++) // Loop over the rows of R.
		{	

			for (int j=0; j<cB; j++) // Loop over the columns of R.
			{

				C[ ROW_MAJOR_IDX2C(i, j, cB) ] = 0.0;		

				for (int k=0; k<cA; k++) // Walk along the columns of A and the rows of B adding element products.
				{  																							
				    C[ ROW_MAJOR_IDX2C(i, j, cB) ] = C[ ROW_MAJOR_IDX2C(i, j, cB) ] + A[ ROW_MAJOR_IDX2C(i, k, cA) ]*B[ ROW_MAJOR_IDX2C(k, j, cB) ];
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "A[%i,%i]: %f, B[%i,%i]: %f, C[%i,%i]: %f", i, k, A[ ROW_MAJOR_IDX2C(i, k, cA) ], k, j, B[ ROW_MAJOR_IDX2C(k, j, cB) ], i, j, C[ ROW_MAJOR_IDX2C(i, j, cB) ]);
				}
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, " ");
			}

		}	
		return 0;
	}
	else
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 0, "Matrix dimensions are not compatible. First matrix has %i columns, and second matrix has %i rows", cA, rB);
		return 1;
	}
} // End of multiplyArbitraryMatrices.


void ITPhysics::printDebugMatrix(int r, int c, char storageModeA, float*A, std::string title, int debugLevel)
{

	// This prints to the output.txt file.
	if (debugLevel < project->get_DebugLevel())
	{
		printf("%s\n", title.c_str());
		for (int i=0; i<r; i++)
		{
			for (int j=0; j<c; j++)
			{
				printf("%f, ", A[ ROW_MAJOR_IDX2C(i, j, c) ]);
			}
			printf("\n");
		}
	}
}