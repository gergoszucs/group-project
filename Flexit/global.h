#ifndef GLOBAL_H
#define GLOBAL_H

#include "flexit.h"
#include "ITProject.h"

#define COL_MAJOR_IDX2C(i, j, noOfRows) (((j)*(noOfRows))+(i)) // 0-based column major storage macro (where i is the row index, j is the col index).
#define ROW_MAJOR_IDX2C(i, j, noOfCols) (((i)*(noOfCols))+(j)) // 0-based row major storage macro (where i is the row index, j is the colindex).

#define PI 3.1415926
#define G -9.81

extern Flexit* w;
extern ITProject* project;
extern QString PROGRAM_VERSION;
extern QString GPUName; // Assumes a single GPU.

// Simulation progress variables.
extern int FrameNumber;
extern float cummulativeElapsedTimeSeconds;

// Program mode flags.
enum RUN_MODES_ENUM { MYGUI, MYBATCH };
extern RUN_MODES_ENUM MY_RUN_MODE; // GUI, BATCH.

// File management flags.
extern bool UnsavedChanges;
extern bool IsDataLoaded;

// Drawing semaphores.
extern bool drawRotateXHorizontal;
extern bool drawRotateYHorizontal;
extern bool drawRotateZVertical;

extern bool drawAxes;
extern bool drawControlPoints;
extern bool drawInterpolatedPoints;
extern bool drawTracks;
extern bool drawAnnotations;
extern bool drawPanels;
extern bool drawNormals;
extern bool drawWakes;
extern bool drawVorticities;
extern bool drawBoundVortices;
extern bool drawPressure;
extern bool drawVelocityField;
extern bool drawBeamElements;
extern bool drawBeamGhostElements;
extern bool drawBeamNodes;
extern bool drawBeamDeflectedNodes;
extern bool drawBeamGhostNodes;
extern bool drawBeamLoadVectors;
extern bool drawQuietGraphics;
extern bool drawGust;

extern bool drawTranslateViewWithSurface;
extern bool drawTranslationCompleteSemaphore;

// OpenGL view parameters.
extern float glViewHalfExtent;
extern float glPanCentreX;
extern float glPanCentreY;

// Simulation semaphores.
extern bool IsPaused;
extern bool IsSimulating;
extern bool IsStep;
extern bool IsReplay;
extern bool IsDryRun;
extern bool IsTrim;
extern bool IsModeUVLM;
extern bool IsModeEulerBernoulli;
extern bool IsScrutiny;

// Data file name strings.
extern QString DataFileNameWithPath;
extern QString DataFileName;

#endif /*  GLOBAL_H */