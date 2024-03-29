#ifndef GLOBAL_H
#define GLOBAL_H

#include "designit.h"
#include "ITProject.h"

#define COL_MAJOR_IDX2C(i, j, noOfRows) (((j)*(noOfRows))+(i)) // 0-based column major storage macro (where i is the row index, j is the col index).
#define ROW_MAJOR_IDX2C(i, j, noOfCols) (((i)*(noOfCols))+(j)) // 0-based row major storage macro (where i is the row index, j is the colindex).

#define PI 4 * atan( 1.0 )
#define G -9.81

#define DEG_TO_RAD(x) x / 180 * M_PI
#define RAD_TO_DEG(x) x / M_PI * 180

extern Designit* w;
extern ITProject* project;
extern bool IsScrutiny;
extern QString PROGRAM_VERSION;

// Simulation progress variables.
extern int FrameNumber;

// Simulation semaphores.
extern bool IsDryRun;

// Data file name strings.
extern QString DataFileNameWithPath;
extern QString DataFileName;

// Program name strings.
extern QString ProgramNameWithPath;
extern QString ProgramName;

// Program mode flags.
enum RUN_MODES_ENUM {MYGUI, MYBATCH};
extern RUN_MODES_ENUM MY_RUN_MODE; // GUI, BATCH.

// File management flags.
extern bool UnsavedChanges;
extern bool IsDataLoaded;

// OpenGL view parameters.
extern float glGaussianViewHalfExtent;
extern float glGaussianPanCentreX;
extern float glGaussianPanCentreY;

// Drawing semaphores.
extern bool drawRotateXHorizontal;
extern bool drawRotateYHorizontal;
extern bool drawRotateZVertical;

extern bool drawAxes;
extern bool drawControlPoints;
extern bool drawInterpolatedPoints;
extern bool drawAnnotations;
extern bool drawNormals;
extern bool drawGrids;

extern bool IsVerticalDragOnly;
extern bool IsHorizontalDragOnly;

extern bool trajectoryMode;



// Editing mode flags.
enum EDIT_MODES_ENUM 
{	
	NONE, 
	DRAG, 
	RESIZE, 
	ROTATE, 
	CENTRED_ROTATE,
	SHEAR, 
	FLIP,
	COPY_SURFACE,
	DELETE_SURFACE,
	INSERT_ROW,
	DELETE_ROW,
	DUPLICATE_ROW,
	INSERT_COL,
	DELETE_COL,
	DUPLICATE_COL,
	MATE_POINTS,
	COPY_SURFACE_MIRROR,
	MERGE_SURFACES_BY_ROW,
	MERGE_SURFACES_BY_ROW_REVERSE,
	MEASURE_DISTANCE,
	DRAG_TRAJECTORY_POINT
};

extern EDIT_MODES_ENUM MY_EDIT_MODE; // GUI, BATCH.

// Window cintrol flag.
enum WIDGET_CONTROL_ENUM { X, Y, Z, R, P, S, NO };
extern WIDGET_CONTROL_ENUM MY_WIDGET_CONTROL;

// Selection mode.
enum SELECT_MODE
{
	POINT_M,
	ROW_M,
	COLUMN_M,
	SURFACE_M
};

extern SELECT_MODE _selectMode;

class Point3;

#endif /*  GLOBAL_H */