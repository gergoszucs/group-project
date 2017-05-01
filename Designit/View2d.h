#pragma once

#include <QGLWidget>

#include "ITControlPoint.h"

class View2d : public QGLWidget
{
	Q_OBJECT

public:
	explicit View2d(QWidget *parent = 0);
	~View2d(void);

	// Accessors.
	ITControlPoint *get_ScratchControlPoint();
	void set_ScratchControlPoint(ITControlPoint *p);

	void set_plane(PLANE p);

	void setSceneParameters(float eyeX, float eyeY, float eyeZoom);
	void finishEdit();

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	//void setViewOrtho(int width, int height);

	void keyPressEvent(QKeyEvent * event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

private:

	// Drawing parameters.
	int xRot;
	int yRot;
	int zRot;

	float eyeX = 0.0;
	float eyeY = 0.0;
	float eyeZoom = 1.0;

	int myWidth;
	int myHeight;

	QPoint lastPos;

	bool primedForClick = false;

	ITControlPoint *_ScratchControlPoint; // Used for MATCH_POINT mode.
	bool scrachPointReady = false;

	bool singleSelect = false;

	PLANE _plane = XY;

	float angleRotated = 0.0;
	float draggedX = 0.0;
	float draggedY = 0.0;
	float draggedZ = 0.0;

	std::vector< ITControlPoint* > focusedPoints;
	std::vector< ITControlPoint* > focusedPoints_light;

	// Utility methods.
	void findControlPointIndicesNearMouse(double posX, double posY, int *targetK, int *targetI, int *targetJ);
	void getAxesPos(float& pX, float& pY, const int x, const int y);
	void addFocusPoint(ITControlPoint* p);
	void setPointHighlight(QMouseEvent *event);

	// Drawing methods.
	void drawMyAxes();
	void draw();
	void drawMyControlPointsNet();
	void drawMyInterpolatedPointsNet();
	void drawMyFocusControlPoints();
	void drawMyFocusControlPoints_light();
	void drawMyGrids();
	void drawMyScratchControlPoint();
	void drawSphere(double r, int lats, int longs, float R, float GG, float B);
};