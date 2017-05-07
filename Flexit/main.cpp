// Command line argument for batch mode: 
// "C:\Users\e802508\Documents\Visual Studio 2015\Projects\Flexit\Flexit\Data\AX1i_100_frames_no_duplicates.json"

#include "Flexit.h"
#include <QtWidgets/QApplication>
#include "CudaUtilities.cuh"
#include <iostream> 
#include "global.h"

// Global variables.
Flexit* w; // The GUI Object.
ITProject *project;
QString PROGRAM_VERSION = QString("1.0"); // (student pack version) Updated on 20170225.
QString GPUName; // Assumes a single GPU.

// Data file name strings.
QString DataFileNameWithPath;
QString DataFileName;

// Simulation progress variables.
int FrameNumber = 0;
float cummulativeElapsedTimeSeconds = 0.0;

// Simulation semaphores.
bool IsPaused = false;
bool IsSimulating = false;
bool IsStep = false;
bool IsReplay = false;
bool IsDryRun = false;
bool IsTrim = false;
bool IsModeUVLM = true;
bool IsModeEulerBernoulli = false;

/*TODO check srv*/
#ifdef NDEBUG
// Release code.
bool IsScrutiny = false; // Enable HTTP logging.
#else
// Debug code.
bool IsScrutiny = false; // Enable HTTP logging.
#endif // End of DEBUGGING

// Program mode flags.
RUN_MODES_ENUM MY_RUN_MODE; // GUI, BATCH. Note that the enum type itself is defined in global.h.

// File management flags.
bool UnsavedChanges = false;
bool IsDataLoaded = false;

// OpenGL view parameters.
float glViewHalfExtent = 50;
float glPanCentreX = 0.0;
float glPanCentreY = 0.0;

// Drawing semaphores.
bool drawAxes = true;
bool drawControlPoints = true;
bool drawInterpolatedPoints = true;
bool drawTracks = true;
bool drawAnnotations = true;
bool drawPanels = true;
bool drawNormals = true;
bool drawWakes = true;
bool drawVorticities = true;
bool drawBoundVortices = true;
bool drawPressure = true;
bool drawVelocityField = true;
bool drawBeamElements = true;
bool drawBeamGhostElements = true;
bool drawBeamNodes = true;
bool drawBeamDeflectedNodes = true;
bool drawBeamGhostNodes = true;
bool drawBeamLoadVectors = true;
bool drawQuietGraphics = false;
bool drawGust = true;

bool drawTranslateViewWithSurface = false;
bool drawTranslationCompleteSemaphore = true;

bool drawRotateXHorizontal = false;
bool drawRotateYHorizontal = false;
bool drawRotateZVertical = true;

#pragma comment(lib, "cudart") 

using std::endl;
using std::exception;
using std::vector;

static const int MaxSize = 96;

int main(int argc, char *argv[])
{
	// =========================================================================
	// CUDA Test
	// =========================================================================
	bool IsCudaTest = true;
	if (IsCudaTest)
	{
		std::cout << "In main." << endl;
		// Create sample data 
		vector<float> data(MaxSize);
		InitializeData(data);

		// Compute cube on the device 
		vector<float> cube(MaxSize);
		RunCubeKernel(data, cube);

		// Print out results 
		std::cout << "Cube kernel results." << endl << endl;

		for (int i = 0; i < MaxSize; ++i)
		{
			std::cout << cube[i] << endl;
		}

		std::cout << "End of results." << endl << endl;
	}
	// =========================================================================
	// End of CUDA Test
	// =========================================================================

	// Instanciate an ITProject object.
	project = new ITProject();
	project->set_DebugLevel(5);

	// Set the run mode depending on whether or not there are data file argument(s).
	if (argc > 2)
	{
		// There are command line arguments, so we're running in batch mode.
		MY_RUN_MODE = MYBATCH;
	}
	else
	{
		MY_RUN_MODE = MYGUI;
	}

	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "GUI MODE.");
		QApplication a(argc, argv);

		// Create the main application window widget.
		w = new Flexit();

		// Set the decals.
		w->setWindowTitle("Flexit");
		w->statusBar()->showMessage(QObject::tr("Ready"));

		// Set the main window icon.
		QIcon icon(":/Resources/flexit.png");
		w->setWindowIcon(icon);
		w->show();

		// Get details about the GPU.
		// Note that this call needs to be made after MY_RUN_MODE has been initialized, after the window show, and before the Qt loop starts.
		int noOfCudaDevices = GetDeviceCount();
		if (noOfCudaDevices == 0)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 0, "No CUDA devices could be found. noOfCudaDevices: %i", noOfCudaDevices);
			exit(EXIT_FAILURE);
		}

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 0, "noOfCudaDevices: %i", noOfCudaDevices);
		std::string gpuString;
		std::string str1 = InitializeDevice(argc, argv, &gpuString);
		GPUName = QString::fromStdString(gpuString);

		w->setHardwareReportTextEdit(QString::fromStdString(str1));

		// Send HTTP log message.
		w->sendHTTPRequest(QString("Program"), QString("Launched"), 0.0, 0, QString());

		if (argc == 2)
		{
			w->loadData(argv[1]);
		}

		int retcode = a.exec();
	}
	else
	{
		// Do batch mode calculations here.
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "BATCH MODE. Argument: %s", argv[1]);
		w->loadData(QString(argv[1]));
	}
}