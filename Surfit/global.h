#ifndef GLOBAL_H
#define GLOBAL_H

#include "surfit.h"
#include "ITProject.h"

#define COL_MAJOR_IDX2C(i, j, noOfRows) (((j)*(noOfRows))+(i)) // 0-based column major storage macro (where i is the row index, j is the col index).
#define ROW_MAJOR_IDX2C(i, j, noOfCols) (((i)*(noOfCols))+(j)) // 0-based row major storage macro (where i is the row index, j is the colindex).

#define PI 3.1415926
#define G -9.81

extern Surfit* w;
extern ITProject* project;
extern bool IsScrutiny;
extern QString PROGRAM_VERSION;

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
extern float glXYViewHalfExtent;
extern float glXYPanCentreX;
extern float glXYPanCentreY;

extern float glXZViewHalfExtent;
extern float glXZPanCentreX;
extern float glXZPanCentreY;

extern float glYZViewHalfExtent;
extern float glYZPanCentreX;
extern float glYZPanCentreY;

extern float glGaussianViewHalfExtent;
extern float glGaussianPanCentreX;
extern float glGaussianPanCentreY;

extern float gl3DViewHalfExtent;
extern float gl3DPanCentreX;
extern float gl3DPanCentreY;

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

// Editing mode flags.
enum EDIT_MODES_ENUM {NONE, 
					  DRAG, 
					  DRAG_ROW, 
					  DRAG_COL, 
					  DRAG_ALL, 
					  RESIZE_ALL, 
					  ROTATE_ALL, 
					  SHEAR_ALL, 
					  PERSPECTIVE_ALL, 
					  FLIP_HORIZONTAL_ALL,
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
					  CENTRED_ROTATE};

extern EDIT_MODES_ENUM MY_EDIT_MODE; // GUI, BATCH.


#endif /*  GLOBAL_H */