#pragma once

#include <QGLWidget>

#include "ITControlPoint.h"

class MyXZView : public QGLWidget
{
	Q_OBJECT

public:

	explicit MyXZView(QWidget *parent = 0);
	~MyXZView(void);

	void synchronizeBaseSurfaceCoordinates();
	void centredRotateFocusPoints(float dTheta);

	// Accessors.
	float get_EditValue();
	void set_EditValue(float a);

	float get_EditValueX();
	void set_EditValueX(float a);

	float get_EditValueY();
	void set_EditValueY(float a);

	int get_Sense();
	void set_Sense(int a);

	bool get_PrimedForFirstClick();
	void set_PrimedForFirstClick(bool p);

	bool get_PrimedForSecondClick();
	void set_PrimedForSecondClick(bool p);

	bool get_SecondClicksFinished();
	void set_SecondClicksFinished(bool p);

	ITControlPoint *get_ScratchControlPoint();
	void set_ScratchControlPoint(ITControlPoint *p);

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void setViewOrtho(int width, int height);
	void keyPressEvent(QKeyEvent * event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:

	// Drawing parameters.
	int xRot;
	int yRot;
	int zRot;

	int myWidth;
	int myHeight;

	QPoint lastPos;

	float _EditValue;
	float _EditValueX;
	float _EditValueY;

	int _Sense;

	bool _PrimedForFirstClick;
	bool _PrimedForSecondClick;
	bool _SecondClicksFinished;

	ITControlPoint *_ScratchControlPoint; // Used for MATCH_POINT mode.

	// Drawing methods.
	void drawMyAxes();
	void draw();
	void drawMyControlPointsNet();
	void drawMyInterpolatedPointsNet();
	void drawMyFocusControlPoints();
	void drawMyAnnotations();
	void drawMyGrids();
	void drawMyScratchControlPoint();
	void drawSphere(double r, int lats, int longs, float R, float GG, float B);

	void findControlPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI, int *targetJ);
	void AssignFocusPoint(QMouseEvent *event);
};