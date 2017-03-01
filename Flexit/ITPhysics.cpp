// System includes.
#include <QTime> // Used in the delay function.



// Dom's includes.
#include "ITPhysics.h"
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITPanel.h"
#include "ITVortex.h"
#include "ITWakePanel.h"
#include "ITBeam.h"
#include "ITBeamNode.h"
#include "ITTrajectoryCurve.h" // Used in playOutTest.
#include "ITTrajectoryCurveSegment.h" // Used in playOutTest.
#include "ITPointTrajectory.h" // Used in playoutTest.
#include "ITGust.h"
#include "ITControlSurface.h"

#include "CudaPhysics.cuh"



ITPhysics::ITPhysics(void)
{
}


ITPhysics::~ITPhysics(void)
{
}


void ITPhysics::delay(int millisecondsToWait)
{
	QTime dieTime = QTime::currentTime().addMSecs(millisecondsToWait);
	while (QTime::currentTime() < dieTime)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
} // End of delay.




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
		for (int i = 0; i<rA; i++) // Loop over the rows of R.
		{

			for (int j = 0; j<cB; j++) // Loop over the columns of R.
			{

				C[ROW_MAJOR_IDX2C(i, j, cB)] = 0.0;

				for (int k = 0; k<cA; k++) // Walk along the columns of A and the rows of B adding element products.
				{
					C[ROW_MAJOR_IDX2C(i, j, cB)] = C[ROW_MAJOR_IDX2C(i, j, cB)] + A[ROW_MAJOR_IDX2C(i, k, cA)] * B[ROW_MAJOR_IDX2C(k, j, cB)];
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "A[%i,%i]: %f, B[%i,%i]: %f, C[%i,%i]: %f", i, k, A[ROW_MAJOR_IDX2C(i, k, cA)], k, j, B[ROW_MAJOR_IDX2C(k, j, cB)], i, j, C[ROW_MAJOR_IDX2C(i, j, cB)]);
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

void ITPhysics::printThreeByThreeMatrix(int r, int c, char storageModeA, float*A, std::string title, int debugLevel)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, debugLevel, "%s", title.c_str());
	for (int i = 0; i<3; i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, debugLevel, "%f %f %f", A[ROW_MAJOR_IDX2C(i, 0, 3)], A[ROW_MAJOR_IDX2C(i, 1, 3)], A[ROW_MAJOR_IDX2C(i, 2, 3)]);
	}

}


void ITPhysics::printDebugMatrix(int r, int c, char storageModeA, float*A, std::string title, int debugLevel)
{

	// This prints to the output.txt file.
	if (debugLevel < project->get_DebugLevel())
	{
		printf("%s\n", title.c_str());
		for (int i = 0; i<r; i++)
		{
			for (int j = 0; j<c; j++)
			{
				printf("%f, ", A[ROW_MAJOR_IDX2C(i, j, c)]);
			}
			printf("\n");
		}
	}
}




void ITPhysics::playOutStep()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside playOutStep.");

	// Prepare for a simulation (empty and delete wake panels and velocity field).
	if (FrameNumber == 0)
	{
		prepareForSimulation();
	}

	if ((!IsPaused) && (!IsReplay) && (!IsDryRun) && (!IsSimulating) && (IsStep))
	{

		UnsavedChanges = true;

		LARGE_INTEGER t1, t2, frequency;
		double elapsedTime;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&t1);

		ITPhysics::PropagateFlexit(FrameNumber);

		QueryPerformanceCounter(&t2);
		elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for frame %i is %f, msecs", FrameNumber, elapsedTime);

		cummulativeElapsedTimeSeconds = cummulativeElapsedTimeSeconds + elapsedTime*0.001;

		if (MY_RUN_MODE == MYGUI)
		{
			w->statusBar()->showMessage(QString("Step completed. Frame: %1 of: %2. Total computation time: %3 secs.").arg(FrameNumber).arg(project->get_MaxKeyFrame()).arg(cummulativeElapsedTimeSeconds));
		}

		// Update the main widget.
		if (MY_RUN_MODE == MYGUI)
		{
			w->updateAllTabsForCurrentFrameNumber();
			ITPhysics::delay(10); // 10 msecs for rendering the current Frame.
		}


		// Finally increment the frame number.
		FrameNumber++;

	} // End of while mode is IsStep.


} // End of playOutStep.











void ITPhysics::prepareForSimulation()
{

	// Reset the elapsed time.
	cummulativeElapsedTimeSeconds = 0.0;

	// Loop through surfaces deleting wake panels.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		project->get_MySurfaces()->at(k)->prepareForSimulation();
	}

	// Delete any pre-existing velocity field data.
	int noOfRows = project->get_VelocityFieldData()->size();
	for (int i = 0; i<noOfRows; i++)
	{
		int noOfCols = project->get_VelocityFieldData()->at(i).size();
		for (int j = 0; j<noOfCols; j++)
		{
			delete project->get_VelocityFieldData()->at(i).at(j);
		}
		project->get_VelocityFieldData()->at(i).clear();
	}

	project->get_VelocityFieldData()->clear();


} // End of prepareForSimulation.




void ITPhysics::PropagateFlexit(int FrameNumber)
{

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 1, "Start FrameNumber: %i of %i", FrameNumber, project->get_MaxKeyFrame());

	LARGE_INTEGER t1, t2, frequency;
	double elapsedTime;


	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::CreateMatrixAndSolveForVortexStrengths(FrameNumber);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Time for CreateMatrixAndSolveForVortexStrengths is %f, msecs", elapsedTime);

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::PropagatePressureDistributionAndForces();
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for PropagatePressureDistributionAndForces is %f, msecs", elapsedTime);

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::PropagateSurfaceGeometry(FrameNumber); // This includes the re-generation of the panel geometry.
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for PropagateSurfaceGeometry is %f, msecs", elapsedTime);

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::InsertWakePanels(FrameNumber);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for InsertWakePanels is %f, msecs", elapsedTime);

	if (FrameNumber > 0) // This is necessary 20160716.
	{

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&t1);
		ITPhysics::PropagateWakeGeometry(FrameNumber);
		QueryPerformanceCounter(&t2);
		elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for PropagateWakeGeometry is %f, msecs", elapsedTime);

		// Compute vector field on cross-section plane.
		if (project->get_IsDoVelocityField())
		{
			QueryPerformanceFrequency(&frequency);
			QueryPerformanceCounter(&t1);
			ITPhysics::calcVelocityFieldCuda(FrameNumber);
			QueryPerformanceCounter(&t2);
			elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for calcVelocityFieldCuda is %f, msecs", elapsedTime);
		}

	}

	// Store data for replay.
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::StoreDataForReplay();
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for StoreDataForReplay is %f, msecs", elapsedTime);

}




void ITPhysics::CreateMatrixAndSolveForVortexStrengths(int FrameNumber)
{
	// This function is called from PropagateFlexit


	// =====================================================================================
	// This function calculates the linear sum of velocities induced on each panel by unit
	// strength vortices on all the other panels. Then it computes the actual vortex strengths
	// required to satisfy the boundary condition of tangential flow on each panel.
	// It does this by constructing and solving a linear system using matrix methods.
	// =====================================================================================

	// =====================================================================================
	// Compute the dimensions of the A matrix.
	// The A matrix is of size rxr, where r is the total number of bound panels in
	// the project (on all surfaces, including reflected ones).
	// =====================================================================================


	int noOfUnknownVortexStrengths = 0; // Initialize r. This is just the number of surface panels.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through all the surfaces, adding the number of panels to r.
	{
		noOfUnknownVortexStrengths = noOfUnknownVortexStrengths + (project->get_MySurfaces()->at(k)->get_MyPanels()->size()  *  project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size());
	}

	// The solution vector (that will contain the vortex strengths).
	std::vector <float> *solPointer = new std::vector <float>(noOfUnknownVortexStrengths);

	project->set_TotalProblemSize(noOfUnknownVortexStrengths);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Total problem size: %i", project->get_TotalProblemSize());

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Size of solPointer: %i. Last value is: %f", solPointer->size(), solPointer->back());


	LARGE_INTEGER t1, t2, frequency;
	double elapsedTime;

	// =====================================================================================
	// Construct the matrix of influence coefficients A.
	// =====================================================================================
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::ManageCalculationOfMatrixOfCoefficients(solPointer, noOfUnknownVortexStrengths);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for ManageCalculationOfMatrixOfCoefficients is %f, msecs", elapsedTime);

	// =====================================================================================
	// Compute the vector on the right-hand side of the equation Ax = b.
	// =====================================================================================
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::ManageCalculationOfVectorB(noOfUnknownVortexStrengths);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for ManageCalculationOfVectorB is %f, msecs", elapsedTime);


	// =====================================================================================
	// Solve for the unknown surface panel vortex strengths.
	// =====================================================================================
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::SolveForSol(solPointer, noOfUnknownVortexStrengths);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for SolveForSol is %f, msecs", elapsedTime);


	// Assign the vorticities to the surface panels (not the wake panels).
	// This is just storing solPointer in a more object-oriented way.
	int solCounter = 0;
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through all the surfaces.
	{
		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{

				project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->set_MyVorticity(solPointer->at(solCounter));

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Setting vorticity: %f, solPointer: %f", project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVorticity(), solPointer->at(solCounter));

				solCounter++;
			}
		}
	}

	// Empty and delete solPointer.
	solPointer->clear();
	delete solPointer;

} // End of CreateMatrixAndSolveForVortexStrengths.




void ITPhysics::ManageCalculationOfMatrixOfCoefficients(std::vector <float> *solPointer, int noOfUnknownVortexStrengths)
{
	// The matrix of influence coefficients only includes terms from the vortices on the bounding surfaces (not the wake).
	// The bounding surface geometry needs to be uploaded to the GPU at each frame because the geometry changes at each frame.
	// This is unlike the Steady VLM case where the geometry remains fixed from one Frame to the next.

	int totalNumberOfCudaComputations = noOfUnknownVortexStrengths*noOfUnknownVortexStrengths;

	// Define the maximum number of vortices of any panel.
	// This defines the pitch of the vortex data in the memory arrays.
	int maxNoOfVortices = 4; // Do not change this value.

	// Set up allocation of arrays of panels and vertices in host memory.
	size_t sizePanelsFloat = noOfUnknownVortexStrengths * sizeof(float); // Memory required for a row of floats.
	size_t sizePanelsInt = noOfUnknownVortexStrengths * sizeof(int); // Memory required for a row of ints.
	size_t sizeMatrixFloat = totalNumberOfCudaComputations * sizeof(float); // Memory required for a matrix of floats. (noOfPanels x noOfPanels)

	// Panel colocation point coordinates (object points).
	float *h_cp_x = (float *)malloc(sizePanelsFloat); // Allocate the host panel control-point x-coordinate.
	float *h_cp_y = (float *)malloc(sizePanelsFloat); // Allocate the host panel control-point y-coordinate.
	float *h_cp_z = (float *)malloc(sizePanelsFloat); // Allocate the host panel control-point x-coordinate.

	// Panel normal coordinates (the normals at the colocation points).
	float *h_n_x = (float *)malloc(sizePanelsFloat); // Allocate the host panel unit normal x-coordinate.
	float *h_n_y = (float *)malloc(sizePanelsFloat); // Allocate the host panel unit normal y-coordinate.
	float *h_n_z = (float *)malloc(sizePanelsFloat); // Allocate the host panel unit normal z-coordinate.

	// Vortex start point coordinates (only the bound vortices).
	float *h_vs_x = (float *)malloc(sizePanelsFloat*maxNoOfVortices); // Vortex start point x-coord.
	float *h_vs_y = (float *)malloc(sizePanelsFloat*maxNoOfVortices); // Vortex start point y-coord.
	float *h_vs_z = (float *)malloc(sizePanelsFloat*maxNoOfVortices); // Vortex start point z-coord.

	// Vortex end point coordinates (only the bound vortices).
	float *h_ve_x = (float *)malloc(sizePanelsFloat*maxNoOfVortices); // Vortex end point x-coord.
	float *h_ve_y = (float *)malloc(sizePanelsFloat*maxNoOfVortices); // Vortex end point y-coord.
	float *h_ve_z = (float *)malloc(sizePanelsFloat*maxNoOfVortices); // Vortex end point z-coord.

	// Allocate the host output matrix of influence coefficients. The square matrix.
	float *h_A = (float *)malloc(sizeMatrixFloat);

	// Verify that allocations succeeded.
	if (h_cp_x == NULL || h_cp_y == NULL || h_cp_z == NULL || h_n_x == NULL || h_n_y == NULL || h_n_z == NULL
		|| h_vs_x == NULL || h_vs_y == NULL || h_vs_z == NULL || h_ve_x == NULL || h_ve_y == NULL || h_ve_z == NULL || h_A == NULL)
	{
		fprintf(stderr, "Failed to allocate host vectors!\n");
		exit(EXIT_FAILURE);
	}


	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Host memory allocated successfully.");

	// Initialize the host array of coefficients (the square matrix of coefficients).
	for (int i = 0; i<totalNumberOfCudaComputations; i++) { h_A[i] = 0.0; }

	// Initialize the host arrays of panel colocation points and vortex segment end-points.
	int panelIndex = 0; // Panel index.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // DOM: Loop through the surfaces.
	{

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "================================= k: %i", k);

		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++) // DOM: Loop through the rows of the panels.
		{
			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++) // DOM: Loop through the columns of the panels.
			{
				// DOM: Colocation point data.

				// DOM: Assign panel control point coordinates.
				h_cp_x[panelIndex] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X();
				h_cp_y[panelIndex] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y();
				h_cp_z[panelIndex] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z();

				// DOM: Assign panel control point unit normals.
				h_n_x[panelIndex] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X();
				h_n_y[panelIndex] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y();
				h_n_z[panelIndex] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z();

				// DOM: Vortex data.               

				// ============================================================================
				// DOM: Loop through all the four bound vortices on the current panel.
				// DOM: The way to think of this is matrix with a row for each panel, and a column for each vortex on the panel. In this implementation, the number of columns is always 4.
				// ============================================================================
				for (int jj = 0; jj<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVortices()->size(); jj++)
				{
					// DOM: Vortex coordinates.
					h_vs_x[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVortices()->at(jj)->get_StartPoint()->get_X();
					h_vs_y[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVortices()->at(jj)->get_StartPoint()->get_Y();
					h_vs_z[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVortices()->at(jj)->get_StartPoint()->get_Z();

					h_ve_x[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVortices()->at(jj)->get_EndPoint()->get_X();
					h_ve_y[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVortices()->at(jj)->get_EndPoint()->get_Y();
					h_ve_z[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVortices()->at(jj)->get_EndPoint()->get_Z();

					// Check for vortices of zero length.
					float epsilon = 0.0001;
					if ((fabs(h_vs_x[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] - h_ve_x[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)]) < epsilon)
						&& (fabs(h_vs_y[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] - h_ve_y[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)]) < epsilon)
						&& (fabs(h_vs_z[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)] - h_ve_z[ROW_MAJOR_IDX2C(panelIndex, jj, maxNoOfVortices)]) < epsilon))
					{
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Zero length bound vortex detected - Matrix of Coeffs.");
					}


				}

				// DOM: Number of bound vortices associated with this panel is assumed to always be 4.

				panelIndex++; // Increment the panel counting index.
			}
		}
	}

	// Do the influence coefficients in CudaPhysics.cu.
	ConstructMatrixOfInfluenceCoefficientsCuda(
		h_cp_x,
		h_cp_y,
		h_cp_z,
		h_n_x,
		h_n_y,
		h_n_z,
		h_vs_x,
		h_vs_y,
		h_vs_z,
		h_ve_x,
		h_ve_y,
		h_ve_z,
		h_A, // This call fills the memory pointed to by h_A.
		noOfUnknownVortexStrengths,
		project->get_RankineCoreRadius(),
		project->get_RankineAlgorithm().at(0),
		FrameNumber);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ConstructMatrixOfInfluenceCoefficientsCuda finished.");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "h_A[20] = %f", h_A[20]);


	// Debugging print the matrix of influence coefficients.
	printDebugMatrix(noOfUnknownVortexStrengths, noOfUnknownVortexStrengths, 'R', h_A, "A", 12);
	printDebugMatrix(panelIndex, 1, 'R', h_cp_x, "CP x", 12);
	printDebugMatrix(panelIndex, 1, 'R', h_cp_y, "CP y", 12);
	printDebugMatrix(panelIndex, 1, 'R', h_cp_z, "CP z", 12);

	printDebugMatrix(panelIndex, 1, 'R', h_n_x, "n x", 12);
	printDebugMatrix(panelIndex, 1, 'R', h_n_y, "n y", 12);
	printDebugMatrix(panelIndex, 1, 'R', h_n_z, "n z", 12);

	// Empty project->get_A() and delete and re-create in preparation for filling it with new influence coefficient matrix values.
	if (!project->get_A()->empty())
	{
		for (int i = 0; i<project->get_A()->size(); i++)
		{
			project->get_A()->at(i).clear();
		}
		project->get_A()->clear();
	}

	delete project->get_A();

	project->set_A(new std::vector < std::vector <double> >);

	// Fill project->get_A().
	int arrayIndex = 0; // Counts through the h_A array (that represents the square matrix of coefficients A).
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{
		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++) // Loop through the rows of the (object) panels i.
		{
			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++) // Loop through the columns of the (object) panels i.
			{
				std::vector <double> *dummy = new std::vector <double>; // This vector will contain entries for a row of the coefficient matrix (all entries are for a single object panel i).

				for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // Loop through the (subject) surfaces j.
				{

					// Loop through all the (subject) panels.
					for (int ii = 0; ii<project->get_MySurfaces()->at(kk)->get_MyPanels()->size(); ii++)
					{
						for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).size(); jj++)
						{

							dummy->push_back((double)h_A[arrayIndex]);
							arrayIndex++;
						}
					}
				}

				project->get_A()->push_back(*dummy);
			}
		}
	}


	for (int i = 0; i<noOfUnknownVortexStrengths*maxNoOfVortices; i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "h_vs_x[ %i ] = %f, h_ve_x[ %i ] = %f", i, h_vs_x[i], i, h_ve_x[i]);
	}

	w->manageUpdateOfDisplayMatrix(noOfUnknownVortexStrengths);

	// Free host memory.
	free(h_cp_x);
	free(h_cp_y);
	free(h_cp_z);

	free(h_n_x);
	free(h_n_y);
	free(h_n_z);

	free(h_vs_x);
	free(h_vs_y);
	free(h_vs_z);

	free(h_ve_x);
	free(h_ve_y);
	free(h_ve_z);

	free(h_A);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End of ManageCalculationOfMatrixOfCoefficients");

} // End of ManageCalculationOfMatrixOfCoefficients.



void ITPhysics::ManageCalculationOfVectorB(int noOfUnknownVortexStrengths)
{

	// This function computes the entries of the vector b (RHS of equation 4.21) where
	// b = -(V.n + T.n) = - V.n - T.n. Note that the elements of b include the minus sign.
	//
	// Reference:   1. /Users/dominiquefleischmann/Documents/WorkingFolder/Mathematics/Tex/Surfit/Aeroelasticity Monograph/master.pdf
	// Revision History:
	// 20161214: Include Gust velocities. Remove temp b parameter and use project instance variable instead.

	// Empty project->get_B() and delete and re-create in preparation for filling it with new wake influence coefficient matrix values.
	if (!project->get_B()->empty())
	{
		project->get_B()->clear();
	}

	delete project->get_B();

	project->set_B(new std::vector <double>);


	// DOM: fill  B.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{
		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++) // Loop through the rows of the (object) panels i.
		{
			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++) // Loop through the columns of the (object) panels j.
			{
				// Each pass through this inner loop corresponds to a single entry in the b vector.

				// DOM: Calculate the entries in the vector b on the right-hand side of the matrix equation.
				// This is equation 4.19 from the paper.
				//         trajectory velocity  wake induced velocity    gust velocity
				// b_[i} =  T_{i}.n_{i}^{t+1} - V_i^(t+1).n_{i}^{t+1} - g_{i}.n_{i}^{t+1} , 
				//                                  where
				//                                  i is the index of the object panel,
				//                                  and t is the iteration number.
				//

				// Start by calculating the subject panel colocation point trajectory velocity T_{i}^{t+1}.
				// Note that the panel normal should be updated every frame in the PropagateSurfaceGeometry function.
				// The next line updates the object panel mid-point earth velocity vector.
				project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->calculateMyTrajectoryVelocity(k);

				// Checked 20150503. Correct.
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, i: %i, j: %i, Panel trajectory velocity: %f, %f, %f",
					k, i, j,
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyMidPointEarthVelocity()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyMidPointEarthVelocity()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyMidPointEarthVelocity()->get_Z());

				// Compute the dot product of the panel veclocity with the panel surface normal.
				//
				// Note that the panel normal should be updated every frame in the
				// PropagateSurfaceGeometry function.

				// TODO: Reverse sense of V.
				// Note that the RHS vector of the matrix equation is the negative of the dot product of the flow velocities with the panel normals.
				// Since the panel earth velocities are the velocities of the panel in the earth frame, the flow velocity is the negative of these vectors. 
				// This means that the positive of the earth velocity should be used in the below calculation here. 
				float TiDotN = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyMidPointEarthVelocity()->get_X()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X()
					+ project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyMidPointEarthVelocity()->get_Y()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y()
					+ project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyMidPointEarthVelocity()->get_Z()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z();

				// 20150612.

				// Now construct the term VW in equation 4.18 from the paper.
				// This is the linear superposition of all induced velocities from
				// all vortices from all wake panels (with known vorticity)
				// evaluated at the mid-point of the object panel.
				float VWdotN = 0.0f; // Velocity induced by all wake panels at the object panel collocation point. 

				bool IsIncludeWakeTerms = true;

				if (IsIncludeWakeTerms)
				{

					for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // Loop through the subject surfaces.
					{

						// Find the maximum and minimum generation indices for this run.
						int firstGeneration;
						int lastGeneration;

						if (project->get_MaxPropagationGeneration() > 0)
						{
							if (project->get_MaxPropagationGeneration() < project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size())
							{
								firstGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size() - project->get_MaxPropagationGeneration();
								lastGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size();
							}
							else
							{
								firstGeneration = 0;
								lastGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size();
							}
						}
						else
						{
							firstGeneration = 0;
							lastGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size();
						}

						// Loop through all the subject wake panel generations.
						for (int ii = firstGeneration; ii<lastGeneration; ii++)
						{
							// Loop down the trailing edge of subject panels within the current generation.
							for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).size(); jj++)
							{
								ITWakePanel* currentSubjectWakePanel = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj);

								// Loop around all the vortices in the current wake panel.
								for (int tt = 0; tt<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->size(); tt++)
								{

									//  Find the velocity induced at the colocation point of the current object panel due to the unit strength tt-th vortex of the ii,jj subject wake panel.
									ITVortex* currentSubjectWakeVortex = currentSubjectWakePanel->get_MyVortices()->at(tt);

									// Define a pointer to the ITPoint object containing the induced velocity that is instanciated in the calc method.
									ITPoint* velInducedAtObjectPanel = currentSubjectWakeVortex->calcFiniteSvensonAt(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint());

									// Scale the influence coefficient linearly with the wake panel vorticity.
									velInducedAtObjectPanel->set_X(velInducedAtObjectPanel->get_X()*currentSubjectWakePanel->get_MyVorticity());
									velInducedAtObjectPanel->set_Y(velInducedAtObjectPanel->get_Y()*currentSubjectWakePanel->get_MyVorticity());
									velInducedAtObjectPanel->set_Z(velInducedAtObjectPanel->get_Z()*currentSubjectWakePanel->get_MyVorticity());

									if (FrameNumber == 1)
									{
										project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Vel X: %f, Vorticity: %f", velInducedAtObjectPanel->get_X(), currentSubjectWakePanel->get_MyVorticity());
									}

									// 20150518: Track down NaN.
									if (project->custom_isnan(velInducedAtObjectPanel->get_X()))
									{
										project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "NaN detected. FrameNumber: %i, ii: %i, jj: %i, tt: %i", FrameNumber, ii, jj, tt);
									}

									// Form the dot product of the subject induced velocity vector with the object surface panel normal.
									// The incremental term is b_{ij} from equation 12.23 in Katz and Plotkin.
									float incrementalTerm = velInducedAtObjectPanel->get_X()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X()
										+ velInducedAtObjectPanel->get_Y()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y()
										+ velInducedAtObjectPanel->get_Z()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z();

									// Accumulate the TdotN term.
									VWdotN = VWdotN + incrementalTerm;

									// Finally free memory.
									delete velInducedAtObjectPanel;

								} // End loop over vortex segments of a subject panel.


							} // End of jj loop.

						} // End of loop through generations.

					} // End of loop through subject surfaces.


				} // End of IsIncludeWakeTerms.

				float gustVelocityDotN = 0.0;
				if (project->get_IsGust())
				{

					// The dot product of the gust velocity with the object panel normal.
					float x = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X();
					float y = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y();
					float z = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z();

					float vx = 0.0;
					float vy = 0.0;
					float vz = 0.0;

					project->get_MyGust()->computeVelocityAtPoint(x, y, z, &vx, &vy, &vz);

					gustVelocityDotN = vx*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X()
						+ vy*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y()
						+ vz*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z();
				}



				// This is the RHS of the main linear system equation.
				// See equation 4.19 from the paper.
				// The element of B is the velocity components normal to the object panel due to the panel trajectory velocity minus the velocity induced by the wakes minus the gust velocity.
				project->get_B()->push_back((double)(TiDotN - VWdotN - gustVelocityDotN));

			} // End of j loop though object panels.

		} // End of i loop though object panels. 

	} // End of k loop through the object surfaces.


} // End of ManageCalculationOfVectorB.




void ITPhysics::SolveForSol(std::vector <float> *solPointer, int noOfUnknownVortexStrengths)
{

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside SolveForSol. FrameNumber: %i ...", FrameNumber);


	// Allocate memory for the A matrix and the LU matrix.
	float* A = 0;  // Pointer to the square matrix on the host with dimensions numvar rows x numvar cols.

	float* LU = 0;  // Pointer to the numvar row x numvar col matrix on the host.
	float* L = 0;  // Pointer to the numvar row x numvar col matrix on the host.
	float* U = 0;  // Pointer to the numvar row x numvar col matrix on the host.
	float* R = 0;  // Pointer to the numvar row x numvar col matrix on the host.

	A = (float *)malloc(noOfUnknownVortexStrengths * noOfUnknownVortexStrengths * sizeof(*A)); // The C array version of STL vv_A.

	LU = (float *)malloc(noOfUnknownVortexStrengths * noOfUnknownVortexStrengths * sizeof(*LU)); // The output of Wallstedt decomposition (Lower and Upper matrices in a single array).
	L = (float *)malloc(noOfUnknownVortexStrengths * noOfUnknownVortexStrengths * sizeof(*L)); // Lower triangular decomposition of B.
	U = (float *)malloc(noOfUnknownVortexStrengths * noOfUnknownVortexStrengths * sizeof(*U)); // Upper triangular decomposition of B.
	R = (float *)malloc(noOfUnknownVortexStrengths * noOfUnknownVortexStrengths * sizeof(*R)); // R will contain L x U.

																							   // Initialize the B matrix from vv_B (B*ExternalSol = b).
																							   // Loop through vv_B.
	for (int i = 0; i<noOfUnknownVortexStrengths; i++) // Loop over the rows of vv_B.
	{
		for (int j = 0; j<noOfUnknownVortexStrengths; j++) // Loop over the columns of vv_B.
		{
			A[ROW_MAJOR_IDX2C(i, j, noOfUnknownVortexStrengths)] = (float)(project->get_A()->at(i).at(j));
		}
	}

	// Do LU decomposition using the Doolittle method.	
	LARGE_INTEGER t1, t2, frequency;
	double elapsedTime;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::DoolittleWallstedt(noOfUnknownVortexStrengths, A, LU);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "SolveForSol. Time for DoolittleWallstedt: %f, msecs", elapsedTime);

	// Separate the upper and lower triangular matrices.
	ITPhysics::separateDoolittleLandU(noOfUnknownVortexStrengths, LU, L, U); // vv_B and v_b are global.


	// ======================================================================================================================
	// Do Dom's CPU Gaussian Elimination algorithm. http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c2-2.pdf
	// Consider:
	// Ax = b, and solve for x.
	// A = LU, so we need to solve for LUx = b, which is L(Ux) = b.
	// Defining unknow y as y = Ux, it follows that we can find x in two stages.
	// First solve Ly = b for y.
	// Then solve Ux = y for x.
	// ======================================================================================================================

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
	ITPhysics::TwoStageGaussianEliminationCPU(L, U, noOfUnknownVortexStrengths, solPointer);
	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "SolveForSol. Time for TwoStageGaussianEliminationCPU: %f, msecs", elapsedTime);

	// Free memory.
	free(A);
	free(LU);
	free(L);
	free(U);
	free(R);

	// Debugging output.
	for (int i = 0; i<solPointer->size(); i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "%f", solPointer->at(i));
	}



} // End of SolveForSol.


















void ITPhysics::DoolittleWallstedt(int d, float *S, float *D)
{
	// References:
	// http://www.sci.utah.edu/~wallstedt

	// The Doolittle convention is that the lower triangular matrix has 1's on the diagonal.
	// The destination matrix D combines L and U.

	for (int k = 0; k<d; ++k)
	{
		for (int j = k; j<d; ++j)
		{
			float sum = 0.0;
			for (int p = 0;p<k;++p)
			{
				sum += D[k*d + p] * D[p*d + j];
			}
			D[k*d + j] = (S[k*d + j] - sum); // not dividing by diagonals
		}

		for (int i = k + 1; i<d; ++i)
		{
			float sum = 0.0;
			for (int p = 0; p<k; ++p)
			{
				sum += D[i*d + p] * D[p*d + k];
			}
			D[i*d + k] = (S[i*d + k] - sum) / D[k*d + k];
		}
	}
} // End of DoolittleWallstedt.

void ITPhysics::separateDoolittleLandU(int d, float *LU, float *L, float *U)
{
	//This function copies the upper triangle of LU into U and the lower triangle of LU into L.
	// References: http://www.sci.utah.edu/~wallstedt

	// According to the Doolittle convention, L has 1's on the diagonal and the diagonal entries of U 
	// are equal to the diagonal entries of LU.

	// Extract the lower matrix.
	for (int i = 0; i<d; ++i)
	{
		for (int j = 0; j<d; ++j)
		{
			if (i>j)
			{
				L[i*d + j] = LU[i*d + j]; // We are below the diagonal.
			}
			else if (i == j)
			{
				L[i*d + j] = 1.0;
			}
			else
			{
				L[i*d + j] = 0.0;
			}
		}
	}

	// Extract the upper matrix.
	for (int i = 0; i<d; ++i)
	{
		for (int j = 0; j<d; ++j)
		{
			if (i <= j)
			{
				U[i*d + j] = LU[i*d + j]; // We are below the diagonal.
			}
			else
			{
				U[i*d + j] = 0.0;
			}
		}
	}

} // End of separateDoolittleLandU.



void ITPhysics::TwoStageGaussianEliminationCPU(float *L, float *U, int noOfPanels, std::vector <float> *solPointer)
{

	// ======================================================================================================================
	// Do Dom's CPU Gaussian Elimination algorithm. http://www.it.uom.gr/teaching/linearalgebra/NumericalRecipiesInC/c2-2.pdf
	// Consider:
	// Ax = b, and solve for x.
	// A = LU, so we need to solve for LUx = b, which is L(Ux) = b.
	// Defining unknow y as y = Ux, it follows that we can find x in two stages.
	// First solve Ly = b for y.
	// Then solve Ux = y for x.
	// ======================================================================================================================

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "TwoStageGaussianEliminationCPU 1");

	// First stage.
	float *y;
	y = (float *)malloc(noOfPanels * sizeof(float));

	y[0] = project->get_B()->at(0) / L[0];

	for (int i = 1; i<noOfPanels; i++)
	{

		float sum = 0.0;

		for (int j = 0; j<i; j++)
		{
			sum = sum + L[i*noOfPanels + j] * y[j];
		}

		y[i] = (1.0 / L[i*noOfPanels + i]) * (project->get_B()->at(i) - sum);

	}

	//for (int i=0; i<noOfPanels; i++)
	//{
	//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Dom's y[ %i ] = %f", i, y[i]);
	//}


	// Second stage.
	float *x;
	x = (float *)malloc(noOfPanels * sizeof(float));

	x[noOfPanels - 1] = y[noOfPanels - 1] / U[noOfPanels*(noOfPanels - 1) + (noOfPanels - 1)];

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "solPointer size = %i", solPointer->size());


	solPointer->at(noOfPanels - 1) = x[noOfPanels - 1];

	for (int i = noOfPanels - 2; i >= 0; i--) // Step through the rows from penultimate to zeroth.
	{
		float sum = 0.0;
		for (int j = noOfPanels - 1; j>i; j--)
		{
			sum = sum + U[i*noOfPanels + j] * x[j];
		}

		x[i] = (1.0 / U[i*noOfPanels + i]) * (y[i] - sum);



		// Assign the persistent data.
		solPointer->at(i) = x[i];

	}



	for (int i = 0; i<noOfPanels; i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Dom's solPointer[ %i ] = %f", i, solPointer->at(i));
	}

	free(x);
	free(y);

} // End of TwoStageGaussianEliminationCPU.










void ITPhysics::PropagatePressureDistributionAndForces()
{
	// Revision history. 
	// 20161102: Svendsen vorticity is multiplied by 4 PI before performing K & P pressure computation.
	// Called once every frame from PropagateFlexit.











	bool IsKatzAndPlotkin = true; // Use Katz and Plotkin equation 12.25 etc.

	if (IsKatzAndPlotkin)
	{

		// Calculate the lift on each panel and sum for the whole surface.
		for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through all the surfaces.
		{
			float totalSurfaceLift = 0.0;
			float totalSurfaceForceX = 0.0;
			float totalSurfaceForceY = 0.0;
			float totalSurfaceForceZ = 0.0;
			float totalSurfaceArea = 0.0;

			for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
			{
				for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
				{

					ITPanel *p = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

					float panelLift = 0.0;

					if (!(p->get_IgnoreForces()))
					{


						// Find the magnitude of the panel mid point velocity 
						// (and assume the earth velocity of the vortex is the same 
						// as that of the colocation point).
						ITPoint *ev = p->get_MyMidPointEarthVelocity();

						ITVortex *v = p->get_MyVortices()->at(1);

						// Find the vector from the start of the vortex to the end of the vortex.
						ITPoint *vortexVector = new ITPoint(v->get_EndPoint()->get_X() - v->get_StartPoint()->get_X(),
							v->get_EndPoint()->get_Y() - v->get_StartPoint()->get_Y(),
							v->get_EndPoint()->get_Z() - v->get_StartPoint()->get_Z());

						ITPoint * result = new ITPoint(0.0, 0.0, 0.0);
						ev->cross(vortexVector, result);
						float mag = result->magnitude(); // This is V cross L.

						if (j == 0)
						{
							// K&P 12.25a. ( * 4 * PI)
							//							panelLift = panelLift + project->get_Rho() * mag * p->get_MyVorticity() * 4.0 * PI;
							panelLift = project->get_Rho() * ev->get_X() * p->get_MyVorticity() * vortexVector->get_Y() * 4.0 * PI;
						}
						else
						{
							// K&P 12.25. ( * 4 * PI)
							//							panelLift = panelLift + project->get_Rho() * mag * (p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j-1)->get_MyVorticity()) * 4.0 * PI;
							panelLift = project->get_Rho() * ev->get_X() * (p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1)->get_MyVorticity()) * vortexVector->get_Y()  * 4.0 * PI;
						}


						delete result;
						delete vortexVector;

						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "p->get_MyVorticity(): %f", p->get_MyVorticity());

						// Now compute pressure difference Delta P using Katz and Plotkin equation 12.26.
						p->set_DeltaPressure(panelLift / p->get_Area());

						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "i: %i, j: %i, lift: %f, area: %f", i, j, p->get_MyVorticity(), p->get_Area());

						// Now compute the total force cartesian coordinates.
						totalSurfaceForceX = totalSurfaceForceX + panelLift*p->get_Normal()->get_X();
						totalSurfaceForceY = totalSurfaceForceY + panelLift*p->get_Normal()->get_Y();
						totalSurfaceForceZ = totalSurfaceForceZ + panelLift*p->get_Normal()->get_Z();

						totalSurfaceLift = totalSurfaceLift + panelLift;

						totalSurfaceArea = totalSurfaceArea + p->get_Area();

					} // End of if !IgnoreForces.

				} // j panel loop
			} // i panel loop


			  // Debugging print of surface lift coefficient.
			ITPoint *averageSurfaceVelocity = project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).at(0)->get_MyMidPointEarthVelocity();
			float averageSurfaceSpeedSquared = averageSurfaceVelocity->get_X()*averageSurfaceVelocity->get_X()
				+ averageSurfaceVelocity->get_Y()*averageSurfaceVelocity->get_Y()
				+ averageSurfaceVelocity->get_Z()*averageSurfaceVelocity->get_Z();

			float CL = totalSurfaceLift / (0.5 * project->get_Rho() * averageSurfaceSpeedSquared * totalSurfaceArea);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface velocity x: %f", averageSurfaceVelocity->get_X());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface speed: %f", sqrt(averageSurfaceSpeedSquared));
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface Lift: %f", totalSurfaceLift);
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface Area: %f", totalSurfaceArea);
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface CL: %f", CL);



			// Format output to ui.
			if (MY_RUN_MODE == MYGUI)
			{
				QString msg1 = QString("Surface: %1, CL: %2").arg(QString::number(k)).arg(QString::number(CL));
				w->appendDataViewTextEdit(msg1);
			}

			// Set ITSurface instance variable.
			project->get_MySurfaces()->at(k)->get_MyCLHistory()->push_back(CL);

			// Set ITSurface instance variable.
			ITPoint *f = new ITPoint(totalSurfaceForceX, totalSurfaceForceY, totalSurfaceForceZ);
			project->get_MySurfaces()->at(k)->get_MyForceHistory()->push_back(f);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "========================= k: %i, Force History z: %f", k, project->get_MySurfaces()->at(k)->get_MyForceHistory()->back()->get_Z());

		} // k surface loop.













		bool IsDrag = true;

		if (IsDrag)
		{
			// Calculate the drag on each panel and sum for the whole surface.
			// Start by creating the sol vector.

			std::vector <float> *sol = new std::vector <float>;

			for (int k = 0; k<project->get_MySurfaces()->size(); k++)
			{

				for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
				{

					for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++) // Walk along chord from leading edge to trailing edge.
					{

						sol->push_back(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVorticity());

					}

				}

			}

			// Now move the matrix _B to memory using ROW_MAJOR_IDX2C.
			int noOfUnknownVortexStrengths = 0; // Initialize r. This is just the number of surface panels.
			for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through all the surfaces, adding the number of panels to r.
			{
				noOfUnknownVortexStrengths = noOfUnknownVortexStrengths + (project->get_MySurfaces()->at(k)->get_MyPanels()->size()  *  project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size());
			}

			float* B = 0;
			B = (float *)malloc(noOfUnknownVortexStrengths * noOfUnknownVortexStrengths * sizeof(*B));

			for (int i = 0; i<noOfUnknownVortexStrengths; i++) // Loop over the rows of vv_B.
			{
				for (int j = 0; j<noOfUnknownVortexStrengths; j++) // Loop over the columns of vv_B.
				{
					B[ROW_MAJOR_IDX2C(i, j, noOfUnknownVortexStrengths)] = (float)(project->get_A()->at(i).at(j));
				}
			}

			float* S = 0;
			S = (float *)malloc(noOfUnknownVortexStrengths * sizeof(*S));

			for (int i = 0; i<noOfUnknownVortexStrengths; i++) // Loop over the rows of vv_B.
			{
				S[i] = sol->at(i);
			}

			// Now initialise W_{ind}
			float* W = 0;
			W = (float *)malloc(noOfUnknownVortexStrengths * sizeof(*W));

			for (int i = 0; i<noOfUnknownVortexStrengths; i++) // Loop over the rows of vv_B.
			{
				W[i] = 0.0;
			}

			// Now multiply the matrices B and S to get W_{ind}. W_{ind} = B*S;
			multiplyArbitraryMatrices(noOfUnknownVortexStrengths, noOfUnknownVortexStrengths, 'R', B, noOfUnknownVortexStrengths, 1, 'R', S, W);

			//printDebugMatrix(noOfUnknownVortexStrengths, noOfUnknownVortexStrengths, 'R', B, "B:", 2);
			//printDebugMatrix(noOfUnknownVortexStrengths, 1, 'R', S, "S:", 2);
			//printDebugMatrix(noOfUnknownVortexStrengths, 1, 'R', W, "W:", 2);

			// Free memory.
			free(B);
			free(S);

			delete sol;

			// Now that we have W, we can compute the drag using Katz and Plotkin equation 12.27.
			int index = 0;
			for (int k = 0; k<project->get_MySurfaces()->size(); k++)
			{
				float totalSurfaceDrag = 0.0;
				float totalSurfaceArea = 0.0;

				for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
				{

					for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++) // Walk along chord from leading edge to trailing edge.
					{

						ITPanel *currentPanel = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

						float length = currentPanel->get_BottomRightPoint()->distanceFrom(currentPanel->get_TopRightPoint());

						float deltaD = 0.0;

						if (j == 0)
						{
							//  * 4 * PI
							deltaD = -project->get_Rho() * W[index] * currentPanel->get_MyVorticity() * length * 4.0 * PI;
						}
						else
						{
							// 
							//  * 4 * PI
							ITPanel *lastPanel = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1);
							deltaD = -project->get_Rho() * W[index] * (currentPanel->get_MyVorticity() - lastPanel->get_MyVorticity()) * length * 4.0 * PI;
						}


						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "i: %i, j: %i, deltaD: %f", i, j, deltaD);

						totalSurfaceDrag = totalSurfaceDrag + deltaD;
						totalSurfaceArea = totalSurfaceArea + currentPanel->get_Area();

						index++;
					}
				}

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, D: %f", k, totalSurfaceDrag);


				// Compute Drag coefficient.

				ITPoint *averageSurfaceVelocity = project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).at(0)->get_MyMidPointEarthVelocity();
				float averageSurfaceSpeedSquared = averageSurfaceVelocity->get_X()*averageSurfaceVelocity->get_X()
					+ averageSurfaceVelocity->get_Y()*averageSurfaceVelocity->get_Y()
					+ averageSurfaceVelocity->get_Z()*averageSurfaceVelocity->get_Z();
				float CD = totalSurfaceDrag / (0.5 * project->get_Rho() * averageSurfaceSpeedSquared * totalSurfaceArea);

				project->get_MySurfaces()->at(k)->get_MyCDHistory()->push_back(CD);

				// Format output to ui.
				if (MY_RUN_MODE == MYGUI)
				{
					QString msg1 = QString("Surface: %1, CD: %2").arg(QString::number(k)).arg(QString::number(CD));
					w->appendDataViewTextEdit(msg1);
				}

			} // End of loop over surfaces.

			free(W);

		} // End of if IsDrag.
		else
		{
			for (int k = 0; k<project->get_MySurfaces()->size(); k++)
			{
				project->get_MySurfaces()->at(k)->get_MyCDHistory()->push_back(0.0);
			}
		}



	} // End of IsKatzAndPlotkin.

} // End of PropagatePressureDistributionAndForces












void ITPhysics::PropagateSurfaceGeometry(int FrameNumber)
{
	// Propagate the surface geometries.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		//============================================================================================
		// Compute new surface, panel and bound vortex segment geometry for this frame.
		// The vorticities must be saved from old panels and copied to new panels.
		//============================================================================================
		project->get_MySurfaces()->at(k)->propagateGeometry(k);
	}

	drawTranslationCompleteSemaphore = false;

} // End of PropagateSurfaceGeometry.


void ITPhysics::InsertWakePanels(int FrameNumber)
{

	// Propagate the wake geometries.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		//============================================================================================
		// Compute new wake geometry for this frame.
		//============================================================================================
		project->get_MySurfaces()->at(k)->insertWakePanels();
	}

} // End of InsertWakePanels.


void ITPhysics::PropagateWakeGeometry(int FrameNumber)
{

	// Propagate the wake along the local streamline.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		project->get_MySurfaces()->at(k)->propagateWakeGeometryCuda(k);
	}

} // End of PropagateWakeGeometry.





void ITPhysics::calcVelocityFieldCuda(int t)
{

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside calcVelocityFieldCuda");


	// noOfSubjectPanels is the total number of panels in the project
	// (both on the surface and in the wake, on all surfaces, including reflected ones).
	int noOfSubjectPanels = 0; // Initialize noOfPanels.
	for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // Loop through all the surfaces.
	{
		// Add the number of bound surface panels to noOfSubjectPanels
		for (int ii = 0; ii<project->get_MySurfaces()->at(kk)->get_MyPanels()->size(); ii++)
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).size(); jj++)
			{
				// Add surface subject panels.
				noOfSubjectPanels++;
			}
		}


		// Add the number of wake panels to noOfSubjectPanels.
		// This takes account of surfaces without wakes.
		int lastGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size();
		for (int ii = 0; ii<lastGeneration; ii++) // Loop over generations.
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).size(); jj++)
			{
				// Add wake subject panels.
				noOfSubjectPanels++;
			}
		}

	}


	// Empty any existing velocity field data.
	if (!project->get_VelocityFieldData()->empty())
	{
		for (int i = 0; i<project->get_VelocityFieldData()->size(); i++)
		{
			for (int j = 0; j<project->get_VelocityFieldData()->at(i).size(); j++)
			{
				delete project->get_VelocityFieldData()->at(i).at(j);
			}

			project->get_VelocityFieldData()->at(i).clear();
		}
		project->get_VelocityFieldData()->clear();
	}



	// Calculate a grid of flow vectors at some fixed plane.
	int nx = project->get_VelocityFieldNx();
	int ny = project->get_VelocityFieldNy();
	int nz = project->get_VelocityFieldNz();

	float minx = project->get_VelocityFieldMinx();
	float miny = project->get_VelocityFieldMiny();
	float minz = project->get_VelocityFieldMinz();

	float maxx = project->get_VelocityFieldMaxx();
	float maxy = project->get_VelocityFieldMaxy();
	float maxz = project->get_VelocityFieldMaxz();

	float dx = (maxx - minx) / nx;
	float dy = (maxy - miny) / ny;
	float dz = (maxz - minz) / nz;

	// noOfVelocityPredictions is the number of points at which we require a prediction of the velocity.   
	int noOfVelocityPredictions = 0; // The total number of velocity field grid points at this generation t.

									 // Actually generate the point geometry.
	if (project->get_VelocityFieldConstantPlane() == std::string("X"))
	{
		float x = (project->get_VelocityFieldMinx() + project->get_VelocityFieldMaxx()) / 2.0;
		for (int rowIndex = 0; rowIndex<nz; rowIndex++)
		{
			std::vector <ITPoint*> dummy_rowFiledPointsVector;

			float z = minz + rowIndex*dz;

			for (int colIndex = 0; colIndex<ny; colIndex++)
			{

				float y = miny + colIndex*dy;

				// Instanciate the seed point.
				ITPoint* s = new ITPoint(x, y, z);

				dummy_rowFiledPointsVector.push_back(s);

				// Increment the number of predictions required.
				noOfVelocityPredictions++;

			} // End of colIndex loop.

			project->get_VelocityFieldData()->push_back(dummy_rowFiledPointsVector);

		} // End of rowIndex loop.
	}
	else if (project->get_VelocityFieldConstantPlane() == std::string("Y"))
	{
		float y = (project->get_VelocityFieldMiny() + project->get_VelocityFieldMaxy()) / 2.0;
		for (int rowIndex = 0; rowIndex<nz; rowIndex++)
		{
			std::vector <ITPoint*> dummy_rowFiledPointsVector;

			float z = minz + rowIndex*dz;

			for (int colIndex = 0; colIndex<nx; colIndex++)
			{

				float x = minx + colIndex*dx;

				// Instanciate the seed point.
				ITPoint* s = new ITPoint(x, y, z);

				dummy_rowFiledPointsVector.push_back(s);

				// Increment the number of predictions required.
				noOfVelocityPredictions++;

			} // End of colIndex loop.

			project->get_VelocityFieldData()->push_back(dummy_rowFiledPointsVector);

		} // End of rowIndex loop.
	}
	else if (project->get_VelocityFieldConstantPlane() == std::string("Z"))
	{
		float z = (project->get_VelocityFieldMinz() + project->get_VelocityFieldMaxz()) / 2.0;
		for (int rowIndex = 0; rowIndex<nx; rowIndex++)
		{
			std::vector <ITPoint*> dummy_rowFiledPointsVector;

			float x = minx + rowIndex*dx;

			for (int colIndex = 0; colIndex<ny; colIndex++)
			{

				float y = miny + colIndex*dy;

				// Instanciate the seed point.
				ITPoint* s = new ITPoint(x, y, z);

				dummy_rowFiledPointsVector.push_back(s);

				// Increment the number of predictions required.
				noOfVelocityPredictions++;

			} // End of colIndex loop.

			project->get_VelocityFieldData()->push_back(dummy_rowFiledPointsVector);

		} // End of rowIndex loop.
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "============================= VelocityField noOfVelocityPredictions: %i", noOfVelocityPredictions);

	// Now put the object data into host memory arrays, ready for copying to the GPU device.

	// DOM: Set up allocation of arrays of panels and vertices in host memory.
	size_t sizeOfSubjectPanelsFloat = noOfSubjectPanels * sizeof(float); // Memory required for noOfPanels floats.
	size_t sizeOfSubjectPanelsInt = noOfSubjectPanels * sizeof(int); // Memory required for noOfPanels ints.
	size_t sizeVelocityPredictionsFloat = noOfVelocityPredictions * sizeof(float); // Memory required for noOfVelocityPredictions floats. This is for each (normalised) velocity component.

																				   // Allocate memory for Kernel input data on the host.
																				   // Coordinates of points at which velocities are to be computed.
	float *h_cp_x = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point x-coordinate.
	float *h_cp_y = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point y-coordinate.
	float *h_cp_z = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point z-coordinate.

																   // Subject Vortex start point coordinates.
	int noOfVorticesPerPanel = 4;
	float *h_vs_x = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point x-coord.
	float *h_vs_y = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point y-coord.
	float *h_vs_z = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point z-coord.

																					// Subject Vortex end point coordinates.
	float *h_ve_x = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point x-coord.
	float *h_ve_y = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point y-coord.
	float *h_ve_z = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point z-coord.

																					// Allocate memory for vorticity values for each subject panel.
	float *h_vorticities = (float *)malloc(sizeOfSubjectPanelsFloat);

	// Allocate the host output vectors of velocity components for each object point.
	float *h_cp_vx = (float *)malloc(sizeVelocityPredictionsFloat);
	float *h_cp_vy = (float *)malloc(sizeVelocityPredictionsFloat);
	float *h_cp_vz = (float *)malloc(sizeVelocityPredictionsFloat);

	// Verify that allocations succeeded.
	if (h_cp_x == NULL
		|| h_cp_y == NULL
		|| h_cp_z == NULL
		|| h_vs_x == NULL
		|| h_vs_y == NULL
		|| h_vs_z == NULL
		|| h_ve_x == NULL
		|| h_ve_y == NULL
		|| h_ve_z == NULL
		|| h_vorticities == NULL
		|| h_cp_vx == NULL
		|| h_cp_vy == NULL
		|| h_cp_vz == NULL)
	{
		fprintf(stderr, "Failed to allocate host vectors!\n");
		exit(EXIT_FAILURE);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host memory allocated successfully.");

	// Now initialize the host array of object point velocity components (output).
	for (int i = 0; i<noOfVelocityPredictions; i++)
	{
		h_cp_vx[i] = 0.0f;
		h_cp_vy[i] = 0.0f;
		h_cp_vz[i] = 0.0f;
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host object point velocities initialized successfully.");

	// Now initialize the host arrays of object point position
	// coordinates (the windward and leeward panel mid-points for all the surfaces).

	int objectPointIndex = 0; // DOM: Object point index.

	for (int i = 0; i<project->get_VelocityFieldData()->size(); i++)
	{
		for (int j = 0; j<project->get_VelocityFieldData()->at(i).size(); j++)
		{

			// DOM: Assign object point coordinates.	
			h_cp_x[objectPointIndex] = project->get_VelocityFieldData()->at(i).at(j)->get_X();
			h_cp_y[objectPointIndex] = project->get_VelocityFieldData()->at(i).at(j)->get_Y();
			h_cp_z[objectPointIndex] = project->get_VelocityFieldData()->at(i).at(j)->get_Z();

			objectPointIndex++;

		}
	}

	// Now initialize the vortex segment end point data for all surface subject panels.

	int subjectSurfacePanelIndex = 0; // Panel index (counts over all the surface panels).

									  // Do the bound vortices.
	for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // DOM: Loop through all the surfaces.
	{
		for (int ii = 0; ii<project->get_MySurfaces()->at(kk)->get_MyPanels()->size(); ii++) // DOM: Loop through the rows of panels on each surface.
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).size(); jj++) // DOM: Loop through the columns of panels on each surface.
			{

				// DOM: Vortex data.

				// DOM: Loop through all the bound vortices on the current panel.
				for (int mm = 0; mm<project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->size(); mm++)
				{

					// DOM: Vortex coordinates.
					h_vs_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_X();
					h_vs_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Y();
					h_vs_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Z();

					h_ve_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_X();
					h_ve_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Y();
					h_ve_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Z();


					// Check for vortices of zero length.
					float epsilon = 0.0001;
					if ((fabs(h_vs_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon))
					{
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Zero length bound vortex detected.");
					}


				} // End of mm loop.

				  // DOM: Host vorticity assignments

				h_vorticities[subjectSurfacePanelIndex] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVorticity();

				// Increment panel index.
				subjectSurfacePanelIndex++;

			} // End of jj loop.

		} // End of ii loop.

	} // End of kk loop.

	  // Now initialize the vortex segment end point data for wake subject panels.

	int subjectWakePanelIndex = subjectSurfacePanelIndex; // Panel index (counts over all the wake panels).

														  // Do the free (wake) vortices.
	for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // DOM: Loop through all the surfaces.
	{
		int lastGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size();

		for (int ii = 0; ii<lastGeneration; ii++) // DOM: Loop through the generations of wake panels of the current surface.
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).size(); jj++) // DOM: Loop through the columns of panels on each surface.
			{

				// DOM: Vortex data.

				// DOM: Loop through all the bound vortices on the current panel.
				for (int mm = 0; mm<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->size(); mm++)
				{

					// DOM: Vortex coordinates.
					h_vs_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_X();
					h_vs_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Y();
					h_vs_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Z();

					h_ve_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_X();
					h_ve_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Y();
					h_ve_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Z();


					// Check for vortices of zero length.
					float epsilon = 0.0001;
					if ((fabs(h_vs_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon))
					{
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Zero length wake vortex detected.");
					}

				} // End of for mm loop.

				  // DOM: Host vorticity assignments

				h_vorticities[subjectWakePanelIndex] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVorticity();

				// Increment panel index.
				subjectWakePanelIndex++;

			} // End of jj loop.

		} // End of ii loop.

	} // End of kk loop.

	// =======================================================================================================================
	// DOM: Copy data from host to device, do Cuda calculations, and copy data back to host. (CudaCode.cu)
	// =======================================================================================================================
	ComputeVelocitiesForBatchOfPointsCuda(
		h_cp_x,
		h_cp_y,
		h_cp_z,
		h_vs_x,
		h_vs_y,
		h_vs_z,
		h_ve_x,
		h_ve_y,
		h_ve_z,
		h_cp_vx,
		h_cp_vy,
		h_cp_vz,
		h_vorticities,
		noOfVorticesPerPanel,
		noOfSubjectPanels,
		noOfVelocityPredictions,
		project->get_RankineAlgorithm().at(0));




	// Extract the velocity data from h_cp_vx, h_cp_vy and h_cp_vz.
	objectPointIndex = 0; // DOM: Object point index.

						  // Store the data in the velocity field ITPoint instance variables.
	for (int i = 0; i<project->get_VelocityFieldData()->size(); i++)
	{
		for (int j = 0; j<project->get_VelocityFieldData()->at(i).size(); j++)
		{

			// Check if we should add gust velocity.
			float gustVx = 0.0;
			float gustVy = 0.0;
			float gustVz = 0.0;

			if (project->get_IsGust())
			{

				project->get_MyGust()->computeVelocityAtPoint(h_cp_x[objectPointIndex], h_cp_y[objectPointIndex], h_cp_z[objectPointIndex], &gustVx, &gustVy, &gustVz);

			}

			// DOM: Assign object point velocity components.
			project->get_VelocityFieldData()->at(i).at(j)->set_VX(h_cp_vx[objectPointIndex] + gustVx);
			project->get_VelocityFieldData()->at(i).at(j)->set_VY(h_cp_vy[objectPointIndex] + gustVy);
			project->get_VelocityFieldData()->at(i).at(j)->set_VZ(h_cp_vz[objectPointIndex] + gustVz);

			objectPointIndex++;
		}
	}



	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Sample velocity: vx: %f, vy: %f, vz: %f", project->get_VelocityFieldData()->at(0).at(0)->get_VX(),
		project->get_VelocityFieldData()->at(0).at(0)->get_VY(),
		project->get_VelocityFieldData()->at(0).at(0)->get_VZ());

	// Free host memory.
	free(h_cp_x);
	free(h_cp_y);
	free(h_cp_z);

	free(h_vs_x);
	free(h_vs_y);
	free(h_vs_z);

	free(h_ve_x);
	free(h_ve_y);
	free(h_ve_z);

	free(h_cp_vx);
	free(h_cp_vy);
	free(h_cp_vz);

	free(h_vorticities);

} // End of calcVelocityFieldCuda.






void ITPhysics::StoreDataForReplay()
{

	// Store wake panel geometry snapshot.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{

		std::vector < std::vector <ITWakePanel*> > *dummyVectorOfVectorsOfWakePanels = new std::vector < std::vector <ITWakePanel*> >;

		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyWakePanels()->size(); i++)
		{

			std::vector <ITWakePanel*> *dummyVectorOfWakePanels = new std::vector <ITWakePanel*>;

			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).size(); j++)
			{
				ITPoint *bl = new ITPoint(project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_BottomLeftPoint()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_BottomLeftPoint()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_BottomLeftPoint()->get_Z());

				ITPoint *br = new ITPoint(project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_BottomRightPoint()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_BottomRightPoint()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_BottomRightPoint()->get_Z());

				ITPoint *tr = new ITPoint(project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_TopRightPoint()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_TopRightPoint()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_TopRightPoint()->get_Z());

				ITPoint *tl = new ITPoint(project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_TopLeftPoint()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_TopLeftPoint()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_TopLeftPoint()->get_Z());

				ITWakePanel *p = new ITWakePanel(bl, br, tr, tl, project->get_MySurfaces()->at(k), 1);

				p->set_K(k);
				p->set_I(i);
				p->set_J(j);
				p->set_F(FrameNumber);

				p->set_MyVorticity(project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j)->get_MyVorticity());

				dummyVectorOfWakePanels->push_back(p);

			}

			dummyVectorOfVectorsOfWakePanels->push_back(*dummyVectorOfWakePanels);

		}

		// Store a snapshot of data.
		project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->push_back(*(dummyVectorOfVectorsOfWakePanels));

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside StoreDataForReplay. Number of layers: %i", project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->size());
	} // End of loop through surfaces.


	  // Now store surface control point deflections.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{

		std::vector < std::vector <ITPoint*> > *dummyVectorOfVectorsOfControlPointDeflections = new std::vector < std::vector <ITPoint*> >;

		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{

			std::vector <ITPoint*> *dummyVectorOfControlPointDeflections = new std::vector <ITPoint*>;

			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITPoint *pW = new ITPoint(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_Z());
				pW->set_VX(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_VX());
				pW->set_VY(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_VY());
				pW->set_VZ(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_W()->get_VZ());

				dummyVectorOfControlPointDeflections->push_back(pW);

			}

			dummyVectorOfVectorsOfControlPointDeflections->push_back(*dummyVectorOfControlPointDeflections);

		}

		// Store a snapshot of data.
		project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->push_back(*(dummyVectorOfVectorsOfControlPointDeflections));

	} // End of loop through surfaces.









	  // Now store surface pressure distribution snapshot.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{

		std::vector < std::vector <float> > *dummyVectorOfVectorsOfPressures = new std::vector < std::vector <float> >;

		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{

			std::vector <float> *dummyVectorOfPressures = new std::vector <float>;

			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				dummyVectorOfPressures->push_back(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_DeltaPressure());
			}

			dummyVectorOfVectorsOfPressures->push_back(*dummyVectorOfPressures);

		}

		// Store a snapshot of data.
		project->get_MySurfaces()->at(k)->get_MyPressureHistory()->push_back(*(dummyVectorOfVectorsOfPressures));

	} // End of loop over surfaces.


	  // Now store surface vorticity distribution snapshot.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{

		std::vector < std::vector <float> > *dummyVectorOfVectorsOfVorticities = new std::vector < std::vector <float> >;

		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{

			std::vector <float> *dummyVectorOfVorticities = new std::vector <float>;

			for (int j = 0; j<project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				dummyVectorOfVorticities->push_back(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MyVorticity());
			}

			dummyVectorOfVectorsOfVorticities->push_back(*dummyVectorOfVorticities);

		}

		// Store a snapshot of data.
		project->get_MySurfaces()->at(k)->get_MyVorticityHistory()->push_back(*(dummyVectorOfVectorsOfVorticities));

	} // End of loop over surfaces.


	  // Now store beam deflection snapshot.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{

		std::vector <ITBeamNode*> *dummyVectorOfNodes = new std::vector <ITBeamNode*>;

		for (int i = 0; i<project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->size(); i++)
		{
			// Create history object.
			ITBeamNode* n = new ITBeamNode(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_X(),
				project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_Y(),
				project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_Z());

			n->set_Mass(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_Mass());

			n->set_E(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_E()); // Aerospace grade aluminium alloy.

			n->set_Ix(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_Ix());
			n->set_Iz(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_Iz());

			n->get_W()->set_X(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_W()->get_X());
			n->get_W()->set_Y(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_W()->get_Y());
			n->get_W()->set_Z(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->at(i)->get_W()->get_Z());

			// Push the new ITBeamNode onto the dummy vector.
			dummyVectorOfNodes->push_back(n);
		}

		// Store a snapshot of data.
		project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->push_back(*(dummyVectorOfNodes));


	} // End of loop over surfaces.






	  // Now store velocity field snapshot.
	std::vector < std::vector <ITPoint*> > *dummyVectorOfVectorsOfVelocities = new std::vector < std::vector <ITPoint*> >;

	for (int i = 0; i<project->get_VelocityFieldData()->size(); i++)
	{

		std::vector <ITPoint*> *dummyVectorOfVelocities = new std::vector <ITPoint*>;

		for (int j = 0; j<project->get_VelocityFieldData()->at(i).size(); j++)
		{
			ITPoint* v = new ITPoint(project->get_VelocityFieldData()->at(i).at(j)->get_X(),
				project->get_VelocityFieldData()->at(i).at(j)->get_Y(),
				project->get_VelocityFieldData()->at(i).at(j)->get_Z());
			v->set_VX(project->get_VelocityFieldData()->at(i).at(j)->get_VX());
			v->set_VY(project->get_VelocityFieldData()->at(i).at(j)->get_VY());
			v->set_VZ(project->get_VelocityFieldData()->at(i).at(j)->get_VZ());

			dummyVectorOfVelocities->push_back(v);
		}

		dummyVectorOfVectorsOfVelocities->push_back(*dummyVectorOfVelocities);

	}

	project->get_VelocityFieldDataHistory()->push_back(*dummyVectorOfVectorsOfVelocities);


	// Store control surface deflection history.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++) // Loop through the surfaces.
	{
		for (int g = 0; g<project->get_MySurfaces()->at(k)->get_MyControlSurfaces()->size(); g++)
		{

			project->get_MySurfaces()->at(k)->get_MyControlSurfaces()->at(g)->get_MyDeflectionHistory()->push_back(project->get_MySurfaces()->at(k)->get_MyControlSurfaces()->at(g)->get_DeflectionAngle());

		} // End of loop over control surfaces.

	} // End of loop over surfaces.


} // End of StoreDataForReplay.

















void ITPhysics::playOutFlexit()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside playOutFlexit.");

	// Prepare for a simulation (empty and delete wake panels and velocity field and do other things).
	if (FrameNumber == 0)
	{
		prepareForSimulation();
	}

	while ((FrameNumber < project->get_MaxKeyFrame()) && (!IsPaused) && (!IsReplay) && (!IsDryRun) && (!IsStep) && (!IsTrim) && (IsSimulating))
	{

		UnsavedChanges = true;

		if (MY_RUN_MODE == MYGUI)
		{
			w->statusBar()->showMessage(QString("Calculation in Progress. Frame: %1 of: %2. Total computation time: %3 secs.").arg(FrameNumber).arg(project->get_MaxKeyFrame()).arg(cummulativeElapsedTimeSeconds));
		}

		LARGE_INTEGER t1, t2, frequency;
		double elapsedTime;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&t1);

		ITPhysics::PropagateFlexit(FrameNumber);

		QueryPerformanceCounter(&t2);
		elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Time for frame %i is %f, msecs", FrameNumber, elapsedTime);

		cummulativeElapsedTimeSeconds = cummulativeElapsedTimeSeconds + elapsedTime*0.001;

		// Update the main widget.
		if (MY_RUN_MODE == MYGUI)
		{
			w->updateAllTabsForCurrentFrameNumber();
			ITPhysics::delay(10); // 10 msecs for rendering the current Frame.
		}


		// Finally increment the frame number.
		FrameNumber++;

	} // End of while FrameNumber < project->get_MaxKeyFrame()


} // End of playOutFlexit






void ITPhysics::playOutDryRun()
{

	// At the start of the dry run, we move points back to base.
	if (FrameNumber == 0)
	{
		for (int k = 0; k<project->get_MySurfaces()->size(); k++)
		{
			project->get_MySurfaces()->at(k)->moveMeBackToBase(k);
		} // k
	}

	while ((FrameNumber < project->get_MaxKeyFrame()) && (!IsPaused) && (!IsSimulating) && (!IsReplay) && (IsDryRun)) // Note that we do NOT require !IsStep here. This allows us to dry run after stepping.
	{

		if (MY_RUN_MODE == MYGUI)
		{
			w->statusBar()->showMessage(QString("Dry run in Progress. Frame: %1 of %2").arg(FrameNumber).arg(project->get_MaxKeyFrame()));
		}

		ITPhysics::PropagateSurfaceGeometry(FrameNumber);

		// Update the main widget.
		if (MY_RUN_MODE == MYGUI)
		{
			w->updateAllTabsForCurrentFrameNumber();
			ITPhysics::delay(10); // 10 msecs for rendering the current Frame.
		}

		FrameNumber++;
	}

} // End of playOutDryRun.




void ITPhysics::playOutReplay()
{

	// At the start of the replay, we move points back to base.
	if (FrameNumber == 0)
	{
		for (int k = 0; k<project->get_MySurfaces()->size(); k++)
		{
			project->get_MySurfaces()->at(k)->moveMeBackToBase(k);
		} // k
	}


	while ((FrameNumber < project->get_MaxKeyFrame()) && (!IsPaused) && (!IsSimulating) && (!IsStep) && (!IsTrim) && (IsReplay))
	{

		if (MY_RUN_MODE == MYGUI)
		{
			w->statusBar()->showMessage(QString("Replay in Progress. Frame: %1 of %2").arg(FrameNumber).arg(project->get_MaxKeyFrame()));
		}


		// Apply the deformations.
//		ApplyReplayDeformations();

		ITPhysics::PropagateSurfaceGeometry(FrameNumber);

		// Update the main widget.
		if (MY_RUN_MODE == MYGUI)
		{
			w->updateAllTabsForCurrentFrameNumber();
			ITPhysics::delay(project->get_ReplayDeltaTMSecs()); // 100 msecs for rendering the current Frame.
		}

		FrameNumber++;
	}

} // End of playOutReplay.




void ITPhysics::playOutTest()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside playOutTest.");


	bool Is20170203 = true;
	if (Is20170203)
	{
		// 20170203 =====================================================================================
		// Compute surface fluid-dynamic forces using Lorentz.
		// 20170203 =====================================================================================

		// Calculate the forces on each vortex line segment and sum to get the total force vector on the whole surface.
		for (int k = 0; k < project->get_MySurfaces()->size(); k++) // Loop through all the surfaces.
		{
			float totalSurfaceForceX = 0.0;
			float totalSurfaceForceY = 0.0;
			float totalSurfaceForceZ = 0.0;
			float totalSurfaceArea = 0.0;

			int noOfRows = project->get_MySurfaces()->at(k)->get_MyPanels()->size();
			int noOfCols = project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size();

			float x, y, z, vx, vy, vz, xL, yL, zL, zeta;
			float Fx, Fy, Fz;

			ITPoint *inducedVelocity = new ITPoint(0.0, 0.0, 0.0);
			ITPoint *totalVelocity = new ITPoint(0.0, 0.0, 0.0);
			ITPoint *vortexVector = new ITPoint(0.0, 0.0, 0.0);
			ITPoint *T = new ITPoint(0.0, 0.0, 0.0);

			for (int i = 0; i < noOfRows; i++)
			{
				for (int j = 0; j < noOfCols; j++)
				{
					// Go through the zones.
					int zone = 0;
					if ((i == 0) && (j == 0)) { zone = 1; }
					else if ((i == 0) && (j > 0) && (j < noOfCols - 1)) { zone = 2; }
					else if ((i == 0) && (j == noOfCols - 1)) { zone = 3; }
					else if ((i > 0) && (i < noOfRows - 1) && (j == 0)) { zone = 4; }
					else if ((i > 0) && (i < noOfRows - 1) && (j > 0) && (j < noOfCols - 1)) { zone = 5; }
					else if ((i > 0) && (i < noOfRows - 1) && (j == noOfCols - 1)) { zone = 6; }
					else if ((i == noOfRows - 1) && (j == 0)) { zone = 7; }
					else if ((i == noOfRows - 1) && (j > 0) && (j < noOfCols - 1)) { zone = 8; }
					else if ((i == noOfRows - 1) && (j == noOfCols - 1)) { zone = 9; }

					ITPanel *p = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

					// Compute total force on vortices grouped to the current panel.

					// Start with bottom vortex. ============================================================
					ITVortex *bottomVortex = p->get_MyVortices()->at(0);

					x = bottomVortex->get_StartPoint()->get_X() + 0.5 * (bottomVortex->get_EndPoint()->get_X() - bottomVortex->get_StartPoint()->get_X());
					y = bottomVortex->get_StartPoint()->get_Y() + 0.5 * (bottomVortex->get_EndPoint()->get_Y() - bottomVortex->get_StartPoint()->get_Y());
					z = bottomVortex->get_StartPoint()->get_Z() + 0.5 * (bottomVortex->get_EndPoint()->get_Z() - bottomVortex->get_StartPoint()->get_Z());

					// Calculate the velocity induced by all vortices at the mid-point of this edge.
					ITPhysics::calcInducedVelocityAtEdgeMidPointCuda(k, i, j, 0, x, y, z, &vx, &vy, &vz);

					// Compute forces.
					inducedVelocity->set_X(vx);
					inducedVelocity->set_Y(vy);
					inducedVelocity->set_Z(vz);

					totalVelocity->set_X(vx - p->get_MyMidPointEarthVelocity()->get_X());
					totalVelocity->set_Y(vy - p->get_MyMidPointEarthVelocity()->get_Y());
					totalVelocity->set_Z(vz - p->get_MyMidPointEarthVelocity()->get_Z());

					xL = bottomVortex->get_EndPoint()->get_X() - bottomVortex->get_StartPoint()->get_X();
					yL = bottomVortex->get_EndPoint()->get_Y() - bottomVortex->get_StartPoint()->get_Y();
					zL = bottomVortex->get_EndPoint()->get_Z() - bottomVortex->get_StartPoint()->get_Z();

					vortexVector->set_X(xL);
					vortexVector->set_Y(yL);
					vortexVector->set_Z(zL);

					totalVelocity->cross(vortexVector, T);

					if (zone == 1) { zeta = p->get_MyVorticity(); }
					else if (zone == 2) { zeta = p->get_MyVorticity(); }
					else if (zone == 3) { zeta = p->get_MyVorticity(); }
					else if (zone == 4) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i - 1).at(j)->get_MyVorticity(); }
					else if (zone == 5) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i - 1).at(j)->get_MyVorticity(); }
					else if (zone == 6) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i - 1).at(j)->get_MyVorticity(); }
					else if (zone == 7) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i - 1).at(j)->get_MyVorticity(); }
					else if (zone == 8) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i - 1).at(j)->get_MyVorticity(); }
					else if (zone == 9) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i - 1).at(j)->get_MyVorticity(); }

					Fx = project->get_Rho() * T->get_X() * zeta;
					Fy = project->get_Rho() * T->get_Y() * zeta;
					Fz = project->get_Rho() * T->get_Z() * zeta;

					totalSurfaceForceX = totalSurfaceForceX + Fx;
					totalSurfaceForceY = totalSurfaceForceY + Fy;
					totalSurfaceForceZ = totalSurfaceForceZ + Fz;










					// Next do right vortex. ============================================================
					ITVortex *rightVortex = p->get_MyVortices()->at(1);

					x = rightVortex->get_StartPoint()->get_X() + 0.5 * (rightVortex->get_EndPoint()->get_X() - rightVortex->get_StartPoint()->get_X());
					y = rightVortex->get_StartPoint()->get_Y() + 0.5 * (rightVortex->get_EndPoint()->get_Y() - rightVortex->get_StartPoint()->get_Y());
					z = rightVortex->get_StartPoint()->get_Z() + 0.5 * (rightVortex->get_EndPoint()->get_Z() - rightVortex->get_StartPoint()->get_Z());

					// Calculate the velocity induced by all vortices at the mid-point of this edge.
					ITPhysics::calcInducedVelocityAtEdgeMidPointCuda(k, i, j, 1, x, y, z, &vx, &vy, &vz);

					// Compute forces.
					inducedVelocity->set_X(vx);
					inducedVelocity->set_Y(vy);
					inducedVelocity->set_Z(vz);

					totalVelocity->set_X(vx - p->get_MyMidPointEarthVelocity()->get_X());
					totalVelocity->set_Y(vy - p->get_MyMidPointEarthVelocity()->get_Y());
					totalVelocity->set_Z(vz - p->get_MyMidPointEarthVelocity()->get_Z());

					xL = rightVortex->get_EndPoint()->get_X() - rightVortex->get_StartPoint()->get_X();
					yL = rightVortex->get_EndPoint()->get_Y() - rightVortex->get_StartPoint()->get_Y();
					zL = rightVortex->get_EndPoint()->get_Z() - rightVortex->get_StartPoint()->get_Z();

					vortexVector->set_X(xL);
					vortexVector->set_Y(yL);
					vortexVector->set_Z(zL);

					totalVelocity->cross(vortexVector, T);

					if (zone == 1) { zeta = p->get_MyVorticity(); }
					else if (zone == 2) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1)->get_MyVorticity(); }
					else if (zone == 3) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1)->get_MyVorticity(); }
					else if (zone == 4) { zeta = p->get_MyVorticity(); }
					else if (zone == 5) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1)->get_MyVorticity(); }
					else if (zone == 6) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1)->get_MyVorticity(); }
					else if (zone == 7) { zeta = p->get_MyVorticity(); }
					else if (zone == 8) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1)->get_MyVorticity(); }
					else if (zone == 9) { zeta = p->get_MyVorticity() - project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j - 1)->get_MyVorticity(); }

					Fx = project->get_Rho() * T->get_X() * zeta;
					Fy = project->get_Rho() * T->get_Y() * zeta;
					Fz = project->get_Rho() * T->get_Z() * zeta;

					totalSurfaceForceX = totalSurfaceForceX + Fx;
					totalSurfaceForceY = totalSurfaceForceY + Fy;
					totalSurfaceForceZ = totalSurfaceForceZ + Fz;


					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "evx: %10.2f, vx: % 10.2f, vy: % 10.2f, vz: % 10.2f, Fx: % 10.2f, Fy: % 10.2f, Fz: % 10.2f", p->get_MyMidPointEarthVelocity()->get_X(), vx, vy, vz, Fx, Fy, Fz);



					// Next do top vortex. ============================================================
					if (i == noOfRows - 1)
					{
						ITVortex *topVortex = p->get_MyVortices()->at(2);

						x = topVortex->get_StartPoint()->get_X() + 0.5 * (topVortex->get_EndPoint()->get_X() - topVortex->get_StartPoint()->get_X());
						y = topVortex->get_StartPoint()->get_Y() + 0.5 * (topVortex->get_EndPoint()->get_Y() - topVortex->get_StartPoint()->get_Y());
						z = topVortex->get_StartPoint()->get_Z() + 0.5 * (topVortex->get_EndPoint()->get_Z() - topVortex->get_StartPoint()->get_Z());

						// Calculate the velocity induced by all vortices at the mid-point of this edge.
						ITPhysics::calcInducedVelocityAtEdgeMidPointCuda(k, i, j, 2, x, y, z, &vx, &vy, &vz);

						// Compute forces.
						inducedVelocity->set_X(vx);
						inducedVelocity->set_Y(vy);
						inducedVelocity->set_Z(vz);

						totalVelocity->set_X(vx - p->get_MyMidPointEarthVelocity()->get_X());
						totalVelocity->set_Y(vy - p->get_MyMidPointEarthVelocity()->get_Y());
						totalVelocity->set_Z(vz - p->get_MyMidPointEarthVelocity()->get_Z());

						xL = topVortex->get_EndPoint()->get_X() - topVortex->get_StartPoint()->get_X();
						yL = topVortex->get_EndPoint()->get_Y() - topVortex->get_StartPoint()->get_Y();
						zL = topVortex->get_EndPoint()->get_Z() - topVortex->get_StartPoint()->get_Z();

						vortexVector->set_X(xL);
						vortexVector->set_Y(yL);
						vortexVector->set_Z(zL);

						totalVelocity->cross(vortexVector, T);

						if (zone == 7) { zeta = p->get_MyVorticity(); }
						else if (zone == 8) { zeta = p->get_MyVorticity(); }
						else if (zone == 9) { zeta = p->get_MyVorticity(); }

						Fx = project->get_Rho() * T->get_X() * zeta;
						Fy = project->get_Rho() * T->get_Y() * zeta;
						Fz = project->get_Rho() * T->get_Z() * zeta;

						totalSurfaceForceX = totalSurfaceForceX + Fx;
						totalSurfaceForceY = totalSurfaceForceY + Fy;
						totalSurfaceForceZ = totalSurfaceForceZ + Fz;

					} // End of if i == noOfRows-1.

				} // End of j loop.

			} // End of i loop.

			  // Multiply by 4.0 * PI
			totalSurfaceForceX = totalSurfaceForceX * 4.0 * PI;
			totalSurfaceForceY = totalSurfaceForceY * 4.0 * PI;
			totalSurfaceForceZ = totalSurfaceForceZ * 4.0 * PI;

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface %i forces x: %f, y: %f, z: %f", k, totalSurfaceForceX, totalSurfaceForceY, totalSurfaceForceZ);

			// Free memory.
			delete inducedVelocity;
			delete totalVelocity;
			delete vortexVector;
			delete T;

		} // End of loop over surfaces.

	} // End of if Is20170203.




} // End of playOutTest.


void ITPhysics::calcInducedVelocityAtEdgeMidPointCuda(int k, int i, int j, int t, float x, float y, float z, float *vx, float *vy, float *vz)
{

	//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside calcInducedVelocityAtEdgeMidPointCuda. k: %i, i: %i, j: %i, t: %i", k, i, j, t);

	// Now put the object data into host memory arrays, ready for copying to the GPU device.

	// noOfSubjectPanels is the total number of panels in the project
	// (both on the surface and in the wake, on all surfaces, including reflected ones).
	int noOfSubjectPanels = 0; // Initialize noOfPanels.
	for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // Loop through all the surfaces.
	{
		// Add the number of bound surface panels to noOfSubjectPanels
		for (int ii = 0; ii<project->get_MySurfaces()->at(kk)->get_MyPanels()->size(); ii++)
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).size(); jj++)
			{
				// Add surface subject panels.
				noOfSubjectPanels++;
			}
		}


		// Add the number of wake panels to noOfSubjectPanels.
		// This takes account of surfaces without wakes.
		int lastGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size();
		for (int ii = 0; ii<lastGeneration; ii++) // Loop over generations.
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).size(); jj++)
			{
				// Add wake subject panels.
				noOfSubjectPanels++;
			}
		}

	}

	int noOfVelocityPredictions = 1; // The total number of velocity field grid points at this generation t.


									 // DOM: Set up allocation of arrays of panels and vertices in host memory.
	size_t sizeOfSubjectPanelsFloat = noOfSubjectPanels * sizeof(float); // Memory required for noOfPanels floats.
	size_t sizeOfSubjectPanelsInt = noOfSubjectPanels * sizeof(int); // Memory required for noOfPanels ints.
	size_t sizeVelocityPredictionsFloat = noOfVelocityPredictions * sizeof(float); // Memory required for noOfVelocityPredictions floats. This is for each (normalised) velocity component.

																				   // Allocate memory for Kernel input data on the host.
																				   // Coordinates of points at which velocities are to be computed.
	float *h_cp_x = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point x-coordinate.
	float *h_cp_y = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point y-coordinate.
	float *h_cp_z = (float *)malloc(sizeVelocityPredictionsFloat); // Allocate the host object-point z-coordinate.

																   // Subject Vortex start point coordinates.
	int noOfVorticesPerPanel = 4;
	float *h_vs_x = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point x-coord.
	float *h_vs_y = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point y-coord.
	float *h_vs_z = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex start point z-coord.

																					// Subject Vortex end point coordinates.
	float *h_ve_x = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point x-coord.
	float *h_ve_y = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point y-coord.
	float *h_ve_z = (float *)malloc(sizeOfSubjectPanelsFloat*noOfVorticesPerPanel); // Vortex end point z-coord.

																					// Allocate memory for vorticity values for each subject panel.
	float *h_vorticities = (float *)malloc(sizeOfSubjectPanelsFloat);

	// Allocate the host output vectors of velocity components for each object point.
	float *h_cp_vx = (float *)malloc(sizeVelocityPredictionsFloat);
	float *h_cp_vy = (float *)malloc(sizeVelocityPredictionsFloat);
	float *h_cp_vz = (float *)malloc(sizeVelocityPredictionsFloat);

	// Verify that allocations succeeded.
	if (h_cp_x == NULL
		|| h_cp_y == NULL
		|| h_cp_z == NULL
		|| h_vs_x == NULL
		|| h_vs_y == NULL
		|| h_vs_z == NULL
		|| h_ve_x == NULL
		|| h_ve_y == NULL
		|| h_ve_z == NULL
		|| h_vorticities == NULL
		|| h_cp_vx == NULL
		|| h_cp_vy == NULL
		|| h_cp_vz == NULL)
	{
		fprintf(stderr, "Failed to allocate host vectors!\n");
		exit(EXIT_FAILURE);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host memory allocated successfully.");

	// Now initialize the host array of object point velocity components (output).
	for (int i = 0; i<noOfVelocityPredictions; i++)
	{
		h_cp_vx[i] = 0.0f;
		h_cp_vy[i] = 0.0f;
		h_cp_vz[i] = 0.0f;
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Host object point velocities initialized successfully.");

	// Now initialize the host arrays of object point position


	// DOM: Assign object point coordinates.	
	h_cp_x[0] = x;
	h_cp_y[0] = y;
	h_cp_z[0] = z;


	// Now initialize the vortex segment end point data for all surface subject panels.

	int subjectSurfacePanelIndex = 0; // Panel index (counts over all the surface panels).

									  // Do the bound vortices.
	for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // DOM: Loop through all the surfaces.
	{
		for (int ii = 0; ii<project->get_MySurfaces()->at(kk)->get_MyPanels()->size(); ii++) // DOM: Loop through the rows of panels on each surface.
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).size(); jj++) // DOM: Loop through the columns of panels on each surface.
			{

				// DOM: Vortex data.

				// DOM: Loop through all the bound vortices on the current panel.
				for (int mm = 0; mm<project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->size(); mm++)
				{

					// DOM: Vortex coordinates.
					h_vs_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_X();
					h_vs_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Y();
					h_vs_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Z();

					h_ve_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_X();
					h_ve_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Y();
					h_ve_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Z();


					// Check for vortices of zero length.
					float epsilon = 0.0001;
					if ((fabs(h_vs_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_x[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_y[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_z[ROW_MAJOR_IDX2C(subjectSurfacePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon))
					{
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Zero length bound vortex detected.");
					}


				} // End of mm loop.

				  // DOM: Host vorticity assignments

				h_vorticities[subjectSurfacePanelIndex] = project->get_MySurfaces()->at(kk)->get_MyPanels()->at(ii).at(jj)->get_MyVorticity();

				// Increment panel index.
				subjectSurfacePanelIndex++;

			} // End of jj loop.

		} // End of ii loop.

	} // End of kk loop.

	  // Now initialize the vortex segment end point data for wake subject panels.

	int subjectWakePanelIndex = subjectSurfacePanelIndex; // Panel index (counts over all the wake panels).

														  // Do the free (wake) vortices.
	for (int kk = 0; kk<project->get_MySurfaces()->size(); kk++) // DOM: Loop through all the surfaces.
	{
		int lastGeneration = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->size();

		for (int ii = 0; ii<lastGeneration; ii++) // DOM: Loop through the generations of wake panels of the current surface.
		{
			for (int jj = 0; jj<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).size(); jj++) // DOM: Loop through the columns of panels on each surface.
			{

				// DOM: Vortex data.

				// DOM: Loop through all the bound vortices on the current panel.
				for (int mm = 0; mm<project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->size(); mm++)
				{

					// DOM: Vortex coordinates.
					h_vs_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_X();
					h_vs_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Y();
					h_vs_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_StartPoint()->get_Z();

					h_ve_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_X();
					h_ve_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Y();
					h_ve_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVortices()->at(mm)->get_EndPoint()->get_Z();


					// Check for vortices of zero length.
					float epsilon = 0.0001;
					if ((fabs(h_vs_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_x[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_y[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon)
						&& (fabs(h_vs_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)] - h_ve_z[ROW_MAJOR_IDX2C(subjectWakePanelIndex, mm, noOfVorticesPerPanel)]) < epsilon))
					{
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Zero length wake vortex detected.");
					}

				} // End of for mm loop.

				  // DOM: Host vorticity assignments

				h_vorticities[subjectWakePanelIndex] = project->get_MySurfaces()->at(kk)->get_MyWakePanels()->at(ii).at(jj)->get_MyVorticity();

				// Increment panel index.
				subjectWakePanelIndex++;

			} // End of jj loop.

		} // End of ii loop.

	} // End of kk loop.

	  // =======================================================================================================================
	  // DOM: Copy data from host to device, do Cuda calculations, and copy data back to host. (CudaCode.cu)
	  // =======================================================================================================================
	ComputeVelocitiesForBatchOfPointsCuda(h_cp_x,
		h_cp_y,
		h_cp_z,
		h_vs_x,
		h_vs_y,
		h_vs_z,
		h_ve_x,
		h_ve_y,
		h_ve_z,
		h_cp_vx,
		h_cp_vy,
		h_cp_vz,
		h_vorticities,
		noOfVorticesPerPanel,
		noOfSubjectPanels,
		noOfVelocityPredictions,
		project->get_RankineAlgorithm().at(0));




	// Extract the velocity data from h_cp_vx, h_cp_vy and h_cp_vz.
	*vx = h_cp_vx[0];
	*vy = h_cp_vy[0];
	*vz = h_cp_vz[0];

	// Free host memory.
	free(h_cp_x);
	free(h_cp_y);
	free(h_cp_z);

	free(h_vs_x);
	free(h_vs_y);
	free(h_vs_z);

	free(h_ve_x);
	free(h_ve_y);
	free(h_ve_z);

	free(h_cp_vx);
	free(h_cp_vy);
	free(h_cp_vz);

	free(h_vorticities);

} // End of calcInducedVelocityAtEdgeMidPointCuda.


