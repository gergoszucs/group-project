#include <QtWidgets/QApplication>
#include "global.h"
#include "designit.h"

// My app was not linking to cudart.lib. Adding cudart.lib to 
// Properties > Configuration Properties > Linker > Input > Additional Dependencies
// solves the problem. See
// https://devtalk.nvidia.com/default/topic/415244/error-lnk2019-unresolved-external-symbol/
// vs2012 Qt solution created using:
// https://codeyarns.com/2013/12/13/how-to-create-qt-applications-using-visual-studio/
// Add to SVN:
// https://tortoisesvn.net/docs/release/TortoiseSVN_en/tsvn-dug-import.html#tsvn-dug-import-in-place
// The vs2012 solution was configured for CUDA using:
// https://code.msdn.microsoft.com/windowsdesktop/CUDA-50-and-Visual-Studio-20e71aa1
// Guidance about deployment:
// https://msdn.microsoft.com/en-us/library/zebw5zk9.aspx
// Memory leak detection using CRT:
// https://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
// Application window icon. (And set the app icon in main.cpp)
// http://doc.qt.io/qt-4.8/appicon.html
// Emailing:
// http://forums.codeguru.com/showthread.php?300530-how-to-send-mails-from-VC-program-without-using-outlook
// Deploying a QT application. Use windeployqt.
// http://stackoverflow.com/questions/22185974/deploying-qt-5-app-on-windows
// Fixed width layout withing an expanding layout.
// http://g.sweyla.com/blog/2010/qt-layouts-both-fixed-and-stretched/
// CURL code for sending emails.
// http://www.codeproject.com/Questions/714669/Sending-Email-with-attachment-using-libcurl

// Global variables.
Designit* w;
ITProject *project;
bool IsScrutiny = false; // Enable HTTP logging.
QString PROGRAM_VERSION = QString("0.1.100");

// Simulation progress variables.
int FrameNumber = 0;

// Simulation semaphores.
bool IsDryRun = false;

bool trajectoryMode = false;

// Data file name strings.
QString DataFileNameWithPath;
QString DataFileName;

// Program name strings.
QString ProgramNameWithPath;
QString ProgramName;

// Program mode flags.
RUN_MODES_ENUM MY_RUN_MODE; // GUI, BATCH. Note that the enum type itself is defined in global.h.

// Edit mode flags.
EDIT_MODES_ENUM MY_EDIT_MODE; // NONE, DRAG, SCALE, ROTATE. Note that the enum type itself is defined in global.h.

// File management flags.
bool UnsavedChanges = false;
bool IsDataLoaded = false;

// OpenGL view parameters.
float glGaussianViewHalfExtent = 50.0;
float glGaussianPanCentreX = 0.0;
float glGaussianPanCentreY = 0.0;

WIDGET_CONTROL_ENUM MY_WIDGET_CONTROL;

// Drawing semaphores.
bool drawRotateXHorizontal = false;
bool drawRotateYHorizontal = false;
bool drawRotateZVertical = true;

// Drawing semaphores.
bool drawAxes = true;
bool drawControlPoints = true;
bool drawInterpolatedPoints = true;
bool drawAnnotations = true;
bool drawNormals = true;
bool drawGrids = true;

bool IsVerticalDragOnly = false;
bool IsHorizontalDragOnly = false;

SELECT_MODE _selectMode = POINT_M;

int main(int argc, char *argv[])
{
	// Instanciate an ITproject object.
	project = new ITProject();
	project->set_DebugLevel(5);

	// Set the run mode depending on whether or not there are data file argument(s).
	if (argc > 1)
	{
		// There are command line arguments, so we're running in batch mode.
		MY_RUN_MODE = MYBATCH;
	}
	else
	{
		MY_RUN_MODE = MYGUI;
	}

	MY_EDIT_MODE = NONE;

	if (MY_RUN_MODE == MYGUI)
	{
		MY_EDIT_MODE = NONE;
		MY_WIDGET_CONTROL = NO;

		QApplication a(argc, argv);
		w = new Designit();

		// Set the decals.
		w->setWindowTitle("DesignIT");
		w->statusBar()->showMessage(QObject::tr("Ready"));

		// Set the main window icon.
		QIcon icon(":/Resources/designit.png");
		w->setWindowIcon(icon);

		w->show();

		return a.exec();
	}
}