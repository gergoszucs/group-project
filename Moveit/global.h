#ifndef GLOBAL_H
#define GLOBAL_H

#include "moveit.h"
#include "ITProject.h"

extern Moveit* w;
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

// Program mode flags.
enum RUN_MODES_ENUM {MYGUI, MYBATCH};
extern RUN_MODES_ENUM MY_RUN_MODE; // GUI, BATCH.

// Edit mode flags.
enum EDIT_MODES_ENUM {DRAG_TRAJECTORY_POINT, NONE};
extern EDIT_MODES_ENUM MY_EDIT_MODE; 

// Window cintrol flag.
enum WIDGET_CONTROL_ENUM {X, Y, Z, R, P, S, NO};
extern WIDGET_CONTROL_ENUM MY_WIDGET_CONTROL; 

// File management flags.
extern bool UnsavedChanges;
extern bool IsDataLoaded;

// OpenGL view parameters.
extern float glXViewHalfExtent;
extern float glXPanCentreX;
extern float glXPanCentreY;

extern float glYViewHalfExtent;
extern float glYPanCentreX;
extern float glYPanCentreY;

extern float glZViewHalfExtent;
extern float glZPanCentreX;
extern float glZPanCentreY;

extern float gl3DViewHalfExtent;
extern float gl3DPanCentreX;
extern float gl3DPanCentreY;

// Drawing semaphores.
extern bool drawAxes;
extern bool drawControlPoints;
extern bool drawInterpolatedPoints;
extern bool drawAnnotations;
extern bool drawNormals;

#endif /*  GLOBAL_H */